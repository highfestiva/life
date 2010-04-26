
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiSoundManagerFMod.h"

#if !defined(LEPRA_WITHOUT_FMOD)



// TODO: refactor and remove unthinkable crap. Move resource ownership into resource manager where it belongs.



namespace UiLepra
{



SoundManagerFMod::SoundManagerFMod(int pMixRate):
	mSampleIDManager(1, 99999, 0),
	mStreamIDManager(100000, 199999, 0),
	mSoundInstanceIDManager(200000, 299999, 0),
	mChannel(0)
{
	if (FSOUND_Init(pMixRate, 32, 0) != 0)
	{
		mNumChannels = FSOUND_GetMaxChannels();
		mChannel = new Channel[mNumChannels];
		const float lPos[3] = {0, 0, 0};
		const float lVel[3] = {0, 0, 0};
		FSOUND_3D_Listener_SetAttributes(lPos, lVel, 0, 1, 0, 0, 0, 1);	// Forward along Y, up along Z.
	}
	else
	{
		mLog.AError("Failed to initialize FMOD sound system!");
	}
}

SoundManagerFMod::~SoundManagerFMod()
{
	FSOUND_Close();
	delete[] (mChannel);
}

SoundManager::SoundID SoundManagerFMod::LoadSound2D(const str& pFileName, LoopMode pLoopMode, int pPriority)
{
	FileNameToSampleTable::Iterator lIter = mFileNameToSampleTable.Find(pFileName);
	if (lIter != mFileNameToSampleTable.End())
	{
		(*lIter)->mReferenceCount++;
		return (*lIter)->mID;
	}
	else
	{
		unsigned lFlags = 0;
		
		switch(pLoopMode)
		{
		case LOOP_FORWARD:
			lFlags = FSOUND_LOOP_NORMAL;
			break;
		case LOOP_PINGPONG:
			lFlags = FSOUND_LOOP_BIDI;
			break;
		case LOOP_NONE:
		default:
			lFlags = FSOUND_LOOP_OFF;
			break;
		}

		lFlags |= FSOUND_2D;

		Sample* lSample = new Sample();
		lSample->mFileName = pFileName;
		lSample->mID = mSampleIDManager.GetFreeId();
		lSample->mSample = FSOUND_Sample_Load((int)lSample->mID,
			astrutil::Encode(pFileName).c_str(), lFlags, 0, 0);

		if (lSample->mSample == 0)
		{
			mSampleIDManager.RecycleId(lSample->mID);
			delete lSample;
			return mSampleIDManager.GetInvalidId();
		}

		FSOUND_Sample_SetDefaults(lSample->mSample, -1, -1, -1, pPriority);

		mFileNameToSampleTable.Insert(pFileName, lSample);
		mIDToSampleTable.Insert(lSample->mID, lSample);

		return lSample->mID;
	}
}

SoundManager::SoundID SoundManagerFMod::LoadSound3D(const str& pFileName, LoopMode pLoopMode, int pPriority)
{
	FileNameToSampleTable::Iterator lIter = mFileNameToSampleTable.Find(pFileName);
	if (lIter != mFileNameToSampleTable.End())
	{
		(*lIter)->mReferenceCount++;
		return (*lIter)->mID;
	}
	else
	{
		unsigned lFlags = 0;
		
		switch(pLoopMode)
		{
		case LOOP_FORWARD:
			lFlags = FSOUND_LOOP_NORMAL;
			break;
		case LOOP_PINGPONG:
			lFlags = FSOUND_LOOP_BIDI;
			break;
		case LOOP_NONE:
		default:
			lFlags = FSOUND_LOOP_OFF;
			break;
		}

		lFlags |= FSOUND_HW3D;

		Sample* lSample = new Sample();
		lSample->mFileName = pFileName;
		lSample->mID = mSampleIDManager.GetFreeId();
		lSample->mSample = FSOUND_Sample_Load((int)lSample->mID,
			astrutil::Encode(pFileName).c_str(), lFlags, 0, 0);

		if (lSample->mSample == 0)
		{
			mSampleIDManager.RecycleId(lSample->mID);
			delete lSample;
			return mSampleIDManager.GetInvalidId();
		}

		FSOUND_Sample_SetDefaults(lSample->mSample, -1, -1, -1, pPriority);
		
		mFileNameToSampleTable.Insert(pFileName, lSample);
		mIDToSampleTable.Insert(lSample->mID, lSample);

		return lSample->mID;
	}
}

SoundManager::SoundID SoundManagerFMod::LoadStream(const str& pFileName, LoopMode pLoopMode, int pPriority)
{
	FileNameToSampleTable::Iterator lIter = mFileNameToSampleTable.Find(pFileName);
	if (lIter != mFileNameToSampleTable.End())
	{
		(*lIter)->mReferenceCount++;
		return (*lIter)->mID;
	}
	else
	{
		unsigned lFlags = 0;
		
		switch(pLoopMode)
		{
		case LOOP_FORWARD:
			lFlags = FSOUND_LOOP_NORMAL;
			break;
		case LOOP_PINGPONG:
			lFlags = FSOUND_LOOP_BIDI;
			break;
		case LOOP_NONE:
		default:
			lFlags = FSOUND_LOOP_OFF;
			break;
		}

		lFlags |= FSOUND_2D;

		Sample* lSample = new Sample();
		lSample->mFileName = pFileName;
		lSample->mID = mStreamIDManager.GetFreeId();
		lSample->mStream = FSOUND_Stream_Open(astrutil::Encode(pFileName).c_str(),
			lFlags, 0, 0);

		if (lSample->mStream == 0)
		{
			mStreamIDManager.RecycleId(lSample->mID);
			delete lSample;
			return mStreamIDManager.GetInvalidId();
		}

		FSOUND_Sample_SetDefaults(FSOUND_Stream_GetSample(lSample->mStream), -1, -1, -1, pPriority);

		mFileNameToSampleTable.Insert(pFileName, lSample);
		mIDToSampleTable.Insert(lSample->mID, lSample);

		return lSample->mID;
	}
}

void SoundManagerFMod::Release(SoundID pSoundID)
{
	IDToSampleTable::Iterator lIter = mIDToSampleTable.Find((int)pSoundID);
	if (lIter != mIDToSampleTable.End())
	{
		Sample* lSample = *lIter;
		lSample->mReferenceCount--;

		if (lSample->mReferenceCount <= 0)
		{
			mIDToSampleTable.Remove(lIter);

			mFileNameToSampleTable.Remove(lSample->mFileName);

			if (lSample->mStream != 0)
			{
				FSOUND_Stream_Stop(lSample->mStream);
				FSOUND_Stream_Close(lSample->mStream);
				mStreamIDManager.RecycleId(lSample->mID);
			}

			if (lSample->mSample != 0)
			{
				FSOUND_Sample_Free(lSample->mSample);
				mSampleIDManager.RecycleId(lSample->mID);
			}

			delete lSample;
		}
	}
}

double SoundManagerFMod::GetStreamTime(SoundID pSoundID)
{
	IDToSampleTable::Iterator lIter = mIDToSampleTable.Find((int)pSoundID);
	if (lIter != mIDToSampleTable.End())
	{
		Sample* lSample = *lIter;
		if (lSample->mStream != 0)
		{
			return (double)FSOUND_Stream_GetTime(lSample->mStream) / 1000.0;
		}
	}

	return -1.0;
}

SoundManager::SoundInstanceID SoundManagerFMod::CreateSoundInstance(SoundID pSoundID)
{
	IDToSampleTable::Iterator lIter = mIDToSampleTable.Find((int)pSoundID);

	if (lIter == mIDToSampleTable.End())
	{
		return (SoundInstanceID)mSoundInstanceIDManager.GetInvalidId();
	}

	Sample* lSample = *lIter;

	// Start the sound in paused mode.
	int lChannel = -1;
	if (lSample->mSample != 0)
	{
		lChannel = FSOUND_PlaySoundEx(FSOUND_FREE, lSample->mSample, 0, 1);
	}
	else if(lSample->mStream != 0)
	{
		lChannel = FSOUND_Stream_PlayEx(FSOUND_FREE, lSample->mStream, 0, 1);
	}

	SoundInstanceID lSoundIID = (SoundInstanceID)mSoundInstanceIDManager.GetInvalidId();
	if (lChannel >= 0)
	{
		lChannel &= 0x00000FFF;

		// Remove the currently playing sound instance. The ID is still allocated though.
		DeleteSoundInstance(mChannel[lChannel].mSoundInstance.mSoundIID);

		// Allocate a new ID and update the channel.
		lSoundIID = (SoundInstanceID)mSoundInstanceIDManager.GetFreeId();

		mChannel[lChannel].mSoundInstance.mSoundID = pSoundID;
		mChannel[lChannel].mSoundInstance.mSoundIID = lSoundIID;
		mChannel[lChannel].mSoundInstance.mChannel = lChannel;

		mSoundInstanceTable.Insert((int)lSoundIID, mChannel[lChannel].mSoundInstance);
	}

	return lSoundIID;
}

void SoundManagerFMod::DeleteSoundInstance(SoundInstanceID pSoundIID)
{
	SoundInstanceTable::Iterator lIter;
	lIter = mSoundInstanceTable.Find((int)pSoundIID);
	if (lIter == mSoundInstanceTable.End())
	{
		return;
	}

	SoundInstance lSI = *lIter;
	mSoundInstanceTable.Remove(lIter);

	if (mChannel[lSI.mChannel].mSoundInstance.mSoundIID == pSoundIID)
	{
		mChannel[lSI.mChannel].mSoundInstance.mSoundIID = (SoundInstanceID)mSoundInstanceIDManager.GetInvalidId();
	}

	mSoundInstanceIDManager.RecycleId(pSoundIID);
}

bool SoundManagerFMod::Play(SoundInstanceID pSoundIID, float pVolume, float pPitch)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return (false);
	}

	SoundInstance lSI = *lSIIter;

	FSOUND_SetPan(lSI.mChannel, FSOUND_STEREOPAN);
	FSOUND_SetVolume(lSI.mChannel, (int)(pVolume * 255.0f));

	if (pPitch != 1.0f)
	{
		// TODO: Fix the pitch to be relative to the sample frequency.
		FSOUND_SetFrequency(lSI.mChannel, (int)(44100.0f * pPitch) );
	}

	FSOUND_SetPaused(lSI.mChannel, 0);
	return (true);
}

