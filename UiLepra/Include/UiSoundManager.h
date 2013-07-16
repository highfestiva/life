
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Thread.h"
#include "../../Lepra/Include/Transformation.h"
#include "../../Lepra/Include/Vector3D.h"
#include "UiLepra.h"



namespace Lepra
{
class FileOpener;
}



namespace UiLepra
{



class SoundStream;



class SoundManager
{
public:
	enum ContextType
	{
		CONTEXT_FMOD,
		CONTEXT_OPENAL,
	};
	static SoundManager* CreateSoundManager(ContextType pType);

	SoundManager();
	virtual ~SoundManager();
	virtual bool Open() = 0;
	virtual void Close() = 0;
	virtual void Suspend() = 0;
	virtual void Resume() = 0;
	virtual bool IsIrreparableErrorState() const = 0;

	virtual float GetMasterVolume() const = 0;
	virtual void SetMasterVolume(float pVolume) = 0;

	enum LoopMode
	{
		LOOP_NONE = 0,
		LOOP_FORWARD,
		LOOP_PINGPONG,
	};

	/*
		Sample load & playback functions.
		2D refers to normal stereo/mono sounds.
		3D refers to sounds that should be played with 3D effects.
	*/

	typedef intptr_t SoundID;
	typedef intptr_t SoundInstanceID;

	// LoadSound2D/3D() loads an entire sound into memory. Returns the handle to the sound.
	virtual SoundID LoadSound2D(const str& pFileName, LoopMode pLoopMode, int pPriority) = 0;
	virtual SoundID LoadSound2D(const str& pFileName, const void* pData, size_t pDataSize, LoopMode pLoopMode, int pPriority) = 0;
	virtual SoundID LoadSound3D(const str& pFileName, LoopMode pLoopMode, int pPriority) = 0;
	virtual SoundID LoadSound3D(const str& pFileName, const void* pData, size_t pDataSize, LoopMode pLoopMode, int pPriority) = 0;
	virtual SoundStream* CreateSoundStream(const str& pFileName, LoopMode pLoopMode, int pPriority) = 0;

	virtual void Release(SoundID pSoundID) = 0;

	// Returns the current time offset in a stream in seconds.
	virtual double GetStreamTime(SoundID pSoundID) = 0;

	// Allocates a sound channel for a sound, and makes it ready to play.
	virtual SoundInstanceID CreateSoundInstance(SoundID pSoundID) = 0;
	virtual void DeleteSoundInstance(SoundInstanceID pSoundIID) = 0;

	// Play sound instance.
	// Set pPitch = 1.0 when you want to keep the default pitch.
	virtual bool Play(SoundInstanceID pSoundIID,
					  float pVolume,
					  float pPitch) = 0;
	virtual void Stop(SoundInstanceID pSoundIID) = 0;
	virtual void StopAll() = 0;
	virtual void TogglePause(SoundInstanceID pSoundIID) = 0;	// Returns true when playing, false if paused.

	virtual bool IsPlaying(SoundInstanceID pSoundIID) = 0;
	virtual bool IsPaused(SoundInstanceID pSoundIID) = 0;

	// Panning: -1.0f = 100% left, 0.0f = middle, 1.0f = 100% right.
	virtual void SetPan(SoundInstanceID pSoundIID, float pPan) = 0;
	// Volume between 0.0f and 1.0f.
	virtual void SetVolume(SoundInstanceID pSoundIID, float pVolume) = 0;
	virtual void SetPitch(SoundInstanceID pSoundIID, float pPitch) = 0;

	// Frequency in Hertz.
	virtual void SetFrequency(SoundInstanceID pSoundIID, int pFrequency) = 0;
	virtual int GetFrequency(SoundInstanceID pSoundIID) = 0;

	/*
		3D functions. 
		
		Velocity is measured in meters per second.
		Orientation in world space.
	*/

	void SetSoundPosition(SoundInstanceID pSoundIID, const Vector3DF& pPos, const Vector3DF& pVel);
	void SetCurrentListener(int pListenerIndex, int pListenerCount);
	void SetListenerPosition(const Vector3DF& pPos, const Vector3DF& pVel,
		const Vector3DF& pUp, const Vector3DF& pForward);

	// 0 removes the doppler effect, values greater than 1 exaggerates the effect.
	// The speed of sound using a doppler factor of 1 is 340 m/s.
	virtual void SetDopplerFactor(float pFactor) = 0;

	// Sets the global attenuation rolloff factor.
	// Normally volume for a sample will scale at 1 / distance. This gives a logarithmic 
	// attenuation of volume as the source gets further away (or closer).
	// Setting this value makes the sound drop off faster or slower. The higher the value, the 
	// faster volume will fall off. The lower the value, the slower it will fall off.
	// For example a rolloff factor of 1 will simulate the real world, where as a value of 2 will
	// make sounds attenuate 2 times quicker.
	virtual void SetRollOffFactor(float pFactor) = 0;

	virtual void SetChorus(SoundInstanceID pSoundIID, 
				   int pFXIndex,
				   float pDelay,
				   float pFeedback,
				   float pRate,
				   float pDepth,		// Mod amount...
				   float pWetness) = 0;

	virtual void SetFlanger(SoundInstanceID pSoundIID, 
					int pFXIndex,
					float pDelay,
					float pFeedback,
					float pRate,
					float pDepth,		// Mod amount...
					float pWetness) = 0;

	virtual void SetCompressor(SoundInstanceID pSoundIID, 
					   int pFXIndex,
					   float pRatio,
					   float pThreshold,
					   float pAttack,
					   float pRelease) = 0;

	virtual void SetEcho(SoundInstanceID pSoundIID, 
				 int pFXIndex,
				 float pFeedback,
				 float pDelay,
				 float pWetness) = 0;

	virtual void SetParamEQ(SoundInstanceID pSoundIID, 
					int pFXIndex,
					float pCenter,
					float pBandwidth,
					float pGain) = 0;

	virtual void SetFileOpener(FileOpener* pOpener) = 0;	// Takes over ownership of opener.

protected:
	virtual void DoSetSoundPosition(SoundInstanceID pSoundIID, const Vector3DF& pPos, const Vector3DF& pVel) = 0;

	struct MicrophoneLocation
	{
		TransformationF mTransform;
		TransformationF mVelocityTransform;
	};
	typedef std::vector<MicrophoneLocation> MicrophoneLocationArray;
	MicrophoneLocationArray mMicrophoneArray;
	size_t mCurrentMicrophone;

	mutable Lock mLock;

private:
};



const SoundManager::SoundID INVALID_SOUNDID = 0;
const SoundManager::SoundInstanceID INVALID_SOUNDINSTANCEID = 0;



}
