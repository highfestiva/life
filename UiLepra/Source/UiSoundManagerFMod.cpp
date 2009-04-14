
// Author: Alexander Hugestrand
//	Copyright (c) 2002-2008, Righteous Games



#include "../Include/UiOpenGLExtensions.h"
#include "../../Lepra/Include/Log.h"
#include "../Include/UiLepra.h"
#include "../Include/UiSoundManagerFMod.h"



namespace UiLepra
{



SoundManagerFMod::SoundManagerFMod(int pMixRate):
	mInitialized(false),
	mSampleIDManager(1, 99999, 0),
	mStreamIDManager(100000, 199999, 0),
	mSoundInstanceIDManager(200000, 299999, 0)
{
	if (FSOUND_Init(pMixRate, 32, 0) != 0)
	{
		mNumChannels = FSOUND_GetMaxChannels();
		mChannel = new Channel[mNumChannels];

		mInitialized = true;
	}
	else
	{
		mLog.AError("Failed to initialize FMOD sound system!");
	}
}

SoundManagerFMod::~SoundManagerFMod()
{
	FSOUND_Close();
	if (mInitialized == true)
	{
		delete[] mChannel;
	}
}

void SoundManagerFMod::Update()
{
}

SoundManager::SoundID SoundManagerFMod::LoadSound2D(const Lepra::String& pFileName, LoopMode pLoopMode, int pPriority)
{
	Clamp(pPriority, 0, 255);

	FileNameToSampleTable::Iterator lIter = mFileNameToSampleTable.Find(pFileName);
	if (lIter != mFileNameToSampleTable.End())
	{
		(*lIter)->mReferenceCount++;
		return (SoundID)(*lIter)->mID;
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
		lSample->mSample = FSOUND_Sample_Load(lSample->mID,
			Lepra::AnsiStringUtility::ToOwnCode(pFileName).c_str(), lFlags, 0, 0);

		if (lSample->mSample == 0)
		{
			mSampleIDManager.RecycleId(lSample->mID);
			delete lSample;
			return (SoundID)mSampleIDManager.GetInvalidId();
		}

		FSOUND_Sample_SetDefaults(lSample->mSample, -1, -1, -1, pPriority);

		mFileNameToSampleTable.Insert(pFileName, lSample);
		mIDToSampleTable.Insert(lSample->mID, lSample);

		return (SoundID)lSample->mID;
	}
}

SoundManager::SoundID SoundManagerFMod::LoadSound3D(const Lepra::String& pFileName, LoopMode pLoopMode, int pPriority)
{
	Clamp(pPriority, 0, 255);

	FileNameToSampleTable::Iterator lIter = mFileNameToSampleTable.Find(pFileName);
	if (lIter != mFileNameToSampleTable.End())
	{
		(*lIter)->mReferenceCount++;
		return (SoundID)(*lIter)->mID;
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
		lSample->mSample = FSOUND_Sample_Load(lSample->mID,
			Lepra::AnsiStringUtility::ToOwnCode(pFileName).c_str(), lFlags, 0, 0);

		if (lSample->mSample == 0)
		{
			mSampleIDManager.RecycleId(lSample->mID);
			delete lSample;
			return (SoundID)mSampleIDManager.GetInvalidId();
		}

		FSOUND_Sample_SetDefaults(lSample->mSample, -1, -1, -1, pPriority);
		
		mFileNameToSampleTable.Insert(pFileName, lSample);
		mIDToSampleTable.Insert(lSample->mID, lSample);

		return (SoundID)lSample->mID;
	}
}

SoundManager::SoundID SoundManagerFMod::LoadStream(const Lepra::String& pFileName, LoopMode pLoopMode, int pPriority)
{
	Clamp(pPriority, 0, 255);

	FileNameToSampleTable::Iterator lIter = mFileNameToSampleTable.Find(pFileName);
	if (lIter != mFileNameToSampleTable.End())
	{
		(*lIter)->mReferenceCount++;
		return (SoundID)(*lIter)->mID;
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
		lSample->mStream = FSOUND_Stream_Open(Lepra::AnsiStringUtility::ToOwnCode(pFileName).c_str(),
			lFlags, 0, 0);

		if (lSample->mStream == 0)
		{
			mStreamIDManager.RecycleId(lSample->mID);
			delete lSample;
			return (SoundID)mStreamIDManager.GetInvalidId();
		}

		FSOUND_Sample_SetDefaults(FSOUND_Stream_GetSample(lSample->mStream), -1, -1, -1, pPriority);

		mFileNameToSampleTable.Insert(pFileName, lSample);
		mIDToSampleTable.Insert(lSample->mID, lSample);

		return (SoundID)lSample->mID;
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

double SoundManagerFMod::GetStreamTime(SoundID pStreamID)
{
	IDToSampleTable::Iterator lIter = mIDToSampleTable.Find((int)pStreamID);
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
	return CreateSoundInstance(pSoundID, false);
}

SoundManager::SoundInstanceID SoundManagerFMod::CreateSoundInstance(SoundID pSoundID, bool pAutoDelete)
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
		DiscardSoundInstance(mChannel[lChannel].mSoundInstance.mSoundIID);

		// Allocate a new ID and update the channel.
		lSoundIID = (SoundInstanceID)mSoundInstanceIDManager.GetFreeId();

		mChannel[lChannel].mSoundInstance.mSoundID = pSoundID;
		mChannel[lChannel].mSoundInstance.mSoundIID = lSoundIID;
		mChannel[lChannel].mSoundInstance.mChannel = lChannel;
		mChannel[lChannel].mSoundInstance.mAutoDelete = pAutoDelete;

		mSoundInstanceTable.Insert((int)lSoundIID, mChannel[lChannel].mSoundInstance);
	}

	return lSoundIID;
}

void SoundManagerFMod::DeleteSoundInstance(SoundInstanceID pSoundIID)
{
	if (pSoundIID == mSoundInstanceIDManager.GetInvalidId())
	{
		return;
	}

	DiscardSoundInstance(pSoundIID);
	mSoundInstanceIDManager.RecycleId(pSoundIID);
}

bool SoundManagerFMod::Play(SoundID pSoundID, float pVolume, float pPitch)
{
	return Play(CreateSoundInstance(pSoundID, true), pVolume, pPitch);
}

bool SoundManagerFMod::Play(SoundInstanceID pSoundIID, float pVolume, float pPitch)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return (false);
	}

	SoundInstance lSI = *lSIIter;

	Clamp(pVolume, 0.0f, 1.0f);

	FSOUND_SetPan(lSI.mChannel, FSOUND_STEREOPAN);
	FSOUND_SetVolume(lSI.mChannel, (int)(pVolume * 255.0f));

	if (pPitch != 1.0f)
	{
		//Clamp(pFrequency, 100, 705600);
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

void SoundManagerFMod::Pause(SoundInstanceID pSoundIID)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return;
	}

	FSOUND_SetPaused((*lSIIter).mChannel, 1);
}

