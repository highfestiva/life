/*
	Class:  Win32CDPlayer
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	TODO: Ditch this class or make it (and base class) portable by
	enumerating CD drives. Will never be used within a game, so
	getting rid of it is preferred?
*/

#include "../../Include/Win32/Win32CDPlayer.h"
#include "../../Include/Log.h"

#include <Windows.h>

#pragma comment(lib, "winmm.lib")

namespace Lepra
{

int gDeviceID = 0;

CDPlayer* CDPlayer::CreateCDPlayer(tchar pDrive)
{
	return new Win32CDPlayer(pDrive);
}

bool CDPlayer::GetFirstDrive(tchar& pDrive)
{
	MCI_OPEN_PARMS lOpenParms;

	gDeviceID = _T('c');
	String lString(StringUtility::Format(_T("%c:\\"), gDeviceID));
	lOpenParms.wDeviceID = 0;
	lOpenParms.lpstrElementName = lString.c_str();
	lOpenParms.lpstrDeviceType = (LPCTSTR)MCI_DEVTYPE_CD_AUDIO;
	lOpenParms.dwCallback = 0;

	while (gDeviceID <= 'z' &&
		  mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT, (DWORD_PTR)&lOpenParms) != 0)
	{
		gDeviceID++;
		lString = StringUtility::Format(_T("%c:\\"), gDeviceID);
		lOpenParms.lpstrElementName = lString.c_str();
	}

	bool lOK = gDeviceID <= 'z';
	
	if (lOK == true)
	{
		mciSendCommand(lOpenParms.wDeviceID, MCI_CLOSE, NULL, NULL);
	}

	pDrive = (char)gDeviceID;
	return lOK;
}

bool CDPlayer::GetNextDrive(tchar& pDrive)
{
	if (gDeviceID <= 'z')
		gDeviceID++;

	bool lOK = gDeviceID <= 'z';

	if (lOK == true)
	{
		MCI_OPEN_PARMS lOpenParms;
		String lString(StringUtility::Format(_T("%c:\\"), gDeviceID));
		lOpenParms.wDeviceID = 0;
		lOpenParms.lpstrElementName = lString.c_str();
		lOpenParms.lpstrDeviceType = (LPCTSTR)MCI_DEVTYPE_CD_AUDIO;
		lOpenParms.dwCallback = 0;

		while (gDeviceID <= 'z' &&
			  mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_ELEMENT, (DWORD_PTR)&lOpenParms) != 0)
		{
			gDeviceID++;
			lString = StringUtility::Format(_T("%c:\\"), gDeviceID);
			lOpenParms.lpstrElementName = lString.c_str();
		}

		lOK = gDeviceID <= 'z';

		if (lOK == true)
		{
			mciSendCommand(lOpenParms.wDeviceID, MCI_CLOSE, NULL, NULL);
		}
	}

	pDrive = (char)gDeviceID;
	return lOK;
}

Win32CDPlayer::Win32CDPlayer(tchar pDrive) :
	mDrive(pDrive),
	mOpened(0)
{
	mMCIOpen.wDeviceID = 0;
	mMCIOpen.lpstrElementName = 0;
	SetDrive(pDrive);
}

Win32CDPlayer::~Win32CDPlayer()
{
	delete[] mMCIOpen.lpstrElementName;
}

void Win32CDPlayer::SetDrive(tchar pDrive)
{
	mDrive = pDrive;

	if (pDrive != 0)
	{
		if (mMCIOpen.lpstrElementName != 0)
		{
			delete[] mMCIOpen.lpstrElementName;
			mMCIOpen.lpstrElementName = 0;
		}

		tchar* lString = new tchar[4];
		lString[0] = pDrive;
		lString[1] = _T(':');
		lString[2] = _T('\\');
		lString[3] = 0;
		mMCIOpen.lpstrElementName = lString;
	}
	else
	{
		mMCIOpen.lpstrElementName = 0;
	}
}

bool Win32CDPlayer::Open()
{
	if (mOpened == false)
	{
		mOpened = OpenCD(_T("Open()"));
	}

	return mOpened;
}

void Win32CDPlayer::Close()
{
	if (mOpened == true)
	{
		CloseCD();
		mOpened = false;
	}
}

bool Win32CDPlayer::IsOpened()
{
	return mOpened;
}

bool Win32CDPlayer::IsReady()
{
	mMCIStatus.dwItem = MCI_STATUS_READY; 
	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_STATUS,
					  MCI_STATUS_ITEM | MCI_WAIT,
					  (DWORD_PTR)&mMCIStatus))
	{
		mLog.AError("IsReady() - Couldn't get CD-status!");
		return false;
	}

	return (mMCIStatus.dwReturn == TRUE);
}

bool Win32CDPlayer::OpenCD(const String& pFunction)
{
	mMCIOpen.lpstrDeviceType = (LPCTSTR)MCI_DEVTYPE_CD_AUDIO;
	mMCIOpen.dwCallback = 0;

	if (mciSendCommand(NULL,
					  MCI_OPEN,
					  MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID |
						(mMCIOpen.lpstrElementName != 0 ? MCI_OPEN_ELEMENT : 0),
					  (DWORD_PTR)&mMCIOpen))
	{
		mLog.Error(_T("OpenCD() - Couldn't get access to the CD player in ") + pFunction + _T("."));
		return false;
	}

	return true;
}

