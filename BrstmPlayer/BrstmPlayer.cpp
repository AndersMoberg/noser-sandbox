// BrstmPlayer.cpp
// Nolan Check
// Created 11/30/2011

#include "BrstmPlayer.hpp"

#include <process.h>

#include <vector>

BrstmPlayerPtr BrstmPlayer::Create()
{
	BrstmPlayerPtr result(new BrstmPlayer);
	if (!result->CreateInternal()) {
		return NULL;
	}
	return result;
}

BrstmPlayer::BrstmPlayer()
	: m_hThread(NULL),
	m_hPlayerThreadCmdEvent(NULL),
	m_hPlayerThreadAckEvent(NULL),
	m_playerThreadCmd(PTC_NONE)
{ }

BrstmPlayer::~BrstmPlayer()
{
	if (m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
	if (m_hPlayerThreadAckEvent)
	{
		CloseHandle(m_hPlayerThreadAckEvent);
		m_hPlayerThreadAckEvent = NULL;
	}
	if (m_hPlayerThreadCmdEvent)
	{
		CloseHandle(m_hPlayerThreadCmdEvent);
		m_hPlayerThreadCmdEvent = NULL;
	}
}

bool BrstmPlayer::CreateInternal()
{
	m_audioOutput = AudioOutput::Create();
	if (!m_audioOutput) {
		return false;
	}

	m_hPlayerThreadCmdEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_hPlayerThreadCmdEvent) {
		return false;
	}
	m_hPlayerThreadAckEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return true;
}

bool BrstmPlayer::Open(FilePtr file)
{
	Stop();

	m_brstmFile = BrstmFile::Create(file);
	if (!m_brstmFile) {
		return false;
	}

	return true;
}

bool BrstmPlayer::Play()
{
	if (!m_hThread)
	{
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, PlayerThread, this, 0, NULL);
		if (!m_hThread) {
			return false;
		}

		// Send PTC_NONE to ensure that opening succeeded
		if (!SendPlayerThreadCmd(PTC_NONE)) {
			return false;
		}
	}

	return true;
}

void BrstmPlayer::Stop()
{
	if (m_hThread)
	{
		SendPlayerThreadCmd(PTC_EXIT);
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	m_brstmFile.reset();
}

bool BrstmPlayer::SendPlayerThreadCmd(PlayerThreadCmd cmd)
{
	m_playerThreadCmd = cmd;
	SetEvent(m_hPlayerThreadCmdEvent);

	HANDLE waits[2] = { m_hPlayerThreadAckEvent, m_hThread };
	if (WaitForMultipleObjects(2, waits, FALSE, INFINITE) != WAIT_OBJECT_0)
	{
		m_playerThreadCmd = PTC_NONE;
		return false;
	}

	m_playerThreadCmd = PTC_NONE;
	return true;
}

static const int NUM_BUFFERS = 4;

unsigned int __stdcall BrstmPlayer::PlayerThread(void* arglist)
{
	BrstmPlayer* pThis = (BrstmPlayer*)arglist;

	if (FAILED(CoInitialize(NULL))) {
		return 1;
	}
	
	// Set up buffers

	size_t decodedBlockSampleCount = pThis->m_brstmFile->GetDecodedBlockSampleCount();
	std::vector<int16_t> buffers[NUM_BUFFERS];
	for (int i = 0; i < NUM_BUFFERS; ++i) {
		buffers[i].resize(decodedBlockSampleCount);
	}
	int currentBuffer = 0;

	if (!pThis->m_brstmFile->OpenVoice(pThis->m_audioOutput)) {
		return 1;
	}

	// Queue up buffers
	for (int i = 0; i < NUM_BUFFERS; ++i)
	{
		size_t decodedSamples;
		if (!pThis->m_brstmFile->DecodeNextBlock(&buffers[i][0],
			decodedBlockSampleCount, 0, decodedSamples)) {
			return 1;
		}
		if (!pThis->m_audioOutput->SubmitBuffer(&buffers[i][0], decodedSamples*2)) {
			return 1;
		}
	}

	HANDLE bufferEndEvent = pThis->m_audioOutput->GetBufferEndEvent();

	// Play!
	if (!pThis->m_audioOutput->Play()) {
		return 1;
	}

	for (;;)
	{
		HANDLE waits[2] = { pThis->m_hPlayerThreadCmdEvent, bufferEndEvent };
		DWORD waitResult = WaitForMultipleObjects(2, waits, FALSE, INFINITE);

		if (waitResult == WAIT_OBJECT_0) // Player thread command event
		{
			switch (pThis->m_playerThreadCmd)
			{
			case PTC_NONE:
				break;
			case PTC_EXIT:
				SetEvent(pThis->m_hPlayerThreadAckEvent);
				goto exit;
			}
		
			SetEvent(pThis->m_hPlayerThreadAckEvent);
		}
		else if (waitResult == WAIT_OBJECT_0+1) // Buffer end event
		{
			// Submit next buffer
			size_t decodedSamples;
			if (!pThis->m_brstmFile->DecodeNextBlock(&buffers[currentBuffer][0],
				decodedBlockSampleCount, 0, decodedSamples)) {
				return 1;
			}
			if (!pThis->m_audioOutput->SubmitBuffer(&buffers[currentBuffer][0], decodedSamples*2)) {
				return 1;
			}

			++currentBuffer;
			if (currentBuffer >= NUM_BUFFERS) {
				currentBuffer = 0;
			}
		}
		else // Error
		{
			return 1;
		}
	}
exit:

	if (!pThis->m_audioOutput->Stop()) {
		return 1;
	}
	
	return 0;
}
