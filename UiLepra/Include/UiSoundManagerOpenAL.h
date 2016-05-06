
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uisoundmanager.h"
#if defined(LEPRA_MAC)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif
#include "../../lepra/include/unordered.h"
#include "../../lepra/include/string.h"



namespace uilepra {



class SoundManagerOpenAL: public SoundManager {
	typedef SoundManager Parent;
public:
	SoundManagerOpenAL(int mix_rate);
	virtual ~SoundManagerOpenAL();
	virtual bool Open();
	virtual void Close();
	virtual void Suspend();
	virtual void Resume();
	virtual bool IsIrreparableErrorState() const;

	virtual float GetMasterVolume() const;
	virtual void SetMasterVolume(float volume);
	virtual float GetMusicVolume() const;
	virtual void SetMusicVolume(float volume);

	virtual SoundID LoadSound2D(const str& file_name, LoopMode LoopMode, int priority);
	virtual SoundID LoadSound2D(const str& file_name, const void* data, size_t data_size, LoopMode loop_mode, int priority);
	virtual SoundID LoadSound3D(const str& file_name, LoopMode LoopMode, int priority);
	virtual SoundID LoadSound3D(const str& file_name, const void* data, size_t data_size, LoopMode loop_mode, int priority);
	virtual SoundStream* CreateSoundStream(const str& file_name, LoopMode loop_mode, int priority);

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

	virtual void SetFileOpener(FileOpener* opener);

protected:
	virtual void DoSetSoundPosition(SoundInstanceID sound_iid, const vec3& pos, const vec3& vel);

private:
	struct Sample;
	struct Source;

	Sample* GetSample(SoundID) const;
	Source* GetSource(SoundInstanceID) const;

	struct Sample {
		Sample(bool looping, int priority);
		~Sample();
		bool Load(const str& file_name);
		bool Load(const void* data, size_t data_size);

		ALuint buffer_;
		bool is_looping_;
		int priority_;
		bool is_ambient_;
		std::unordered_set<Source*, LEPRA_VOIDP_HASHER> source_list_;
	};

	struct Source {
		Source();
		~Source();
		bool SetSample(Sample* sample, float roll_off_factor);

		Sample* sample_;
		ALuint sid_;
	};

	typedef std::unordered_set<Sample*, LEPRA_VOIDP_HASHER> SampleSet;
	typedef std::unordered_set<Source*, LEPRA_VOIDP_HASHER> SourceSet;

	ALCdevice* device_;
	ALCcontext* context_;
	float roll_off_factor_;
	float master_volume_;
	float music_volume_;
	int mix_rate_;
	bool is_irreparable_error_state_;

	SampleSet sample_set_;
	SourceSet source_set_;

	logclass();
};



}
