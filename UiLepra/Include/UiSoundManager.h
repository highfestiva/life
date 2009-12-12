
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Vector3D.h"
#include "UiLepra.h"



namespace UiLepra
{



class SoundManager
{
public:
	enum ContextType
	{
		CONTEXT_FMOD,
	};
	static SoundManager* CreateSoundManager(ContextType pType);

	SoundManager();
	virtual ~SoundManager();

	enum LoopMode
	{
		LOOP_NONE = 0,
		LOOP_FORWARD,
		LOOP_PINGPONG,
	};

	// Updates the sound system. Should be called every frame.
	virtual void Update() = 0;

	/*
		Sample load & playback functions.
		2D refers to normal stereo/mono sounds.
		3D refers to sounds that should be played with 3D effects.
	*/

	enum SoundID
	{
		INVALID_SOUNDID = 0
	};
	enum SoundInstanceID
	{
		INVALID_SOUNDINSTANCEID = 0
	};

	// LoadSound2D/3D() loads an entire sound into memory. Returns the handle to the sound.
	virtual SoundID LoadSound2D(const Lepra::String& pFileName, LoopMode pLoopMode, int pPriority) = 0;
	virtual SoundID LoadSound3D(const Lepra::String& pFileName, LoopMode pLoopMode, int pPriority) = 0;
	virtual SoundID LoadStream(const Lepra::String& pFileName, LoopMode pLoopMode, int pPriority) = 0;

	virtual void Release(SoundID pSoundID) = 0;

	// Returns the current time offset in a stream in seconds.
	virtual double GetStreamTime(SoundID pStreamID) = 0;

	// Allocates a sound channel for a sound, and makes it ready to play.
	virtual SoundInstanceID CreateSoundInstance(SoundID pSoundID) = 0;
	virtual void DeleteSoundInstance(SoundInstanceID pSoundIID) = 0;

	// Play and forget... Don't bother about creating a sound instance.
	// Set pPitch = 1.0 when you want to keep the default pitch.
	virtual bool Play(SoundID pSoundID,
					  float pVolume,
					  float pPitch) = 0;

	// Play sound instance.
	// Set pPitch = 1.0 when you want to keep the default pitch.
	virtual bool Play(SoundInstanceID pSoundIID,
					  float pVolume,
					  float pPitch) = 0;
	virtual void Stop(SoundInstanceID pSoundIID) = 0;
	virtual void StopAll() = 0;
	virtual void Pause(SoundInstanceID pSoundIID) = 0;
	virtual void Unpause(SoundInstanceID pSoundIID) = 0;

	virtual bool IsPlaying(SoundInstanceID pSoundIID) = 0;
	virtual bool IsPaused(SoundInstanceID pSoundIID) = 0;

	// Panning: -1.0f = 100% left, 0.0f = middle, 1.0f = 100% right.
	virtual void SetPan(SoundInstanceID pSoundIID, float pPan) = 0;
	virtual float GetPan(SoundInstanceID pSoundIID) = 0;

	// Volume between 0.0f and 1.0f.
	virtual void SetVolume(SoundInstanceID pSoundIID, float pVolume) = 0;
	virtual float GetVolume(SoundInstanceID pSoundIID) = 0;

	// Frequency in Hertz.
	virtual void SetFrequency(SoundInstanceID pSoundIID, int pFrequency) = 0;
	virtual int GetFrequency(SoundInstanceID pSoundIID) = 0;

	/*
		3D functions. 
		
		Velocity is measured in meters per second.
		Orientation in world space.
	*/

	virtual void Set3DSoundAttributes(SoundInstanceID pSoundIID, const Lepra::Vector3DF& pPos, const Lepra::Vector3DF& pVel) = 0;
	virtual void Get3DSoundAttributes(SoundInstanceID pSoundIID, Lepra::Vector3DF& pPos, Lepra::Vector3DF& pVel) = 0;

	virtual void Set3dCurrentListener(int pListenerIndex, int pListenerCount) = 0;
	virtual void Set3DListenerAttributes(const Lepra::Vector3DF& pPos, const Lepra::Vector3DF& pVel,
		const Lepra::Vector3DF& pUp, const Lepra::Vector3DF& pForward) = 0;
	virtual void Get3DListenerAttributes(Lepra::Vector3DF& pPos, Lepra::Vector3DF& pVel,
		Lepra::Vector3DF& pUp, Lepra::Vector3DF& pForward) = 0;

	// 0 removes the doppler effect, values greater than 1 exaggerates the effect.
	// The speed of sound using a doppler factor of 1 is 340 m/s.
	virtual void Set3DDopplerFactor(float pFactor) = 0;

	// Sets the global attenuation rolloff factor.
	// Normally volume for a sample will scale at 1 / distance. This gives a logarithmic 
	// attenuation of volume as the source gets further away (or closer).
	// Setting this value makes the sound drop off faster or slower. The higher the value, the 
	// faster volume will fall off. The lower the value, the slower it will fall off.
	// For example a rolloff factor of 1 will simulate the real world, where as a value of 2 will
	// make sounds attenuate 2 times quicker.
	virtual void Set3DRollOffFactor(float pFactor) = 0;
protected:
private:
};



}
