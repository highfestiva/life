/*
	Class:  PosixCDPlayer
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	This class doesn't really have anything to do with posix... And it
	only supports ATAPI-drives at the moment.

	I used the information on the following page to implement this:

	http://www.cosc.brocku.ca/~cspress/HelloWorld/1999/04-apr/music_cd_linux.html
*/

#ifndef POSIXCDPLAYER_H
#define POSIXCDPLAYER_H

#include "../CDPlayer.h"
#include "../Thread.h"
#include "../String.h"

#include <math.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>

namespace Lepra
{

class PosixCDPlayer : public CDPlayer
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

	bool OpenTray();
	bool CloseTray();
protected:
private:
	
	PosixCDPlayer(tchar pDrive);
	virtual ~PosixCDPlayer();

	bool ReadTrackInfo();
	
	// Initializes time format to milliseconds.
	bool InitTimeFormat(const String& pFunction);

	float64 MSFToSeconds(cdrom& pAddr);
	cdrom SecondsToMSF(float64 pSeconds);
	
	int mFD; // File descriptor of the cdrom device file.
	cdrom mCDHeader; // Contains first and last track index.
	cdrom* mTrackInfo; // Contains info about each track.

	tchar mDrive;
	bool mOpened;
	bool mPlaying;
	bool mPaused;
	
	String mDevice;
};

} // End namespace.

#endif
