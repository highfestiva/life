
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/IdManager.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/String.h"
#include "../../ThirdParty/fmod/api/inc/fmod.h"
#include "UiSoundManager.h"



namespace UiLepra
{



class SoundManagerFMod: public SoundManager
{
public:
	friend class SoundManager;

	virtual ~SoundManagerFMod();

	void Update();

	/*
		Sample load & playback functions.
	*/

	SoundID LoadSound2D(const Lepra::String& pFileName, LoopMode pLoopMode = LOOP_NONE, int pPriority = 0);
	SoundID LoadSound3D(const Lepra::String& pFileName, LoopMode pLoopMode = LOOP_NONE, int pPriority = 0);
	SoundID LoadStream(const Lepra::String& pFileName, LoopMode pLoopMode = LOOP_NONE, int pPriority = 0);

	void Release(SoundID pSoundID);

	double GetStreamTime(SoundID pStreamID);

	SoundInstanceID CreateSoundInstance(SoundID pSoundID);
	void DeleteSoundInstance(SoundInstanceID pSoundIID);

	bool Play(SoundID pSoundID,
			  float pVolume,
			  float pPitch);
	bool Play(SoundInstanceID pSoundIID,
			  float pVolume,
			  float pPitch);

	void Stop(SoundInstanceID pSoundIID);
	void StopAll();
	void Pause(SoundInstanceID pSoundIID);
	void Unpause(SoundInstanceID pSoundIID);

	bool IsPlaying(SoundInstanceID pSoundIID);
	bool IsPaused(SoundInstanceID pSoundIID);

	void SetPan(SoundInstanceID pSoundIID, float pPan);
	float GetPan(SoundInstanceID pSoundIID);

	void SetVolume(SoundInstanceID pSoundIID, float pVolume);
	float GetVolume(SoundInstanceID pSoundIID);

	void SetFrequency(SoundInstanceID pSoundIID, int pFrequency);
	int GetFrequency(SoundInstanceID pSoundIID);

	void Set3DSoundAttributes(SoundInstanceID pSoundIID, const Lepra::Vector3DF& pPos, const Lepra::Vector3DF& pVel);
	void Get3DSoundAttributes(SoundInstanceID pSoundIID, Lepra::Vector3DF& pPos, Lepra::Vector3DF& pVel);

	void Set3dCurrentListener(int pListenerIndex, int pListenerCount);
	void Set3DListenerAttributes(const Lepra::Vector3DF& pPos, const Lepra::Vector3DF& pVel,
		const Lepra::Vector3DF& pUp, const Lepra::Vector3DF& pForward);
	void Get3DListenerAttributes(Lepra::Vector3DF& pPos, Lepra::Vector3DF& pVel,
		Lepra::Vector3DF& pUp, Lepra::Vector3DF& pForward);

	void Set3DDopplerFactor(float pFactor);
	void Set3DRollOffFactor(float pFactor);
	
	/*
		Sound FX.
	*/

	int GetChannel(SoundInstanceID pSoundIID);

	void SetChorus(int pChannelIndex, 
				   int pFXIndex,
				   float pDelay,
				   float pFeedback,
				   float pRate,
				   float pDepth,		// Mod amount...
				   float pWetness);

	void SetFlanger(int pChannelIndex, 
					int pFXIndex,
					float pDelay,
					float pFeedback,
					float pRate,
					float pDepth,		// Mod amount...
					float pWetness);

	void SetCompressor(int pChannelIndex, 
					   int pFXIndex,
					   float pRatio,
					   float pThreshold,
					   float pAttack,
					   float pRelease);

	void SetEcho(int pChannelIndex, 
				 int pFXIndex,
				 float pFeedback,
				 float pDelay,
				 float pWetness);

	void SetParamEQ(int pChannelIndex, 
					int pFXIndex,
					float pCenter,
					float pBandwidth,
					float pGain);

protected:
private:

	SoundInstanceID CreateSoundInstance(SoundID pSoundID, bool pAutoDelete);

	class Sample
	{
	public:
		Sample() :
			mID(-1),
			mReferenceCount(1),
			mSample(0),
			mStream(0)
		{
		}

		int mID;
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
			mChannel(-1),
			mAutoDelete(false)
		{
		}

		SoundID mSoundID;
		SoundInstanceID mSoundIID;
		int mChannel;
		bool mAutoDelete;
	};

	class Channel
	{
	public:
		SoundInstance mSoundInstance;
	};

	typedef Lepra::HashTable<Lepra::String, Sample*> FileNameToSampleTable;
	typedef Lepra::HashTable<int, Sample*> IDToSampleTable;
	typedef Lepra::HashTable<int, SoundInstance> SoundInstanceTable;

	SoundManagerFMod(int pMixRate = 44100);

	inline void Clamp(float& pValue, float pMin, float pMax);
	inline void Clamp(int& pValue, int pMin, int pMax);

	void DiscardSoundInstance(SoundInstanceID pSoundIID);

	bool mInitialized;

	FileNameToSampleTable mFileNameToSampleTable;
	IDToSampleTable mIDToSampleTable;
	SoundInstanceTable mSoundInstanceTable;

	Lepra::IdManager<int> mSampleIDManager;
	Lepra::IdManager<int> mStreamIDManager;
	Lepra::IdManager<int> mSoundInstanceIDManager;

	FXHandles mFXHandles;

	Channel* mChannel;
	int mNumChannels;

	LOG_CLASS_DECLARE();
};



}
