
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiSoundManager.h"
#if defined(LEPRA_MAC)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include "../../Lepra/Include/Unordered.h"
#include "../../Lepra/Include/String.h"



namespace UiLepra
{



class SoundManagerOpenAL: public SoundManager
{
	typedef SoundManager Parent;
public:
	SoundManagerOpenAL(int pMixRate);
	virtual ~SoundManagerOpenAL();
	virtual bool Open();
	virtual void Close();
	virtual void Suspend();
	virtual void Resume();
	virtual bool IsIrreparableErrorState() const;

	virtual float GetMasterVolume() const;
	virtual void SetMasterVolume(float pVolume);
	virtual float GetMusicVolume() const;
	virtual void SetMusicVolume(float pVolume);

	virtual SoundID LoadSound2D(const str& pFileName, LoopMode LoopMode, int pPriority);
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

	void SetListenerPosition(const vec3& pPos, const vec3& pVel,
		const vec3& pUp, const vec3& pForward);

	void SetDopplerFactor(float pFactor);
	void SetRollOffFactor(float pFactor);

	int GetChannel(SoundInstanceID pSoundIID);

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
	virtual void DoSetSoundPosition(SoundInstanceID pSoundIID, const vec3& pPos, const vec3& pVel);

private:
	struct Sample;
	struct Source;

	Sample* GetSample(SoundID) const;
	Source* GetSource(SoundInstanceID) const;

	struct Sample
	{
		Sample(bool pLooping, int pPriority);
		~Sample();
		bool Load(const str& pFileName);
		bool Load(const void* pData, size_t pDataSize);

		ALuint mBuffer;
		bool mIsLooping;
		int mPriority;
		bool mIsAmbient;
		std::unordered_set<Source*, LEPRA_VOIDP_HASHER> mSourceList;
	};

	struct Source
	{
		Source();
		~Source();
		bool SetSample(Sample* pSample, float pRollOffFactor);

		Sample* mSample;
		ALuint mSid;
	};

	typedef std::unordered_set<Sample*, LEPRA_VOIDP_HASHER> SampleSet;
	typedef std::unordered_set<Source*, LEPRA_VOIDP_HASHER> SourceSet;

	ALCdevice* mDevice;
	ALCcontext* mContext;
	float mRollOffFactor;
	float mMasterVolume;
	float mMusicVolume;
	int mMixRate;
	bool mIsIrreparableErrorState;

	SampleSet mSampleSet;
	SourceSet mSourceSet;

	logclass();
};



}
