// AudioOutput.cpp
// Nolan Check
// Created 11/30/2011

#include "AudioOutput.hpp"

template<class Interface>
inline void SafeRelease(Interface*& pInterface)
{
	if (pInterface)
	{
		pInterface->Release();
		pInterface = NULL;
	}
}

AudioOutputPtr AudioOutput::Create()
{
	AudioOutputPtr result(new AudioOutput);
	if (!result->CreateInternal()) {
		return NULL;
	}
	return result;
}

AudioOutput::AudioOutput()
	: m_pXAudio2(NULL),
	m_pMasteringVoice(NULL),
	m_pSourceVoice(NULL)
{ }

AudioOutput::VoiceCallback::VoiceCallback()
	: m_hBufferEndEvent(NULL)
{ }

AudioOutput::~AudioOutput()
{
	if (m_voiceCallback.m_hBufferEndEvent != NULL)
	{
		CloseHandle(m_voiceCallback.m_hBufferEndEvent);
		m_voiceCallback.m_hBufferEndEvent = NULL;
	}
	SafeRelease(m_pXAudio2);
}

bool AudioOutput::CreateInternal()
{
	HRESULT hr;

	hr = XAudio2Create(&m_pXAudio2);
	if (FAILED(hr)) {
		return false;
	}

	m_voiceCallback.m_hBufferEndEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_voiceCallback.m_hBufferEndEvent) {
		return false;
	}

	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

bool AudioOutput::OpenVoice(const WAVEFORMATEX* pSourceFormat)
{
	HRESULT hr;

	if (m_pSourceVoice)
	{
		m_pSourceVoice->DestroyVoice();
		m_pSourceVoice = NULL;
	}

	hr = m_pXAudio2->CreateSourceVoice(&m_pSourceVoice, pSourceFormat,
		XAUDIO2_VOICE_NOPITCH, XAUDIO2_DEFAULT_FREQ_RATIO, &m_voiceCallback);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

bool AudioOutput::Play()
{
	HRESULT hr;

	hr = m_pSourceVoice->Start();
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

bool AudioOutput::Stop()
{
	HRESULT hr;

	hr = m_pSourceVoice->Stop();
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

bool AudioOutput::SubmitBuffer(const void* src, size_t size)
{
	HRESULT hr;

	XAUDIO2_BUFFER buf = { 0 };
	buf.AudioBytes = size;
	buf.pAudioData = (const BYTE*)src;
	hr = m_pSourceVoice->SubmitSourceBuffer(&buf);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

void AudioOutput::VoiceCallback::OnBufferEnd(void* pBufferContext)
{
	SetEvent(m_hBufferEndEvent);
}