void SoundManagerFMod::StopAll()
{
	for (int i = 0; i < FSOUND_GetMaxChannels(); i++)
	{
		FSOUND_StopSound(i);
	}
}

void SoundManagerFMod::Stop(SoundInstanceID pSoundIID)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return;
	}

	FSOUND_StopSound((*lSIIter).mChannel);
}

void SoundManagerFMod::TogglePause(SoundInstanceID pSoundIID)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return;
	}

	signed char lSetPause = !FSOUND_GetPaused((*lSIIter).mChannel);
	FSOUND_SetPaused((*lSIIter).mChannel, lSetPause);
}

bool SoundManagerFMod::IsPlaying(SoundInstanceID pSoundIID)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return false;
	}

	return (FSOUND_IsPlaying((*lSIIter).mChannel) != 0);
}

bool SoundManagerFMod::IsPaused(SoundInstanceID pSoundIID)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return false;
	}

	return (FSOUND_GetPaused((*lSIIter).mChannel) != 0);
}

void SoundManagerFMod::SetPan(SoundInstanceID pSoundIID, float pPan)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return;
	}

	FSOUND_SetPan((*lSIIter).mChannel, (int)((pPan + 1.0f) * 127.5f));
}

void SoundManagerFMod::SetVolume(SoundInstanceID pSoundIID, float pVolume)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return;
	}

	FSOUND_SetVolume((*lSIIter).mChannel, (int)(pVolume * 255.0f));
}

