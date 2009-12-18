
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

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

	void Update();

	SoundID LoadSound2D(const Lepra::String& pFileName, LoopMode pLoopMode, int pPriority);
	SoundID LoadSound3D(const Lepra::String& pFileName, LoopMode LoopMode, int pPriority);
	SoundID LoadStream(const Lepra::String& pFileName, LoopMode LoopMode, int pPriority);

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

	void SetFrequency(SoundInstanceID pSoundIID, int pFrequency);
	int GetFrequency(SoundInstanceID pSoundIID);

	void SetSoundPosition(SoundInstanceID pSoundIID, const Lepra::Vector3DF& pPos, const Lepra::Vector3DF& pVel);
	void GetSoundPosition(SoundInstanceID pSoundIID, Lepra::Vector3DF& pPos, Lepra::Vector3DF& pVel);

	void SetCurrentListener(int pListenerIndex, int pListenerCount);
	void SetListenerPosition(const Lepra::Vector3DF& pPos, const Lepra::Vector3DF& pVel,
		const Lepra::Vector3DF& pUp, const Lepra::Vector3DF& pForward);
	void GetListenerPosition(Lepra::Vector3DF& pPos, Lepra::Vector3DF& pVel,
		Lepra::Vector3DF& pUp, Lepra::Vector3DF& pForward);

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

protected:
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
		Lepra::String mFileName;
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

	typedef Lepra::HashTable<Lepra::String, Sample*> FileNameToSampleTable;
	typedef Lepra::HashTable<int, Sample*> IDToSampleTable;
	typedef Lepra::HashTable<int, SoundInstance> SoundInstanceTable;

	FileNameToSampleTable mFileNameToSampleTable;
	IDToSampleTable mIDToSampleTable;
	SoundInstanceTable mSoundInstanceTable;

	Lepra::IdManager<SoundID> mSampleIDManager;
	Lepra::IdManager<SoundID> mStreamIDManager;
	Lepra::IdManager<SoundInstanceID> mSoundInstanceIDManager;

	FXHandles mFXHandles;

	Channel* mChannel;
	int mNumChannels;

	LOG_CLASS_DECLARE();
};



}