void SoundManagerFMod::Unpause(SoundInstanceID pSoundIID)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return;
	}

	FSOUND_SetPaused((*lSIIter).mChannel, 0);
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

	Clamp(pPan, -1.0f, 1.0f);
	FSOUND_SetPan((*lSIIter).mChannel, (int)((pPan + 1.0f) * 127.5f));
}

float SoundManagerFMod::GetPan(SoundInstanceID pSoundIID)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return 0;
	}

	return ((float)FSOUND_GetPan((*lSIIter).mChannel) / 127.5f) - 1.0f;
}

void SoundManagerFMod::SetVolume(SoundInstanceID pSoundIID, float pVolume)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return;
	}

	Clamp(pVolume, 0.0f, 1.0f);
	FSOUND_SetVolume((*lSIIter).mChannel, (int)(pVolume * 255.0f));
}

float SoundManagerFMod::GetVolume(SoundInstanceID pSoundIID)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return 0;
	}

	return (float)FSOUND_GetVolume((*lSIIter).mChannel) / 255.0f;
}

void SoundManagerFMod::SetFrequency(SoundInstanceID pSoundIID, int pFrequency)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return;
	}

	Clamp(pFrequency, 100, 705600);
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

void SoundManagerFMod::Set3DSoundAttributes(SoundInstanceID pSoundIID, const Lepra::Vector3DF& pPos, const Lepra::Vector3DF& pVel)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
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

void SoundManagerFMod::Get3DSoundAttributes(SoundInstanceID pSoundIID, Lepra::Vector3DF& pPos, Lepra::Vector3DF& pVel)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return;
	}

	float lPos[3];
	float lVel[3];

	FSOUND_3D_GetAttributes((*lSIIter).mChannel, lPos, lVel);

	pPos.x = lPos[0];
	pPos.y = lPos[1];
	pPos.z = lPos[2];

	pVel.x = lVel[0];
	pVel.y = lVel[1];
	pVel.z = lVel[2];
}

void SoundManagerFMod::Set3dCurrentListener(int pListenerIndex, int pListenerCount)
{
	FSOUND_3D_Listener_SetCurrent(pListenerIndex, pListenerCount);
}

