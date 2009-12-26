
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiSoundManager.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <set>
#include "../../Lepra/Include/String.h"



namespace UiLepra
{



class SoundManagerOpenAL: public SoundManager
{
	typedef SoundManager Parent;
public:
	SoundManagerOpenAL(int pMixRate);
	virtual ~SoundManagerOpenAL();

	SoundID LoadSound2D(const str& pFileName, LoopMode LoopMode, int pPriority);
	SoundID LoadSound3D(const str& pFileName, LoopMode LoopMode, int pPriority);
	SoundID LoadStream(const str& pFileName, LoopMode LoopMode, int pPriority);

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

	void SetSoundPosition(SoundInstanceID pSoundIID, const Vector3DF& pPos, const Vector3DF& pVel);
	void GetSoundPosition(SoundInstanceID pSoundIID, Vector3DF& pPos, Vector3DF& pVel);

	void SetCurrentListener(int pListenerIndex, int pListenerCount);
	void SetListenerPosition(const Vector3DF& pPos, const Vector3DF& pVel,
		const Vector3DF& pUp, const Vector3DF& pForward);
	void GetListenerPosition(Vector3DF& pPos, Vector3DF& pVel,
		Vector3DF& pUp, Vector3DF& pForward);

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

protected:
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

		ALuint mBuffer;
		bool mIsLooping;
		int mPriority;
		bool mIsAmbient;
		std::set<Source*> mSourceList;
	};

	struct Source
	{
		Source();
		~Source();
		bool SetSample(Sample* pSample, float pRollOffFactor);

		Sample* mSample;
		ALuint mSid;
		bool mIsPaused;
	};

	typedef std::set<Sample*> SampleSet;
	typedef std::set<Source*> SourceSet;

	ALCdevice* mDevice;
	ALCcontext* mContext;
	float mRollOffFactor;

	SampleSet mSampleSet;
	SourceSet mSourceSet;

	LOG_CLASS_DECLARE();
};



}