void SoundManagerFMod::SetPitch(SoundInstanceID pSoundIID, float pPitch)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return;
	}

	// TODO: Fix the pitch to be relative to the sample frequency.
	FSOUND_SetFrequency((*lSIIter).mChannel, (int)(44100.0f * pPitch) );
}

void SoundManagerFMod::SetFrequency(SoundInstanceID pSoundIID, int pFrequency)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return;
	}

	FSOUND_SetFrequency((*lSIIter).mChannel, pFrequency);
}

int SoundManagerFMod::GetFrequency(SoundInstanceID pSoundIID)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return 0;
	}

	return FSOUND_GetFrequency((*lSIIter).mChannel);
}

void SoundManagerFMod::SetDopplerFactor(float pFactor)
{
	FSOUND_3D_SetDopplerFactor(pFactor);
}

void SoundManagerFMod::SetRollOffFactor(float pFactor)
{
	FSOUND_3D_SetRolloffFactor(pFactor);
}

void SoundManagerFMod::SetChorus(SoundInstanceID pSoundIID, int pFXIndex, float /*pDelay*/, float pFeedback,
	float pRate, float pDepth, float pWetness)
{
	int lFXID = mFXHandles.Get(GetChannel(pSoundIID), pFXIndex);
	FSOUND_FX_SetChorus(lFXID, pWetness, pDepth, pFeedback, pRate, 1, 0, 2);
}