void SoundManagerFMod::Set3DListenerAttributes(const Lepra::Vector3DF& pPos, const Lepra::Vector3DF& pVel,
	const Lepra::Vector3DF& pUp, const Lepra::Vector3DF& pForward)
{
	float lPos[3];
	float lVel[3];

	lPos[0] = pPos.x;
	lPos[1] = pPos.y;
	lPos[2] = pPos.z;

	lVel[0] = pVel.x;
	lVel[1] = pVel.y;
	lVel[2] = pVel.z;

	FSOUND_3D_Listener_SetAttributes(lPos, lVel, pForward.x, pForward.y, pForward.z, pUp.x, pUp.y, pUp.z);
}

void SoundManagerFMod::Get3DListenerAttributes(Lepra::Vector3DF& pPos, Lepra::Vector3DF& pVel,
	Lepra::Vector3DF& pUp, Lepra::Vector3DF& pForward)
{
	float lPos[3];
	float lVel[3];

	FSOUND_3D_Listener_GetAttributes(lPos, lVel, &pForward.x, &pForward.y, &pForward.z, &pUp.x, &pUp.y, &pUp.z);

	pPos.x = lPos[0];
	pPos.y = lPos[1];
	pPos.z = lPos[2];

	pVel.x = lVel[0];
	pVel.y = lVel[1];
	pVel.z = lVel[2];
}

void SoundManagerFMod::Set3DDopplerFactor(float pFactor)
{
	FSOUND_3D_SetDopplerFactor(pFactor);
}

void SoundManagerFMod::Set3DRollOffFactor(float pFactor)
{
	FSOUND_3D_SetRolloffFactor(pFactor);
}

int SoundManagerFMod::GetChannel(SoundInstanceID pSoundIID)
{
	SoundInstanceTable::Iterator lSIIter = mSoundInstanceTable.Find(pSoundIID);
	if (lSIIter == mSoundInstanceTable.End())
	{
		return -1;
	}

	return (*lSIIter).mChannel;
}

void SoundManagerFMod::SetChorus(int pChannelIndex,
								int pFXIndex,
								float /*pDelay*/,
								float pFeedback,
								float pRate,
								float pDepth,
								float pWetness)
{
	int lFXID = mFXHandles.Get(pChannelIndex, pFXIndex);
	FSOUND_FX_SetChorus(lFXID, pWetness, pDepth, pFeedback, pRate, 1, 0, 2);
}

void SoundManagerFMod::SetFlanger(int pChannelIndex, 
								 int pFXIndex,
								 float /*pDelay*/,
								 float pFeedback,
								 float pRate,
								 float pDepth,
								 float pWetness)
{
	int lFXID = mFXHandles.Get(pChannelIndex, pFXIndex);
	FSOUND_FX_SetFlanger(lFXID, pWetness, pDepth, pFeedback, pRate, 1, 0, 2);
}

void SoundManagerFMod::SetCompressor(int pChannelIndex, 
									int pFXIndex,
									float pRatio,
									float pThreshold,
									float pAttack,
									float pRelease)
{
	int lFXID = mFXHandles.Get(pChannelIndex, pFXIndex);
	FSOUND_FX_SetCompressor(lFXID, 0, pAttack, pRelease, pThreshold, pRatio, 0);
}

void SoundManagerFMod::SetEcho(int pChannelIndex, 
							  int pFXIndex,
							  float pFeedback,
							  float pDelay,
							  float pWetness)
{
	int lFXID = mFXHandles.Get(pChannelIndex, pFXIndex);
	FSOUND_FX_SetEcho(lFXID, pWetness, pFeedback, pDelay, pDelay, 0);
}

void SoundManagerFMod::SetParamEQ(int pChannelIndex, 
								 int pFXIndex,
								 float pCenter,
								 float pBandwidth,
								 float pGain)
{
	int lFXID = mFXHandles.Get(pChannelIndex, pFXIndex);
	FSOUND_FX_SetParamEQ(lFXID, pCenter, pBandwidth, pGain);
}

void SoundManagerFMod::DiscardSoundInstance(SoundInstanceID pSoundIID)
{
	if (pSoundIID == mSoundInstanceIDManager.GetInvalidId())
	{
		return;
	}

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

	if (lSI.mAutoDelete == true)
	{
		mSoundInstanceIDManager.RecycleId(lSI.mSoundIID);
	}
}

void SoundManagerFMod::Clamp(float& pValue, float pMin, float pMax)
{
	if (pValue < pMin)
	{
		pValue = pMin;
	}
	
	if (pValue > pMax)
	{
		pValue = pMax;
	}
}

void SoundManagerFMod::Clamp(int& pValue, int pMin, int pMax)
{
	if (pValue < pMin)
	{
		pValue = pMin;
	}
	
	if (pValue > pMax)
	{
		pValue = pMax;
	}
}



LOG_CLASS_DEFINE(UI_SOUND, SoundManagerFMod);



}
