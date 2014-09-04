
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "UiSoundManager.h"



namespace UiLepra
{



class SoundManagerMock: public SoundManager
{
	typedef SoundManager Parent;
public:
	SoundManagerMock();
	virtual ~SoundManagerMock();

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

protected:
	virtual void DoSetSoundPosition(SoundInstanceID pSoundIID, const vec3& pPos, const vec3& pVel);

private:
};



}
