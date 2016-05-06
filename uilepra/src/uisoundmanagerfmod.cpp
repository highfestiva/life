
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uisoundmanagerfmod.h"

#if !defined(LEPRA_WITHOUT_FMOD)



// TODO: refactor and remove unthinkable crap. Move resource ownership into resource manager where it belongs.



namespace uilepra {



SoundManagerFMod::SoundManagerFMod(int mix_rate):
	sample_id_manager_(1, 99999, 0),
	stream_id_manager_(100000, 199999, 0),
	sound_instance_id_manager_(200000, 299999, 0),
	channel_(0) {
	if (FSOUND_Init(mix_rate, 32, 0) != 0) {
		num_channels_ = FSOUND_GetMaxChannels();
		channel_ = new Channel[num_channels_];
		const float _pos[3] = {0, 0, 0};
		const float _vel[3] = {0, 0, 0};
		FSOUND_3D_Listener_SetAttributes(_pos, _vel, 0, 1, 0, 0, 0, 1);	// Forward along Y, up along Z.
	} else {
		log_.Error("Failed to initialize FMOD sound system!");
	}
}

SoundManagerFMod::~SoundManagerFMod() {
	FSOUND_Close();
	delete[] (channel_);
}




float SoundManagerFMod::GetMasterVolume() const {
	return 1;
}

void SoundManagerFMod::SetMasterVolume(float volume) {
	// TODO: implement.
	volume;
}



SoundManager::SoundID SoundManagerFMod::LoadSound2D(const str& file_name, LoopMode loop_mode, int priority) {
	return LoadSound(file_name, 0, 0, loop_mode, priority, FSOUND_2D);
}

SoundManager::SoundID SoundManagerFMod::LoadSound2D(const str& file_name, const void* data, size_t data_size, LoopMode loop_mode, int priority) {
	return LoadSound(file_name, data, data_size, loop_mode, priority, FSOUND_2D);
}

SoundManager::SoundID SoundManagerFMod::LoadSound3D(const str& file_name, LoopMode loop_mode, int priority) {
	return LoadSound(file_name, 0, 0, loop_mode, priority, FSOUND_HW3D);
}

SoundManager::SoundID SoundManagerFMod::LoadSound3D(const str& file_name, const void* data, size_t data_size, LoopMode loop_mode, int priority) {
	return LoadSound(file_name, data, data_size, loop_mode, priority, FSOUND_HW3D);
}

SoundStream* SoundManagerFMod::CreateSoundStream(const str&, LoopMode, int) {
	return 0;
}

void SoundManagerFMod::Release(SoundID sound_id) {
	IDToSampleTable::Iterator iter = id_to_sample_table_.Find((int)sound_id);
	if (iter != id_to_sample_table_.End()) {
		Sample* sample = *iter;
		sample->reference_count_--;

		if (sample->reference_count_ <= 0) {
			id_to_sample_table_.Remove(iter);

			file_name_to_sample_table_.Remove(sample->file_name_);

			if (sample->stream_ != 0) {
				FSOUND_Stream_Stop(sample->stream_);
				FSOUND_Stream_Close(sample->stream_);
				stream_id_manager_.RecycleId(sample->id_);
			}

			if (sample->sample_ != 0) {
				FSOUND_Sample_Free(sample->sample_);
				sample_id_manager_.RecycleId(sample->id_);
			}

			delete sample;
		}
	}
}

double SoundManagerFMod::GetStreamTime(SoundID sound_id) {
	IDToSampleTable::Iterator iter = id_to_sample_table_.Find((int)sound_id);
	if (iter != id_to_sample_table_.End()) {
		Sample* sample = *iter;
		if (sample->stream_ != 0) {
			return (double)FSOUND_Stream_GetTime(sample->stream_) / 1000.0;
		}
	}

	return -1.0;
}

SoundManager::SoundInstanceID SoundManagerFMod::CreateSoundInstance(SoundID sound_id) {
	IDToSampleTable::Iterator iter = id_to_sample_table_.Find((int)sound_id);

	if (iter == id_to_sample_table_.End()) {
		return (SoundInstanceID)sound_instance_id_manager_.GetInvalidId();
	}

	Sample* sample = *iter;

	// Start the sound in paused mode.
	int channel = -1;
	if (sample->sample_ != 0) {
		channel = FSOUND_PlaySoundEx(FSOUND_FREE, sample->sample_, 0, 1);
	} else if(sample->stream_ != 0) {
		channel = FSOUND_Stream_PlayEx(FSOUND_FREE, sample->stream_, 0, 1);
	}

	SoundInstanceID _sound_iid = (SoundInstanceID)sound_instance_id_manager_.GetInvalidId();
	if (channel >= 0) {
		channel &= 0x00000FFF;

		// Remove the currently playing sound instance. The ID is still allocated though.
		DeleteSoundInstance(channel_[channel].sound_instance_.sound_iid_);

		// Allocate a new ID and update the channel.
		_sound_iid = (SoundInstanceID)sound_instance_id_manager_.GetFreeId();

		channel_[channel].sound_instance_.sound_id_ = sound_id;
		channel_[channel].sound_instance_.sound_iid_ = _sound_iid;
		channel_[channel].sound_instance_.channel_ = channel;

		sound_instance_table_.Insert((int)_sound_iid, channel_[channel].sound_instance_);
	}

	return _sound_iid;
}

void SoundManagerFMod::DeleteSoundInstance(SoundInstanceID sound_iid) {
	SoundInstanceTable::Iterator iter;
	iter = sound_instance_table_.Find((int)sound_iid);
	if (iter == sound_instance_table_.End()) {
		return;
	}

	SoundInstance si = *iter;
	sound_instance_table_.Remove(iter);

	if (channel_[si.channel_].sound_instance_.sound_iid_ == sound_iid) {
		channel_[si.channel_].sound_instance_.sound_iid_ = (SoundInstanceID)sound_instance_id_manager_.GetInvalidId();
	}

	sound_instance_id_manager_.RecycleId(sound_iid);
}

bool SoundManagerFMod::Play(SoundInstanceID sound_iid, float volume, float pitch) {
	SoundInstanceTable::Iterator si_iter = sound_instance_table_.Find(sound_iid);
	if (si_iter == sound_instance_table_.End()) {
		return (false);
	}

	SoundInstance si = *si_iter;

	FSOUND_SetPan(si.channel_, FSOUND_STEREOPAN);
	FSOUND_SetVolume(si.channel_, (int)(volume * 255.0f));

	if (pitch != 1.0f) {
		// TODO: Fix the pitch to be relative to the sample frequency.
		FSOUND_SetFrequency(si.channel_, (int)(44100.0f * pitch) );
	}

	FSOUND_SetPaused(si.channel_, 0);
	return (true);
}

void SoundManagerFMod::StopAll() {
	for (int i = 0; i < FSOUND_GetMaxChannels(); i++) {
		FSOUND_StopSound(i);
	}
}

void SoundManagerFMod::Stop(SoundInstanceID sound_iid) {
	SoundInstanceTable::Iterator si_iter = sound_instance_table_.Find(sound_iid);
	if (si_iter == sound_instance_table_.End()) {
		return;
	}

	FSOUND_StopSound((*si_iter).channel_);
}

void SoundManagerFMod::TogglePause(SoundInstanceID sound_iid) {
	SoundInstanceTable::Iterator si_iter = sound_instance_table_.Find(sound_iid);
	if (si_iter == sound_instance_table_.End()) {
		return;
	}

	signed char set_pause = !FSOUND_GetPaused((*si_iter).channel_);
	FSOUND_SetPaused((*si_iter).channel_, set_pause);
}

bool SoundManagerFMod::IsPlaying(SoundInstanceID sound_iid) {
	SoundInstanceTable::Iterator si_iter = sound_instance_table_.Find(sound_iid);
	if (si_iter == sound_instance_table_.End()) {
		return false;
	}

	return (FSOUND_IsPlaying((*si_iter).channel_) != 0);
}

bool SoundManagerFMod::IsPaused(SoundInstanceID sound_iid) {
	SoundInstanceTable::Iterator si_iter = sound_instance_table_.Find(sound_iid);
	if (si_iter == sound_instance_table_.End()) {
		return false;
	}

	return (FSOUND_GetPaused((*si_iter).channel_) != 0);
}

void SoundManagerFMod::SetPan(SoundInstanceID sound_iid, float pan) {
	SoundInstanceTable::Iterator si_iter = sound_instance_table_.Find(sound_iid);
	if (si_iter == sound_instance_table_.End()) {
		return;
	}

	FSOUND_SetPan((*si_iter).channel_, (int)((pan + 1.0f) * 127.5f));
}

void SoundManagerFMod::SetVolume(SoundInstanceID sound_iid, float volume) {
	SoundInstanceTable::Iterator si_iter = sound_instance_table_.Find(sound_iid);
	if (si_iter == sound_instance_table_.End()) {
		return;
	}

	FSOUND_SetVolume((*si_iter).channel_, (int)(volume * 255.0f));
}

void SoundManagerFMod::SetPitch(SoundInstanceID sound_iid, float pitch) {
	SoundInstanceTable::Iterator si_iter = sound_instance_table_.Find(sound_iid);
	if (si_iter == sound_instance_table_.End()) {
		return;
	}

	// TODO: Fix the pitch to be relative to the sample frequency.
	FSOUND_SetFrequency((*si_iter).channel_, (int)(44100.0f * pitch) );
}

void SoundManagerFMod::SetFrequency(SoundInstanceID sound_iid, int frequency) {
	SoundInstanceTable::Iterator si_iter = sound_instance_table_.Find(sound_iid);
	if (si_iter == sound_instance_table_.End()) {
		return;
	}

	FSOUND_SetFrequency((*si_iter).channel_, frequency);
}

int SoundManagerFMod::GetFrequency(SoundInstanceID sound_iid) {
	SoundInstanceTable::Iterator si_iter = sound_instance_table_.Find(sound_iid);
	if (si_iter == sound_instance_table_.End()) {
		return 0;
	}

	return FSOUND_GetFrequency((*si_iter).channel_);
}

void SoundManagerFMod::SetDopplerFactor(float factor) {
	FSOUND_3D_SetDopplerFactor(factor);
}

void SoundManagerFMod::SetRollOffFactor(float factor) {
	FSOUND_3D_SetRolloffFactor(factor);
}

void SoundManagerFMod::SetChorus(SoundInstanceID sound_iid, int fx_index, float /*delay*/, float feedback,
	float rate, float depth, float wetness) {
	int fxid = fx_handles_.Get(GetChannel(sound_iid), fx_index);
	FSOUND_FX_SetChorus(fxid, wetness, depth, feedback, rate, 1, 0, 2);
}

void SoundManagerFMod::SetFlanger(SoundInstanceID sound_iid, int fx_index, float /*delay*/, float feedback,
	float rate, float depth, float wetness) {
	int fxid = fx_handles_.Get(GetChannel(sound_iid), fx_index);
	FSOUND_FX_SetFlanger(fxid, wetness, depth, feedback, rate, 1, 0, 2);
}

void SoundManagerFMod::SetCompressor(SoundInstanceID sound_iid, int fx_index, float ratio, float threshold,
	float attack, float release) {
	int fxid = fx_handles_.Get(GetChannel(sound_iid), fx_index);
	FSOUND_FX_SetCompressor(fxid, 0, attack, release, threshold, ratio, 0);
}

void SoundManagerFMod::SetEcho(SoundInstanceID sound_iid, int fx_index, float feedback, float delay, float wetness) {
	int fxid = fx_handles_.Get(GetChannel(sound_iid), fx_index);
	FSOUND_FX_SetEcho(fxid, wetness, feedback, delay, delay, 0);
}

void SoundManagerFMod::SetParamEQ(SoundInstanceID sound_iid, int fx_index, float center, float bandwidth, float gain) {
	int fxid = fx_handles_.Get(GetChannel(sound_iid), fx_index);
	FSOUND_FX_SetParamEQ(fxid, center, bandwidth, gain);
}



void SoundManagerFMod::SetFileOpener(FileOpener*) {
}



SoundManager::SoundID SoundManagerFMod::LoadSound(const str& file_name, const void* data, size_t data_size, LoopMode loop_mode, int priority, int extra_flags) {
	FileNameToSampleTable::Iterator iter = file_name_to_sample_table_.Find(file_name);
	if (iter != file_name_to_sample_table_.End()) {
		(*iter)->reference_count_++;
		return (*iter)->id_;
	}

	unsigned flags = 0;
	switch(loop_mode) {
		case kLoopForward:
			flags = FSOUND_LOOP_NORMAL;
			break;
		case kLoopPingpong:
			flags = FSOUND_LOOP_BIDI;
			break;
		case kLoopNone:
		default:
			flags = FSOUND_LOOP_OFF;
			break;
	}

	flags |= extra_flags;

	Sample* sample = new Sample();
	sample->file_name_ = file_name;
	sample->id_ = sample_id_manager_.GetFreeId();
	if (data) {
		sample->sample_ = FSOUND_Sample_Load((int)sample->id_,
			(const char*)data, flags, 0, data_size);
	} else {
		sample->sample_ = FSOUND_Sample_Load((int)sample->id_,
			file_name.c_str(), flags, 0, 0);
	}

	if (sample->sample_ == 0) {
		sample_id_manager_.RecycleId(sample->id_);
		delete sample;
		return sample_id_manager_.GetInvalidId();
	}

	FSOUND_Sample_SetDefaults(sample->sample_, -1, -1, -1, priority);

	file_name_to_sample_table_.Insert(file_name, sample);
	id_to_sample_table_.Insert(sample->id_, sample);

	return sample->id_;
}

void SoundManagerFMod::DoSetSoundPosition(SoundInstanceID sound_iid, const vec3& pos, const vec3& vel) {
	SoundInstanceTable::Iterator si_iter = sound_instance_table_.Find(sound_iid);
	if (si_iter == sound_instance_table_.End()) {
		deb_assert(false);
		return;
	}

	float _pos[3];
	float _vel[3];

	_pos[0] = pos.x;
	_pos[1] = pos.y;
	_pos[2] = pos.z;

	_vel[0] = vel.x;
	_vel[1] = vel.y;
	_vel[2] = vel.z;

	FSOUND_3D_SetAttributes((*si_iter).channel_, _pos, _vel);
}



int SoundManagerFMod::GetChannel(SoundInstanceID sound_iid) {
	SoundInstanceTable::Iterator si_iter = sound_instance_table_.Find(sound_iid);
	if (si_iter == sound_instance_table_.End()) {
		deb_assert(false);
		return -1;
	}

	return (*si_iter).channel_;
}



loginstance(kUiSound, SoundManagerFMod);



}



#endif // !WITHOUT_FMOD
