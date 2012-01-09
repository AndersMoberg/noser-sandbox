// BrstmFile.cpp
// Nolan Check
// Created 12/1/2011

#include "BrstmFile.hpp"

#include <cstdint>
#include <iostream>
#include <vector>

#define CONST_BE32(a) (((a >> 24) & 0xFF) | ((a >> 8) & 0xFF00) | ((a << 8) & 0xFF0000) | ((a << 24) & 0xFF000000))

BrstmFilePtr BrstmFile::Create(FilePtr file)
{
	BrstmFilePtr result(new BrstmFile);
	if (!result->CreateInternal(file)) {
		return NULL;
	}
	return result;
}

BrstmFile::BrstmFile()
{ }

bool BrstmFile::CreateInternal(FilePtr file)
{
	m_file = file;

	uint32_t rstmTag;
	if (!m_file->Read32(0x0, &rstmTag)) { return false; }
	if (rstmTag != CONST_BE32(0x5253544D)) { // 'RSTM'
		return false;
	}

	uint32_t magic;
	if (!m_file->Read32(0x4, &magic)) { return false; }
	if (magic != CONST_BE32(0xFEFF0100)) {
		return false;
	}

	uint32_t headOffset;
	if (!m_file->Read32BE(0x10, &headOffset)) { return false; }

	uint32_t headTag;
	if (!m_file->Read32(headOffset+0x0, &headTag)) { return false; }
	if (headTag != CONST_BE32(0x48454144)) {
		return false;
	}

	if (!m_file->Read8(headOffset+0x20, &m_codecNumber)) { return false; }
	std::cout << "Codec number: 0x" << std::hex << (int)m_codecNumber << std::dec << std::endl;
	if (!m_file->Read8(headOffset+0x21, &m_loopFlag)) { return false; }
	std::cout << "Loop flag: " << (int)m_loopFlag << std::endl;
	if (!m_file->Read8(headOffset+0x22, &m_numChannels)) { return false; }
	std::cout << "Number of channels: " << (int)m_numChannels << std::endl;
	if (!m_file->Read16BE(headOffset+0x24, &m_sampleRate)) { return false; }
	std::cout << "Sample rate: " << m_sampleRate << std::endl;
	if (!m_file->Read32BE(headOffset+0x2C, &m_numSamples)) { return false; }
	std::cout << "Number of samples: " << m_numSamples << std::endl;

	// At headOffset+0x30, there is the file offset of the beginning of sound data.
	if (!m_file->Read32BE(headOffset+0x30, &m_initialFileCursor)) { return false; }

	m_fileCursor = m_initialFileCursor;
	m_sampleCursor = 0;

	if (!m_file->Read32BE(headOffset+0x38, &m_blockSize)) { return false; }
	std::cout << "Block size: " << m_blockSize << std::endl;
	if (!m_file->Read32BE(headOffset+0x48, &m_finalBlockSize)) { return false; }
	std::cout << "Final block size: " << m_finalBlockSize << std::endl;

	m_decodedBlockSampleCount = (m_blockSize/8)*14 * (m_numChannels == 1 ? 1 : 2);

	// Read ADPCM coefficients

	uint32_t coefOffset1;
	if (!m_file->Read32BE(headOffset+0x1C, &coefOffset1)) { return false; }
	uint32_t coefOffset2;
	if (!m_file->Read32BE(headOffset+0x10+coefOffset1, &coefOffset2)) { return false; }
	uint32_t coefOffset = coefOffset2+0x10;
	m_channels.resize(m_numChannels);
	int coefSpacing = 0x38;
	for (int i = 0; i < m_numChannels; ++i)
	{
		for (int j = 0; j < 16; ++j)
		{
			if (!m_file->Read16BE(headOffset+coefOffset+i*coefSpacing+j*2, (uint16_t*)&m_channels[i].coeffs[j])) {
				return false;
			}
		}
	}

	return true;
}

