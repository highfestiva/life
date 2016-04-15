
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/UiMusicPlayer.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/Random.h"
#include "../../UiLepra/Include/UiSoundStream.h"



namespace UiCure
{



MusicPlayer::MusicPlayer(UiLepra::SoundManager* pSoundManager):
	mSoundManager(pSoundManager),
	mMusicStream(0),
	mMode(MODE_IDLE),
	mCurrentSongIndex(0x7FFFFFFF),
	mVolume(1),
	mPreSongPauseTime(0),
	mPostSongPauseTime(0)
{
}

MusicPlayer::~MusicPlayer()
{
	KillSong();
	mSoundManager = 0;
}



void MusicPlayer::AddSong(const str& pName)
{
	mSongNameList.push_back(pName);
}

void MusicPlayer::Shuffle()
{
	SongNameList lShuffeledList;
	while (!mSongNameList.empty())
	{
		const size_t lIndex = Random::GetRandomNumber() % mSongNameList.size();
		const str& lName = mSongNameList[lIndex];
		lShuffeledList.push_back(lName);
		mSongNameList.erase(mSongNameList.begin()+lIndex);
	}
	mSongNameList = lShuffeledList;
}

void MusicPlayer::SetVolume(float pVolume)
{
	if (!Math::IsEpsEqual(mVolume, pVolume))
	{
		mVolume = pVolume;
		if (mMusicStream)
		{
			mMusicStream->SetVolume(mVolume);
		}
	}
}

void MusicPlayer::SetSongPauseTime(float pBefore, float pAfter)
{
	mPreSongPauseTime = pBefore;
	mPostSongPauseTime = pAfter;
}

bool MusicPlayer::Playback()
{
	if (mMode == MODE_IDLE)
	{
		SetMode(MODE_PRE_PLAYBACK);
	}
	return true;
}

bool MusicPlayer::Pause()
{
	if (mMusicStream)
	{
		return mMusicStream->Pause();
	}
	return false;
}

bool MusicPlayer::Stop()
{
	KillSong();
	SetMode(MODE_IDLE);
	return true;
}

bool MusicPlayer::Update()
{
	mModeSetTimer.UpdateTimer();

	switch (mMode)
	{
		case MODE_IDLE:
		break;
		case MODE_PRE_PLAYBACK:
		{
			if (mModeSetTimer.GetTimeDiff() >= mPreSongPauseTime)
			{
				StartPlayback();
				if (mMusicStream)
				{
					SetMode(MODE_PLAYBACK);
				}
				else
				{
					return false;
				}
			}
		}
		break;
		case MODE_PLAYBACK:
		{
			deb_assert(mMusicStream);
			if (!mMusicStream)
			{
				return false;
			}
			mMusicStream->Update();
			if (!mMusicStream->IsPlaying())
			{
				SetMode(MODE_POST_PLAYBACK);
			}
		}
		break;
		case MODE_POST_PLAYBACK:
		{
			KillSong();
			if (mModeSetTimer.GetTimeDiff() >= mPostSongPauseTime)
			{
				SetMode(MODE_PRE_PLAYBACK);
			}
		}
		break;
	}
	return true;
}



void MusicPlayer::SetMode(MODE pMode)
{
	mMode = pMode;
	mModeSetTimer.PopTimeDiff();
}

bool MusicPlayer::StartPlayback()
{
	if (mMusicStream)
	{
		delete mMusicStream;
		mMusicStream = 0;
	}

	if (mSongNameList.empty())
	{
		return false;
	}
	++mCurrentSongIndex;
	if (mCurrentSongIndex >= mSongNameList.size())
	{
		mCurrentSongIndex = 0;
	}
	mMusicStream = mSoundManager->CreateSoundStream(mSongNameList[mCurrentSongIndex], UiLepra::SoundManager::LOOP_NONE, 0);
	if (mMusicStream)
	{
		mMusicStream->SetVolume(mVolume);
		return mMusicStream->Playback();
	}
	else
	{
		mLog.Errorf("Unable to load song %s!", mSongNameList[mCurrentSongIndex].c_str());
	}
	return false;
}

void MusicPlayer::KillSong()
{
	delete mMusicStream;
	mMusicStream = 0;
}


loginstance(GAME, MusicPlayer);



}
