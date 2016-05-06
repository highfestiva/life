
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/string.h"
#include "../../lepra/include/thread.h"
#include "../../lepra/include/transformation.h"
#include "../../lepra/include/vector3d.h"
#include "uilepra.h"



namespace lepra {
class FileOpener;
}



namespace uilepra {



class SoundStream;



class SoundManager {
public:
	enum ContextType {
		kContextFmod,
		kContextOpenal,
	};
	static SoundManager* CreateSoundManager(ContextType type);

	SoundManager();
	virtual ~SoundManager();
	virtual bool Open() = 0;
	virtual void Close() = 0;
	virtual void Suspend() = 0;
	virtual void Resume() = 0;
	virtual bool IsIrreparableErrorState() const = 0;

	virtual float GetMasterVolume() const = 0;
	virtual void SetMasterVolume(float volume) = 0;
	virtual float GetMusicVolume() const = 0;
	virtual void SetMusicVolume(float volume) = 0;

	enum LoopMode {
		kLoopNone = 0,
		kLoopForward,
		kLoopPingpong,
	};

	/*
		Sample load & playback functions.
		2D refers to normal stereo/mono sounds.
		3D refers to sounds that should be played with 3D effects.
	*/

	typedef intptr_t SoundID;
	typedef intptr_t SoundInstanceID;

	// LoadSound2D/3D() loads an entire sound into memory. Returns the handle to the sound.
	virtual SoundID LoadSound2D(const str& file_name, LoopMode loop_mode, int priority) = 0;
	virtual SoundID LoadSound2D(const str& file_name, const void* data, size_t data_size, LoopMode loop_mode, int priority) = 0;
	virtual SoundID LoadSound3D(const str& file_name, LoopMode loop_mode, int priority) = 0;
	virtual SoundID LoadSound3D(const str& file_name, const void* data, size_t data_size, LoopMode loop_mode, int priority) = 0;
	virtual SoundStream* CreateSoundStream(const str& file_name, LoopMode loop_mode, int priority) = 0;

	virtual void Release(SoundID sound_id) = 0;

	// Returns the current time offset in a stream in seconds.
	virtual double GetStreamTime(SoundID sound_id) = 0;

	// Allocates a sound channel for a sound, and makes it ready to play.
	virtual SoundInstanceID CreateSoundInstance(SoundID sound_id) = 0;
	virtual void DeleteSoundInstance(SoundInstanceID sound_iid) = 0;

	// Play sound instance.
	// Set pitch = 1.0 when you want to keep the default pitch.
	virtual bool Play(SoundInstanceID sound_iid,
					  float volume,
					  float pitch) = 0;
	virtual void Stop(SoundInstanceID sound_iid) = 0;
	virtual void StopAll() = 0;
	virtual void TogglePause(SoundInstanceID sound_iid) = 0;	// Returns true when playing, false if paused.

	virtual bool IsPlaying(SoundInstanceID sound_iid) = 0;
	virtual bool IsPaused(SoundInstanceID sound_iid) = 0;

	// Panning: -1.0f = 100% left, 0.0f = middle, 1.0f = 100% right.
	virtual void SetPan(SoundInstanceID sound_iid, float pan) = 0;
	// Volume between 0.0f and 1.0f.
	virtual void SetVolume(SoundInstanceID sound_iid, float volume) = 0;
	virtual void SetPitch(SoundInstanceID sound_iid, float pitch) = 0;

	// Frequency in Hertz.
	virtual void SetFrequency(SoundInstanceID sound_iid, int frequency) = 0;
	virtual int GetFrequency(SoundInstanceID sound_iid) = 0;

	/*
		3D functions.

		Velocity is measured in meters per second.
		Orientation in world space.
	*/

	void SetSoundPosition(SoundInstanceID sound_iid, const vec3& pos, const vec3& vel);
	void SetCurrentListener(int listener_index, int listener_count);
	void SetListenerPosition(const vec3& pos, const vec3& vel,
		const vec3& up, const vec3& forward);

	// 0 removes the doppler effect, values greater than 1 exaggerates the effect.
	// The speed of sound using a doppler factor of 1 is 340 m/s.
	virtual void SetDopplerFactor(float factor) = 0;

	// Sets the global attenuation rolloff factor.
	// Normally volume for a sample will scale at 1 / distance. This gives a logarithmic
	// attenuation of volume as the source gets further away (or closer).
	// Setting this value makes the sound drop off faster or slower. The higher the value, the
	// faster volume will fall off. The lower the value, the slower it will fall off.
	// For example a rolloff factor of 1 will simulate the real world, where as a value of 2 will
	// make sounds attenuate 2 times quicker.
	virtual void SetRollOffFactor(float factor) = 0;

	virtual void SetChorus(SoundInstanceID sound_iid,
				   int fx_index,
				   float delay,
				   float feedback,
				   float rate,
				   float depth,		// Mod amount...
				   float wetness) = 0;

	virtual void SetFlanger(SoundInstanceID sound_iid,
					int fx_index,
					float delay,
					float feedback,
					float rate,
					float depth,		// Mod amount...
					float wetness) = 0;

	virtual void SetCompressor(SoundInstanceID sound_iid,
					   int fx_index,
					   float ratio,
					   float threshold,
					   float attack,
					   float release) = 0;

	virtual void SetEcho(SoundInstanceID sound_iid,
				 int fx_index,
				 float feedback,
				 float delay,
				 float wetness) = 0;

	virtual void SetParamEQ(SoundInstanceID sound_iid,
					int fx_index,
					float center,
					float bandwidth,
					float gain) = 0;

	virtual void SetFileOpener(FileOpener* opener) = 0;	// Takes over ownership of opener.

protected:
	virtual void DoSetSoundPosition(SoundInstanceID sound_iid, const vec3& pos, const vec3& vel) = 0;

	struct MicrophoneLocation {
		xform transform_;
		xform velocity_transform_;
	};
	typedef std::vector<MicrophoneLocation> MicrophoneLocationArray;
	MicrophoneLocationArray microphone_array_;
	size_t current_microphone_;

	mutable Lock lock_;

private:
};



const SoundManager::SoundID INVALID_SOUNDID = 0;
const SoundManager::SoundInstanceID INVALID_SOUNDINSTANCEID = 0;



}