void SoundManagerFMod::SetFlanger(SoundInstanceID pSoundIID, int pFXIndex, float /*pDelay*/, float pFeedback,
	float pRate, float pDepth, float pWetness)
{
	int lFXID = mFXHandles.Get(GetChannel(pSoundIID), pFXIndex);
	FSOUND_FX_SetFlanger(lFXID, pWetness, pDepth, pFeedback, pRate, 1, 0, 2);
}

void SoundManagerFMod::SetCompressor(SoundInstanceID pSoundIID, int pFXIndex, float pRatio, float pThreshold,
	float pAttack, float pRelease)
{
	int lFXID = mFXHandles.Get(GetChannel(pSoundIID), pFXIndex);
	FSOUND_FX_SetCompressor(lFXID, 0, pAttack, pRelease, pThreshold, pRatio, 0);
}

void SoundManagerFMod::SetEcho(SoundInstanceID pSoundIID, int pFXIndex, float pFeedback, float pDelay, float pWetness)
{
	int lFXID = mFXHandles.Get(GetChannel(pSoundIID), pFXIndex);
	FSOUND_FX_SetEcho(lFXID, pWetness, pFeedback, pDelay, pDelay, 0);
}

void SoundManagerFMod::SetParamEQ(SoundInstanceID pSoundIID, int pFXIndex, float pCenter, float pBandwidth, float pGain)
{
	int lFXID = mFXHandles.Get(GetChannel(pSoundIID), pFXIndex);
	FSOUND_FX_SetParamEQ(lFXID, pCenter, pBandwidth, pGain);
}



int SoundManagerFMod::GetChannel(SoundInstanceID pSoundIID)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		assert(false);
		return -1;
	}

	return (*lSIIter).mChannel;
}

void SoundManagerFMod::DoSetSoundPosition(SoundInstanceID pSoundIID, const Vector3DF& pPos, const Vector3DF& pVel)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		assert(false);
		return;
	}

	float lPos[3];
	float lVel[3];

	lPos[0] = pPos.x;
	lPos[1] = pPos.y;
	lPos[2] = pPos.z;

	lVel[0] = pVel.x;
	lVel[1] = pVel.y;
	lVel[2] = pVel.z;

	FSOUND_3D_SetAttributes((*lSIIter).mChannel, lPos, lVel);
}



LOG_CLASS_DEFINE(UI_SOUND, SoundManagerFMod);



}



#endif // !WITHOUT_FMOD
