/*
	Class:  PosixCDPlayer
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../../Include/Posix/PosixCDPlayer.h"
#include "../../Include/Math.h"
#include <fcntl.h>
#include <unistd.h>

namespace Lepra
{

CDPlayer* CDPlayer::CreateCDPlayer(tchar pDrive)
{
	return new PosixCDPlayer(pDrive);
}

bool CDPlayer::GetFirstDrive(tchar& pDrive)
{
	// TODO: Do something else instead?
	pDrive = 0;
}

bool CDPlayer::GetNextDrive(tchar& pDrive)
{
	// TODO: Do something else instead?
	pDrive = 0;
}

PosixCDPlayer::PosixCDPlayer(tchar pDrive) :
	mFD(0),
	mTrackInfo(0),
	mDrive(pDrive),
	mOpened(false),
	mPlaying(false),
	mPaused(false),
	mDevice(_T("/dev/cdrom"))
{
	SetDrive(mDrive);
}

PosixCDPlayer::~PosixCDPlayer()
{
	Close();
}

void PosixCDPlayer::SetDrive(tchar pDrive)
{	
	mDrive = pDrive;

	switch (mDrive)
	{
		case 0:
		default:
		{
			mDevice = _T("/dev/cdrom");
		}
		break;
	}
}

bool PosixCDPlayer::Open()
{
	if (mOpened == false)
	{
		mFD = open((const char*)mDevice.ToAnsi(), O_RDONLY);
		mOpened = (mFD >= 0);
		
		if (mOpened == true)
		{
			if (ioctl(mFD, CDROMREADTOCHDR, &mCDHeader) == 0)
			{
				mOpened = ReadTrackInfo();
			}
			else
			{
				Close();				
			}
		}
	}

	return mOpened;	
}

void PosixCDPlayer::Close()
{
	if (mOpened == true)
	{
		close(mFD);
		
		delete[] mTrackInfo;
		mTrackInfo = 0;
		
		mOpened = false;
		mPlaying = false;
		mPaused = false;
	}
}

bool PosixCDPlayer::ReadTrackInfo()
{
	assert(mOpened);
	
	int lNumTracks = GetNumTracks();
	int i;
	
	if (mTrackInfo != 0)
	{
		delete[] mTrackInfo;
	}

	// One extra entry for the leadout...
	mTrackInfo = new cdrom[lNumTracks + 1];
	
	for (i = 0; i < lNumTracks; i++)
	{
		mTrackInfo[i].cdte_format = CDROM_MSF;
		mTrackInfo[i].cdte_track = i + 1;
	
		if (ioctl(mFD, CDROMREADTOCENTRY, &mTrackInfo[i]) != 0)
		{
			// Error.
			return false;
		}
	}

	mTrackInfo[lNumTracks].cdte_format = CDROM_MSF;
	mTrackInfo[lNumTracks].cdte_track = CDROM_LEADOUT;
	
	return (ioctl(mFD, CDROMREADTOCENTRY, &mTrackInfo[lNumTracks]) == 0);
}

bool PosixCDPlayer::IsOpened()
{
	return mOpened;
}

bool PosixCDPlayer::IsReady()
{
	// TODO: Verify that this works...
	bool lReady = false;
	if (mOpened == false)
	{
		lReady = Open();
		Close();
	}
	else
	{
		// Try to read header again.
		lReady = (ioctl(mFD, CDROMREADTOCHDR, &mCDHeader) == 0);
	}
	return lReady;
}

int PosixCDPlayer::GetNumTracks()
{
	return (mCDHeader.cdth_trk1 - mCDHeader.cdth_trk0);
}

float64 PosixCDPlayer::GetMediaLength()
{
	float64 lMediaLength = 0;
	
	if (mOpened == true) 
	{
		lMediaLength = MSFToSeconds(mTrackInfo[GetNumTracks()].cdte_addr) - 
				MSFToSeconds(mTrackInfo[0].cdte_addr);
	}
	
	return lMediaLength;
}

float64 PosixCDPlayer::GetCurrentPos()
{
	
}

int PosixCDPlayer::GetCurrentTrack()
{
	
}

float64 PosixCDPlayer::GetTrackLength(int pTrack)
{
	assert(pTrack >= 0 && pTrack < GetNumTracks());
	
	float64 lTrackLength = 0;
	
	if (mOpened == true) 
	{
		lTrackLength = MSFToSeconds(mTrackInfo[pTrack + 1].cdte_addr) - 
				MSFToSeconds(mTrackInfo[pTrack].cdte_addr);
	}
	
	return lTrackLength;
}

float64 PosixCDPlayer::GetTrackPos(int pTrack)
{
	
}

bool PosixCDPlayer::Play(int pTrack, CDPlayMode pMode, SynchMode pSynchMode)
{
	assert(pTrack >= 0 && pTrack < GetNumTracks());

	cdrom lMSF;
	lMSF.cdmsf_min0   = mTrackInfo[pTrack].cdte_addr.msf.minute;
	lMSF.cdmsf_sec0   = mTrackInfo[pTrack].cdte_addr.msf.second;
	lMSF.cdmsf_frame0 = mTrackInfo[pTrack].cdte_addr.msf.frame;
	
	int lLast = pTrack + 1;
	if (pMode == CD_PLAY)
	{
		lLast = GetNumTracks();
	}
	
	lMSF.cdmsf_min1   = mTrackInfo[lLast].cdte_addr.msf.minute;
	lMSF.cdmsf_sec1   = mTrackInfo[lLast].cdte_addr.msf.second;
	lMSF.cdmsf_frame1 = mTrackInfo[lLast].cdte_addr.msf.minute;

	return (ioctl(mFD, CDROMPLAYMSF, &lMSF) == 0);
}

bool PosixCDPlayer::Stop()
{
	bool lOk = mOpened;
	
	if (lOk)
	{
		lOk = (ioctl(mFD, CDROMSTOP, 0) == 0);
	}
	
	if (lOk)
	{
		mPlaying = false;
		mPaused = false;
	}
	
	return lOk;
}

bool PosixCDPlayer::Pause()
{
	bool lOk = mOpened && mPlaying;
	
	if (lOk)
	{
		lOk = (ioctl(mFD, CDROMPAUSE, 0) == 0);
	}

	if (lOk)
	{
		mPaused = true;
	}
	
	return lOk;
}

bool PosixCDPlayer::Resume()
{
	bool lOk = mOpened;
	
	if (lOk)
	{
		lOk = (ioctl(mFD, CDROMRESUME, 0) == 0);
	}

	if (lOk)
	{
		mPlaying = true;
		mPaused = false;
	}
	
	return lOk;
}

bool PosixCDPlayer::IsPaused()
{
	return mPaused;
}

bool PosixCDPlayer::IsPlaying()
{
	return mPlaying;
}

bool PosixCDPlayer::SetVolume(float64 pVolume)
{
	bool lOk = mOpened;
	
	if (lOk)
	{
		pVolume = Math::Clamp(pVolume, 0.0, 1.0);
		uint8 lVolume = (uint8)(pVolume * 255.0);

		cdrom lVolCtrl;
		lVolCtrl.channel0 = lVolume;
		lVolCtrl.channel1 = lVolume;
		lVolCtrl.channel2 = lVolume;
		lVolCtrl.channel3 = lVolume;
		
		lOk = (ioctl(mFD, CDROMVOLCTRL, &lVolCtrl) == 0);
	}

	return lOk;
}

bool PosixCDPlayer::Seek(float64 pPosition)
{
	bool lOk = mOpened;
	
	if (lOk)
	{
		cdrom lMSF = SecondsToMSF(pPosition);
		lOk = (ioctl(mFD, CDROMSEEK, &lMSF) == 0);	
	}
	
	return lOk;
}

bool PosixCDPlayer::OpenTray()
{
	bool lOk = mOpened;
	
	if (lOk)
	{
		lOk = (ioctl(mFD, CDROMEJECT, 0) == 0);
	}
	
	return lOk;
}

bool PosixCDPlayer::CloseTray()
{
	bool lOk = mOpened;
	
	if (lOk)
	{
		lOk = (ioctl(mFD, CDROMCLOSETRAY, 0) == 0);
	}
	
	return lOk;
}

float64 PosixCDPlayer::MSFToSeconds(cdrom& pAddr)
{
	return	(float64)(pAddr.msf.minute * CD_SECS) +
		(float64)pAddr.msf.second +
		(float64)pAddr.msf.frame / (float64)CD_FRAMES;
}

cdrom PosixCDPlayer::SecondsToMSF(float64 pSeconds)
{
	cdrom lMSF;

	int lSeconds = (int)floor(pSeconds);
	
	lMSF.cdmsf_min0 = (uint8)(lSeconds / CD_SECS);
	lMSF.cdmsf_min1 = lMSF.cdmsf_min0;
	
	lMSF.cdmsf_sec0 = (uint8)(lSeconds % CD_SECS);
	lMSF.cdmsf_sec1 = lMSF.cdmsf_sec0;
	
	int lFrame = (int)floor((pSeconds - (float64)lSeconds) * (float64)CD_FRAMES);
	lMSF.cdmsf_frame0 = (uint8)lFrame;
	lMSF.cdmsf_frame1 = lMSF.cdmsf_frame0;
	
	return lMSF;
}

} // End namespace.
