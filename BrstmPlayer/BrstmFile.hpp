// BrstmFile.hpp
// Nolan Check
// Created 12/1/2011

#ifndef _BRSTMFILE_HPP
#define _BRSTMFILE_HPP

#include <cstdint>
#include <memory>
#include <vector>

#include "AudioOutput.hpp"
#include "File.hpp"

class BrstmFile;
typedef std::shared_ptr<BrstmFile> BrstmFilePtr;

class BrstmFile
{

public:

	static BrstmFilePtr Create(FilePtr file);

	bool OpenVoice(AudioOutputPtr audioOutput);
	size_t GetDecodedBlockSampleCount();
	bool DecodeNextBlock(int16_t* dst, size_t dstSamples, int track,
		size_t& decodedSamples);

private:

	BrstmFile();
	bool CreateInternal(FilePtr file);
	
	struct Channel
	{
		int16_t coeffs[16];
		int16_t hist1;
		int16_t hist2;
	};

	bool DecodeChannel(int16_t* dst, size_t dstSamples, size_t channelSpacing,
		const uint8_t* src, size_t srcSize, Channel& channel);

	FilePtr m_file;
	uint8_t m_codecNumber;
	uint8_t m_loopFlag;
	uint8_t m_numChannels;
	uint16_t m_sampleRate;
	uint32_t m_numSamples;
	uint32_t m_initialFileCursor;

	uint32_t m_fileCursor;
	size_t m_sampleCursor;
	uint32_t m_blockSize;
	uint32_t m_finalBlockSize;
	size_t m_decodedBlockSampleCount;

	std::vector<Channel> m_channels;

};

#endif