void Win32CDPlayer::CloseCD()
{
	mciSendCommand(mMCIOpen.wDeviceID, MCI_CLOSE, NULL, NULL);
	mMCIOpen.wDeviceID = 0;
}

bool Win32CDPlayer::InitTimeFormat(const String& pFunction)
{
	// Set the time format to milliseconds.
	MCI_SET_PARMS lMCISet;
	lMCISet.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_SET,
					  MCI_SET_TIME_FORMAT | MCI_WAIT,
					  (DWORD_PTR)&lMCISet))
	{
		mLog.Error(_T("InitTimeFormat() - error setting the time format in ") + pFunction + _T("."));
		return false;
	}

	return true;
}

int Win32CDPlayer::GetNumTracks()
{
	// Read number of tracks.
	mMCIStatus.dwItem = MCI_STATUS_NUMBER_OF_TRACKS; 
	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_STATUS,
					  MCI_STATUS_ITEM | MCI_WAIT,
					  (DWORD_PTR)&mMCIStatus))
	{
		mLog.AError("GetNumTracks() - Couldn't get number of tracks!");
		return -1;
	}

	return (int)mMCIStatus.dwReturn;
}

int Win32CDPlayer::GetCurrentTrack()
{
	// Read current track.
	mMCIStatus.dwItem = MCI_STATUS_CURRENT_TRACK; 
	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_STATUS,
					  MCI_STATUS_ITEM | MCI_WAIT,
					  (DWORD_PTR)&mMCIStatus))
	{
		mLog.AError("GetCurrentTrack() - Couldn't read current track!");
		return -1;
	}

	return (int)mMCIStatus.dwReturn;
}

float64 Win32CDPlayer::GetCurrentPos()
{
	if (InitTimeFormat(_T("GetCurrentPos()")) == false)
	{
		return 0.0;
	}

	// Read current position.
	mMCIStatus.dwItem = MCI_STATUS_POSITION;
	mMCIStatus.dwCallback = 0;
	mMCIStatus.dwReturn = 0;
	mMCIStatus.dwTrack = 0;
	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_STATUS,
					  MCI_STATUS_ITEM | MCI_WAIT,
					  (DWORD_PTR)&mMCIStatus))
	{
		mLog.AError("GetCurrentPos() - Couldn't read current track!");
		return 0.0;
	}

	return (float64)mMCIStatus.dwReturn / 1000.0;
}

float64 Win32CDPlayer::GetTrackPos(int pTrack)
{
	if (InitTimeFormat(_T("GetTrackPos()")) == false)
	{
		return 0.0;
	}

	// Get the length of the track.
	mMCIStatus.dwItem = MCI_STATUS_POSITION;
	mMCIStatus.dwTrack = pTrack;

	mciSendCommand(mMCIOpen.wDeviceID,
					MCI_STATUS,
					MCI_TRACK | MCI_STATUS_ITEM | MCI_WAIT,
					(DWORD_PTR)&mMCIStatus);

	return (float64)mMCIStatus.dwReturn / 1000.0;
}

float64 Win32CDPlayer::GetTrackLength(int pTrack)
{
	if (InitTimeFormat(_T("GetTrackLength()")) == false)
	{
		return 0.0;
	}

	// Get the length of the track.
	mMCIStatus.dwItem = MCI_STATUS_LENGTH;
	mMCIStatus.dwTrack = pTrack;

	mciSendCommand(mMCIOpen.wDeviceID,
					MCI_STATUS,
					MCI_TRACK | MCI_STATUS_ITEM | MCI_WAIT,
					(DWORD_PTR)&mMCIStatus);

	return (float64)mMCIStatus.dwReturn / 1000.0;
}

float64 Win32CDPlayer::GetMediaLength()
{
	if (InitTimeFormat(_T("GetMediaLength()")) == false)
	{
		return 0.0;
	}

	// Get the total length of the CD.
	mMCIStatus.dwItem = MCI_STATUS_LENGTH;
	mciSendCommand(mMCIOpen.wDeviceID,
					MCI_STATUS,
					MCI_STATUS_ITEM | MCI_WAIT,
					(DWORD_PTR)&mMCIStatus);

	return (float64)mMCIStatus.dwReturn / 1000.0;
}

bool Win32CDPlayer::OpenTray()
{
	if (OpenCD(_T("OpenTray()")) == false)
	{
		return false;
	}

	MCI_SET_PARMS lMCISet;
	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_SET,
					  MCI_SET_DOOR_OPEN | MCI_WAIT,
					  (DWORD_PTR)&lMCISet))
	{
		mLog.AError("OpenTray() - error opening the CD tray.");
		CloseCD();
		return false;
	}

	CloseCD();
	return true;
}

