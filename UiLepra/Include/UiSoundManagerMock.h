
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#pragma once

#include "uisoundmanager.h"



namespace uilepra {



class SoundManagerMock: public SoundManager {
	typedef SoundManager Parent;
public:
	SoundManagerMock();
	virtual ~SoundManagerMock();

	SoundID LoadSound2D(const str& file_name, LoopMode LoopMode, int priority);
	SoundID LoadSound3D(const str& file_name, LoopMode LoopMode, int priority);
	SoundID LoadStream(const str& file_name, LoopMode LoopMode, int priority);

	void Release(SoundID sound_id);

	double GetStreamTime(SoundID sound_id);

	SoundInstanceID CreateSoundInstance(SoundID sound_id);
	void DeleteSoundInstance(SoundInstanceID sound_iid);

	bool Play(SoundInstanceID sound_iid,
			  float volume,
			  float pitch);

	void Stop(SoundInstanceID sound_iid);
	void StopAll();
	void TogglePause(SoundInstanceID sound_iid);

	bool IsPlaying(SoundInstanceID sound_iid);
	bool IsPaused(SoundInstanceID sound_iid);

	void SetPan(SoundInstanceID sound_iid, float pan);
	void SetVolume(SoundInstanceID sound_iid, float volume);
	void SetPitch(SoundInstanceID sound_iid, float pitch);

	void SetFrequency(SoundInstanceID sound_iid, int frequency);
	int GetFrequency(SoundInstanceID sound_iid);

	void SetListenerPosition(const vec3& pos, const vec3& vel,
		const vec3& up, const vec3& forward);

	void SetDopplerFactor(float factor);
	void SetRollOffFactor(float factor);

	int GetChannel(SoundInstanceID sound_iid);

	void SetChorus(SoundInstanceID sound_iid,
				   int fx_index,
				   float delay,
				   float feedback,
				   float rate,
				   float depth,		// Mod amount...
				   float wetness);

	void SetFlanger(SoundInstanceID sound_iid,
					int fx_index,
					float delay,
					float feedback,
					float rate,
					float depth,		// Mod amount...
					float wetness);

	void SetCompressor(SoundInstanceID sound_iid,
					   int fx_index,
					   float ratio,
					   float threshold,
					   float attack,
					   float release);

	void SetEcho(SoundInstanceID sound_iid,
				 int fx_index,
				 float feedback,
				 float delay,
				 float wetness);

	void SetParamEQ(SoundInstanceID sound_iid,
					int fx_index,
					float center,
					float bandwidth,
					float gain);

protected:
	virtual void DoSetSoundPosition(SoundInstanceID sound_iid, const vec3& pos, const vec3& vel);

private:
};



}
