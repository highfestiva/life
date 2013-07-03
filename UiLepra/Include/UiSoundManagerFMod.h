
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#if !defined(LEPRA_WITHOUT_FMOD)

#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/IdManager.h"
#include "../../Lepra/Include/String.h"
#include "../../ThirdParty/fmod/api/inc/fmod.h"
#include "UiSoundManager.h"



namespace UiLepra
{



class SoundManagerFMod: public SoundManager
{
	typedef SoundManager Parent;
public:
	SoundManagerFMod(int pMixRate);
	virtual ~SoundManagerFMod();

	virtual float GetMasterVolume() const;
	virtual void SetMasterVolume(float pVolume);

	virtual SoundID LoadSound2D(const str& pFileName, LoopMode pLoopMode, int pPriority);
	virtual SoundID LoadSound2D(const str& pFileName, const void* pData, size_t pDataSize, LoopMode pLoopMode, int pPriority);
	virtual SoundID LoadSound3D(const str& pFileName, LoopMode LoopMode, int pPriority);
	virtual SoundID LoadSound3D(const str& pFileName, const void* pData, size_t pDataSize, LoopMode pLoopMode, int pPriority);
	virtual SoundStream* CreateSoundStream(const str& pFileName, LoopMode pLoopMode, int pPriority);

	void Release(SoundID pSoundID);

	double GetStreamTime(SoundID pSoundID);

	SoundInstanceID CreateSoundInstance(SoundID pSoundID);
	void DeleteSoundInstance(SoundInstanceID pSoundIID);

	bool Play(SoundInstanceID pSoundIID,
			  float pVolume,
			  float pPitch);

	void Stop(SoundInstanceID pSoundIID);
	void StopAll();
	void TogglePause(SoundInstanceID pSoundIID);

	bool IsPlaying(SoundInstanceID pSoundIID);
	bool IsPaused(SoundInstanceID pSoundIID);

	void SetPan(SoundInstanceID pSoundIID, float pPan);
	void SetVolume(SoundInstanceID pSoundIID, float pVolume);
	void SetPitch(SoundInstanceID pSoundIID, float pPitch);

	void SetFrequency(SoundInstanceID pSoundIID, int pFrequency);
	int GetFrequency(SoundInstanceID pSoundIID);

	void SetDopplerFactor(float pFactor);
	void SetRollOffFactor(float pFactor);

	void SetChorus(SoundInstanceID pSoundIID, 
				   int pFXIndex,
				   float pDelay,
				   float pFeedback,
				   float pRate,
				   float pDepth,		// Mod amount...
				   float pWetness);

	void SetFlanger(SoundInstanceID pSoundIID, 
					int pFXIndex,
					float pDelay,
					float pFeedback,
					float pRate,
					float pDepth,		// Mod amount...
					float pWetness);

	void SetCompressor(SoundInstanceID pSoundIID, 
					   int pFXIndex,
					   float pRatio,
					   float pThreshold,
					   float pAttack,
					   float pRelease);

	void SetEcho(SoundInstanceID pSoundIID, 
				 int pFXIndex,
				 float pFeedback,
				 float pDelay,
				 float pWetness);

	void SetParamEQ(SoundInstanceID pSoundIID, 
					int pFXIndex,
					float pCenter,
					float pBandwidth,
					float pGain);

	virtual void SetFileOpener(FileOpener* pOpener);

protected:
	SoundID LoadSound(const str& pFileName, const void* pData, size_t pDataSize, LoopMode pLoopMode, int pPriority, int pExtraFlags);
	virtual void DoSetSoundPosition(SoundInstanceID pSoundIID, const Vector3DF& pPos, const Vector3DF& pVel);

private:
	int GetChannel(SoundInstanceID pSoundIID);

	class Sample
	{
	public:
		Sample() :
			mID(INVALID_SOUNDID),
			mReferenceCount(1),
			mSample(0),
			mStream(0)
		{
		}

		SoundID mID;
		int mReferenceCount;
		str mFileName;
		FSOUND_SAMPLE* mSample;
		FSOUND_STREAM* mStream;
	};

	class FXHandles
	{
	public:
		FXHandles() :
			mNumChannels(0),
			mNumFX(0),
			mFXID(0)
		{
		}

		FXHandles(int pNumChannels, int pNumFX) :
			mNumChannels(pNumChannels),
			mNumFX(pNumFX),
			mFXID(0)
		{
			mFXID = new int*[mNumChannels];
			for (int i = 0; i < mNumChannels; i++)
			{
				mFXID[i] = new int[mNumFX];
			}
		}

		virtual ~FXHandles()
		{
			for (int i = 0; i < mNumChannels; i++)
			{
				delete[] mFXID[i];
			}

			if (mFXID != 0)
				delete[] mFXID;

			mFXID = 0;
		}

		void Reset(int pNumChannels, int pNumFX)
		{
			int i;
			for (i = 0; i < mNumChannels; i++)
			{
				delete[] mFXID[i];
			}

			delete[] mFXID;

			mNumChannels = pNumChannels;
			mNumFX = pNumFX;

			mFXID = new int*[mNumChannels];
			for (i = 0; i < mNumChannels; i++)
			{
				mFXID[i] = new int[mNumFX];
			}
		}

		void Set(int pChannel, int pFXIndex, int pFXID)
		{
			mFXID[pChannel][pFXIndex] = pFXID;
		}

		int Get(int pChannel, int pFXIndex)
		{
			if (pChannel < 0 || pChannel >= mNumChannels ||
			   pFXIndex < 0 || pFXIndex >= mNumFX)
			{
				return -1;
			}

			return mFXID[pChannel][pFXIndex];
		}

	private:
		int mNumChannels;
		int mNumFX;
		int** mFXID;
	};

	class SoundInstance
	{
	public:
		inline SoundInstance() :
			mSoundID((SoundID)-1),
			mSoundIID((SoundInstanceID)-1),
			mChannel(-1)
		{
		}

		SoundID mSoundID;
		SoundInstanceID mSoundIID;
		int mChannel;
	};

	class Channel
	{
	public:
		SoundInstance mSoundInstance;
	};

	typedef HashTable<str, Sample*> FileNameToSampleTable;
	typedef HashTable<int, Sample*> IDToSampleTable;
	typedef HashTable<int, SoundInstance> SoundInstanceTable;

	FileNameToSampleTable mFileNameToSampleTable;
	IDToSampleTable mIDToSampleTable;
	SoundInstanceTable mSoundInstanceTable;

	IdManager<SoundID> mSampleIDManager;
	IdManager<SoundID> mStreamIDManager;
	IdManager<SoundInstanceID> mSoundInstanceIDManager;

	FXHandles mFXHandles;

	Channel* mChannel;
	int mNumChannels;

	LOG_CLASS_DECLARE();
};



}



#endif // !WITHOUT_FMOD
