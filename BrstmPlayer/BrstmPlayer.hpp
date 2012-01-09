// BrstmPlayer.hpp
// Nolan Check
// Created 11/30/2011

#ifndef _BRSTMPLAYER_HPP
#define _BRSTMPLAYER_HPP

#include <Windows.h>

#include <memory>

#include "AudioOutput.hpp"
#include "BrstmFile.hpp"
#include "File.hpp"

class BrstmPlayer;
typedef std::shared_ptr<BrstmPlayer> BrstmPlayerPtr;

class BrstmPlayer
{

public:

	static BrstmPlayerPtr Create();
	~BrstmPlayer();

	bool Open(FilePtr file);
	bool Play();
	void Stop();

private:

	BrstmPlayer();
	bool CreateInternal();
	
	enum PlayerThreadCmd
	{
		PTC_NONE,
		PTC_EXIT
	};
	bool SendPlayerThreadCmd(PlayerThreadCmd cmd);

	static unsigned int __stdcall PlayerThread(void* arglist);
	PlayerThreadCmd m_playerThreadCmd;
	HANDLE m_hPlayerThreadCmdEvent;
	HANDLE m_hPlayerThreadAckEvent;

	BrstmFilePtr m_brstmFile;
	AudioOutputPtr m_audioOutput;
	HANDLE m_hThread;

};

#endif
