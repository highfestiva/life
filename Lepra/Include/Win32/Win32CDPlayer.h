/*
	Class:  Win32CDPlayer
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#ifndef WIN32CDPLAYER_H
#define WIN32CDPLAYER_H

#include "../CDPlayer.h"
#include "../HashTable.h"
#include "../Log.h"
#include "../String.h"

#include <math.h>
#pragma warning(disable: 4201)	// Nonstandard extention: unnamed struct (Mmsystem.h).
#include <Mmsystem.h>

namespace Lepra
{

class Win32CDPlayer : public CDPlayer
{
public:

	friend class PlayData;
	friend class CDPlayer;

	void SetDrive(tchar pDrive);

	bool Open();
	void Close();
	bool IsOpened();
	bool IsReady();

	int GetNumTracks();
	float64 GetMediaLength();

	// Returns erroneous values once in a while.
	// What can we do about that?
	float64 GetCurrentPos();

	int GetCurrentTrack();
	float64 GetTrackLength(int pTrack);
	float64 GetTrackPos(int pTrack);

	bool Play(int pTrack, CDPlayMode pMode, SynchMode pSynchMode = ASYNCH);
	bool Stop();
	bool Pause();
	bool Resume();
	bool IsPaused();
	bool IsPlaying();
	bool SetVolume(float64 pVolume);
	
	bool Seek(float64 pPosition);

	// Only works while CD is closed.
	bool OpenTray();
	bool CloseTray();
protected:
private:

	Win32CDPlayer(tchar pDrive);
	virtual ~Win32CDPlayer();

	bool OpenCD(const String& pFunction);
	void CloseCD();

	// Initializes time format to milliseconds.
	bool InitTimeFormat(const String& pFunction);

	MCI_STATUS_PARMS mMCIStatus;
	MCI_OPEN_PARMS mMCIOpen;

	tchar mDrive;
	bool mOpened;

	static LogDecorator mLog;
};

} // End namespace.

#endif