bool BrstmFile::OpenVoice(AudioOutputPtr audioOutput)
{
	WAVEFORMATEX wfx = { 0 };
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = (m_numChannels == 1 ? 1 : 2);
	wfx.nSamplesPerSec = m_sampleRate;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = (wfx.nChannels * wfx.wBitsPerSample) / 8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	wfx.cbSize = 0;

	for (int i = 0; i < m_numChannels; ++i)
	{
		m_channels[i].hist1 = 0;
		m_channels[i].hist2 = 0;
	}

	return audioOutput->OpenVoice(&wfx);
}

size_t BrstmFile::GetDecodedBlockSampleCount()
{
	return m_decodedBlockSampleCount;
}

bool BrstmFile::DecodeNextBlock(int16_t* dst, size_t dstSamples, int track,
	size_t& decodedSamples)
{
	decodedSamples = 0;

	if (m_numChannels == 1) // Mono
	{
		// TODO: Implement
	}
	else // Stereo
	{
		// TODO: Handle end of song
		bool atFinalBlock = (m_sampleCursor + m_decodedBlockSampleCount/2 >= m_numSamples);
		uint32_t blockSize = atFinalBlock ? m_finalBlockSize : m_blockSize;
		std::vector<uint8_t> block(blockSize);
		
		uint32_t trackCursor = m_fileCursor + 2*track * blockSize;

		// Left channel
		if (!m_file->Read(trackCursor, &block[0], blockSize)) {
			return false;
		}
		if (!DecodeChannel(dst, dstSamples, 2, &block[0], blockSize,
			m_channels[2*track+0])) {
			return false;
		}

		// Right channel
		if (!m_file->Read(trackCursor+blockSize, &block[0], blockSize)) {
			return false;
		}
		if (!DecodeChannel(dst+1, dstSamples-1, 2, &block[0], blockSize,
			m_channels[2*track+1])) {
			return false;
		}

		decodedSamples = (blockSize/8)*14 * 2;

		if (atFinalBlock) {
			m_fileCursor = m_initialFileCursor;
			m_sampleCursor = 0;
		} else {
			m_fileCursor += blockSize * m_numChannels;
			m_sampleCursor += decodedSamples / 2;
		}
	}

	return true;
}

static int16_t clamp16(int i) {
	return (i < -32768) ? -32768 : ((i > 32767) ? 32767 : i);
}

bool BrstmFile::DecodeChannel(int16_t* dst, size_t dstSamples,
	size_t channelSpacing, const uint8_t* src, size_t srcSize,
	Channel& channel)
{
	size_t dstCursor = 0;

	for (size_t packet = 0; packet < srcSize/8; ++packet)
	{
		const uint8_t* packetSrc = &src[packet*8];

		uint8_t header = packetSrc[0];

		int scale = 1 << (header & 0xF);
		int coefIndex = header >> 4;

		// XXX: What the hell? vgmstream reads 16 coefficients from the block, but
		// has a full range of 32 coefficients during decoding!
		coefIndex &= 0x7;
	
		int coef1 = channel.coeffs[2*coefIndex+0];
		int coef2 = channel.coeffs[2*coefIndex+1];

		// Each packet contains 14 samples
		for (size_t i = 0; i < 14; ++i)
		{
			if (dstCursor >= dstSamples) {
				return false;
			}

			uint8_t sampleByte = packetSrc[1+i/2];
			int sample;
			if (i & 1) {
				sample = (int8_t)((sampleByte << 4) & 0xF0) / 16;
			} else {
				sample = (int8_t)(sampleByte & 0xF0) / 16;
			}

			dst[dstCursor] = clamp16((((sample * scale) << 11) + 1024 + (coef1*channel.hist1 + coef2*channel.hist2)) >> 11);

			channel.hist2 = channel.hist1;
			channel.hist1 = dst[dstCursor];

			dstCursor += channelSpacing;
		}
	}

	return true;
}
