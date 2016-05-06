
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#if !defined(LEPRA_WITHOUT_FMOD)

#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/idmanager.h"
#include "../../lepra/include/string.h"
#include "../../thirdparty/fmod/api/inc/fmod.h"
#include "uisoundmanager.h"



namespace uilepra {



class SoundManagerFMod: public SoundManager {
	typedef SoundManager Parent;
public:
	SoundManagerFMod(int mix_rate);
	virtual ~SoundManagerFMod();

	virtual float GetMasterVolume() const;
	virtual void SetMasterVolume(float volume);

	virtual SoundID LoadSound2D(const str& file_name, LoopMode loop_mode, int priority);
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

	void SetDopplerFactor(float factor);
	void SetRollOffFactor(float factor);

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
	SoundID LoadSound(const str& file_name, const void* data, size_t data_size, LoopMode loop_mode, int priority, int extra_flags);
	virtual void DoSetSoundPosition(SoundInstanceID sound_iid, const vec3& pos, const vec3& vel);

private:
	int GetChannel(SoundInstanceID sound_iid);

	class Sample {
	public:
		Sample() :
			id_(INVALID_SOUNDID),
			reference_count_(1),
			sample_(0),
			stream_(0) {
		}

		SoundID id_;
		int reference_count_;
		str file_name_;
		FSOUND_SAMPLE* sample_;
		FSOUND_STREAM* stream_;
	};

	class FXHandles {
	public:
		FXHandles() :
			num_channels_(0),
			num_fx_(0),
			fxid_(0) {
		}

		FXHandles(int num_channels, int num_fx) :
			num_channels_(num_channels),
			num_fx_(num_fx),
			fxid_(0) {
			fxid_ = new int*[num_channels_];
			for (int i = 0; i < num_channels_; i++) {
				fxid_[i] = new int[num_fx_];
			}
		}

		virtual ~FXHandles() {
			for (int i = 0; i < num_channels_; i++) {
				delete[] fxid_[i];
			}

			if (fxid_ != 0)
				delete[] fxid_;

			fxid_ = 0;
		}

		void Reset(int num_channels, int num_fx) {
			int i;
			for (i = 0; i < num_channels_; i++) {
				delete[] fxid_[i];
			}

			delete[] fxid_;

			num_channels_ = num_channels;
			num_fx_ = num_fx;

			fxid_ = new int*[num_channels_];
			for (i = 0; i < num_channels_; i++) {
				fxid_[i] = new int[num_fx_];
			}
		}

		void Set(int channel, int fx_index, int fxid) {
			fxid_[channel][fx_index] = fxid;
		}

		int Get(int channel, int fx_index) {
			if (channel < 0 || channel >= num_channels_ ||
			   fx_index < 0 || fx_index >= num_fx_) {
				return -1;
			}

			return fxid_[channel][fx_index];
		}

	private:
		int num_channels_;
		int num_fx_;
		int** fxid_;
	};

	class SoundInstance {
	public:
		inline SoundInstance() :
			sound_id_((SoundID)-1),
			sound_iid_((SoundInstanceID)-1),
			channel_(-1) {
		}

		SoundID sound_id_;
		SoundInstanceID sound_iid_;
		int channel_;
	};

	class Channel {
	public:
		SoundInstance sound_instance_;
	};

	typedef HashTable<str, Sample*> FileNameToSampleTable;
	typedef HashTable<int, Sample*> IDToSampleTable;
	typedef HashTable<int, SoundInstance> SoundInstanceTable;

	FileNameToSampleTable file_name_to_sample_table_;
	IDToSampleTable id_to_sample_table_;
	SoundInstanceTable sound_instance_table_;

	IdManager<SoundID> sample_id_manager_;
	IdManager<SoundID> stream_id_manager_;
	IdManager<SoundInstanceID> sound_instance_id_manager_;

	FXHandles fx_handles_;

	Channel* channel_;
	int num_channels_;

	logclass();
};



}



#endif // !WITHOUT_FMOD