bool Win32CDPlayer::CloseTray()
{
	if (OpenCD(_T("CloseTray()")) == false)
	{
		return false;
	}

	MCI_SET_PARMS lMCISet;
	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_SET,
					  MCI_SET_DOOR_CLOSED | MCI_WAIT,
					  (DWORD_PTR)&lMCISet))
	{
		mLog.AError("CloseTray() - error closing the CD tray.");
		CloseCD();
		return false;
	}

	CloseCD();
	return true;
}

bool Win32CDPlayer::Play(int pTrack, CDPlayMode pMode, SynchMode pSynchMode)
{
	// Set the time format to track/minute/second/frame (TMSF). 
	MCI_SET_PARMS lMCISet;
	lMCISet.dwCallback = 0;
	lMCISet.dwAudio = 0;
	lMCISet.dwTimeFormat = MCI_FORMAT_TMSF; 
	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_SET,
					  MCI_SET_TIME_FORMAT,
					  (DWORD_PTR)&lMCISet))
	{
		mLog.AError("Play() - error while setting time format.");
		return false;
	}

	// Get the length of the track.
	mMCIStatus.dwItem = MCI_STATUS_LENGTH;
	mMCIStatus.dwTrack = pTrack;
	mciSendCommand(mMCIOpen.wDeviceID,
					MCI_STATUS,
					MCI_TRACK | MCI_STATUS_ITEM | MCI_WAIT,
					(DWORD_PTR)&mMCIStatus);

	// Setup the "from" and "to" play interval.
	MCI_PLAY_PARMS lMCIPlay;
	lMCIPlay.dwFrom = MCI_MAKE_TMSF(pTrack, 0, 0, 0); 

	lMCIPlay.dwTo = MCI_MAKE_TMSF(pTrack, 
								   MCI_MSF_MINUTE(mMCIStatus.dwReturn),
								   MCI_MSF_SECOND(mMCIStatus.dwReturn),
								   MCI_MSF_FRAME(mMCIStatus.dwReturn));

	// Set MCI_WAIT flag, and this function won't return until the playback
	// has finished.
	DWORD lFlags = MCI_FROM;

	if (pMode == CD_PLAYTRACK)
	{
		lFlags |= MCI_TO;
	}

	if (pSynchMode == SYNCH)
	{
		lFlags |= MCI_WAIT;
	}

	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_PLAY,
					  lFlags,
					  (DWORD_PTR)&lMCIPlay))
	{
		mLog.AError("Play() - error playing track.");
		return false;
	}

	return true;
}


bool Win32CDPlayer::Stop()
{
	MCI_SET_PARMS lMCISet;
	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_STOP,
					  0,
					  (DWORD_PTR)&lMCISet))
	{
		mLog.AError("Stop() - error stopping the CD.");
		return false;
	}

	return true;
}

bool Win32CDPlayer::Pause()
{
	MCI_SET_PARMS lMCISet;
	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_PAUSE,
					  0,
					  (DWORD_PTR)&lMCISet))
	{
		mLog.AError("Pause() - error pausing the CD.");
		return false;
	}

	return true;
}

bool Win32CDPlayer::Resume()
{
	MCI_SET_PARMS lMCISet;
	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_RESUME,
					  0,
					  (DWORD_PTR)&lMCISet))
	{
		mLog.AError("Resume() - error resuming the CD.");
		return false;
	}

	return true;
}

bool Win32CDPlayer::IsPaused()
{
	mMCIStatus.dwItem = MCI_STATUS_MODE;
	mciSendCommand(mMCIOpen.wDeviceID,
					MCI_STATUS,
					MCI_STATUS_ITEM | MCI_WAIT,
					(DWORD_PTR)&mMCIStatus);
  
	return (mMCIStatus.dwReturn == MCI_MODE_PAUSE);
}

bool Win32CDPlayer::IsPlaying()
{
	mMCIStatus.dwItem = MCI_STATUS_MODE;
	mciSendCommand(mMCIOpen.wDeviceID,
					MCI_STATUS,
					MCI_STATUS_ITEM | MCI_WAIT,
					(DWORD_PTR)&mMCIStatus);
  
	return (mMCIStatus.dwReturn == MCI_MODE_PLAY);
}

bool Win32CDPlayer::SetVolume(float64)
{
	// TRICKY: Can't set the CD volume using MCI. 
	// TODO: Figure out if there's another way.
	// http://blogs.msdn.com/larryosterman/archive/2005/10/06/477874.aspx

	return false;
}

bool Win32CDPlayer::Seek(float64 pPosition)
{
	if (InitTimeFormat(_T("Seek()")) == false)
	{
		return 0.0;
	}

	// Get the total length of the CD.
	MCI_SEEK_PARMS lSeek;
	lSeek.dwCallback = 0;
	lSeek.dwTo = (int)(pPosition * 1000.0);
	if (mciSendCommand(mMCIOpen.wDeviceID,
					  MCI_SEEK,
					  MCI_TO | MCI_WAIT,
					  (DWORD_PTR)&lSeek))
	{
		mLog.AError("Seek() - error seeking the CD.");
		return false;
	}

	return true;
}

LogDecorator Win32CDPlayer::mLog(typeid(Win32CDPlayer));

} // End namespace.