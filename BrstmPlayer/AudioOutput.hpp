// AudioOutput.hpp
// Nolan Check
// Created 11/30/2011

#ifndef _AUDIOOUTPUT_HPP
#define _AUDIOOUTPUT_HPP

#include <XAudio2.h>

#include <memory>

class AudioOutput;
typedef std::shared_ptr<AudioOutput> AudioOutputPtr;

class AudioOutput
{

public:

	static AudioOutputPtr Create();
	~AudioOutput();

	bool OpenVoice(const WAVEFORMATEX* pSourceFormat);
	HANDLE GetBufferEndEvent() { return m_voiceCallback.m_hBufferEndEvent; }
	bool Play();
	bool Stop();
	bool SubmitBuffer(const void* src, size_t size);

private:

	AudioOutput();
	bool CreateInternal();

	class VoiceCallback : public IXAudio2VoiceCallback
	{
	public:
		VoiceCallback();
		void __stdcall OnBufferEnd(void* pBufferContext);
		void __stdcall OnBufferStart(void* pBufferContext) { }
		void __stdcall OnLoopEnd(void* pBufferContext) { }
		void __stdcall OnStreamEnd() { }
		void __stdcall OnVoiceError(void* pBufferContext, HRESULT Error) { }
		void __stdcall OnVoiceProcessingPassEnd() { }
		void __stdcall OnVoiceProcessingPassStart(UINT32 BytesRequired) { }
		HANDLE m_hBufferEndEvent;
	};

	IXAudio2* m_pXAudio2;
	IXAudio2MasteringVoice* m_pMasteringVoice;
	IXAudio2SourceVoice* m_pSourceVoice;
	VoiceCallback m_voiceCallback;

};

#endif
