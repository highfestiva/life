
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uisoundmanageropenal.h"
#include "../../lepra/include/lepraassert.h"
#include "../../lepra/include/resourcetracker.h"
#include "../../lepra/include/systemmanager.h"
#include "../../thirdparty/freealut-1.1.0/include/AL/alut.h"
#include "../include/uichibixmalstream.h"
#include "../include/uioggalstream.h"



#ifdef LEPRA_WINDOWS
extern "C" ALC_API void ALC_APIENTRY alc_init(void);	// Not intended for this type of use, but LGPL OpenAL can't load dsound.dll from DllMain.
extern "C" ALC_API void ALC_APIENTRY alc_deinit(void);	// Not intended for this type of use, but LGPL OpenAL can't load dsound.dll from DllMain.
#endif // Windows

#ifdef LEPRA_DEBUG
#define OAL_ASSERT()	{ ALenum al_error = alGetError(); deb_assert(al_error == AL_NO_ERROR); }
#define OALUT_ASSERT()	{ ALenum alut_error = alutGetError(); deb_assert(alut_error == ALUT_ERROR_NO_ERROR); }
#else // !Debug
#define OAL_ASSERT()	alGetError()
#define OALUT_ASSERT()	alutGetError()
#endif // Debug / !debug


namespace uilepra {



SoundManagerOpenAL::SoundManagerOpenAL(int mix_rate):
	device_(0),
	context_(0),
	roll_off_factor_(1),
	master_volume_(1),
	music_volume_(1),
	mix_rate_(mix_rate),
	is_irreparable_error_state_(false) {
	Open();
}

SoundManagerOpenAL::~SoundManagerOpenAL() {
	OAL_ASSERT();
	SetFileOpener(0);
	Close();
}

bool SoundManagerOpenAL::Open() {
#ifdef LEPRA_WINDOWS
	alc_init();
	device_ = ::alcOpenDevice(0);
#else // !Windows
	device_ = ::alcOpenDevice("'((direction \"write\")) '((devices '(alsa sdl native null)))");
	if (!device_) {
		device_ = ::alcOpenDevice(0);
	}
#endif // Windows / !Windows
	if (!device_) {
		log_.Error("Could not open any sound device!");
#ifdef LEPRA_WINDOWS
		alc_deinit();
#endif // Windows
		return false;
	}

	const int attributes[3] = { ALC_FREQUENCY, mix_rate_, 0 };
	context_ = ::alcCreateContext(device_, attributes);
	if (!context_) {
		log_.Error("Could not create sound context!");
		alcCloseDevice(device_);
		device_ = 0;
#ifdef LEPRA_WINDOWS
		alc_deinit();
#endif // Windows
		return false;
	}
	::alcMakeContextCurrent(context_);
	OAL_ASSERT();

	::alutInitWithoutContext(0, 0);
	OALUT_ASSERT();

	const float _pos[3] = {0, 0, 0};
	const float _vel[3] = {0, 0, 0};
	const float direction[3+3] = {0, 1, 0, 0, 0, 1};	// Forward along Y, up along Z.
	::alListenerfv(AL_POSITION, _pos);
	::alListenerfv(AL_VELOCITY, _vel);
	::alListenerfv(AL_ORIENTATION, direction);

	is_irreparable_error_state_ = false;

	OAL_ASSERT();

	return true;
}

void SoundManagerOpenAL::Close() {
	StopAll();
	SourceSet::iterator x = source_set_.begin();
	for (; x != source_set_.end(); ++x) {
		delete (*x);
	}
	SampleSet::iterator y = sample_set_.begin();
	for (; y != sample_set_.end(); ++y) {
		delete (*y);
	}
	source_set_.clear();
	sample_set_.clear();

	OAL_ASSERT();

	::alutExit();
	OALUT_ASSERT();

	::alcMakeContextCurrent(0);
	if (context_) {
		::alcDestroyContext(context_);
		context_ = 0;
	}
	if (device_) {
		::alcCloseDevice(device_);
		device_ = 0;
	}

#ifdef LEPRA_WINDOWS
	alc_deinit();
#endif // Windows
}

void SoundManagerOpenAL::Suspend() {
	ScopeLock lock(&lock_);
	::alcMakeContextCurrent(0);
	::alcSuspendContext(context_);
	OAL_ASSERT();
}

void SoundManagerOpenAL::Resume() {
	ScopeLock lock(&lock_);
	::alcMakeContextCurrent(context_);
	::alcProcessContext(context_);
	OAL_ASSERT();
}

bool SoundManagerOpenAL::IsIrreparableErrorState() const {
	return is_irreparable_error_state_;
}


float SoundManagerOpenAL::GetMasterVolume() const {
	return master_volume_;
}

void SoundManagerOpenAL::SetMasterVolume(float volume) {
	master_volume_ = volume;
}

float SoundManagerOpenAL::GetMusicVolume() const {
	return master_volume_ * music_volume_;
}

void SoundManagerOpenAL::SetMusicVolume(float volume) {
	music_volume_ = volume;
}



SoundManager::SoundID SoundManagerOpenAL::LoadSound2D(const str& file_name, LoopMode loop_mode, int priority) {
	Sample* _sample = (Sample*)LoadSound3D(file_name, loop_mode, priority);
	if (_sample) {
		_sample->is_ambient_ = true;
	}
	return ((SoundID)_sample);
}

SoundManager::SoundID SoundManagerOpenAL::LoadSound2D(const str& file_name, const void* data, size_t data_size, LoopMode loop_mode, int priority) {
	Sample* _sample = (Sample*)LoadSound3D(file_name, data, data_size, loop_mode, priority);
	if (_sample) {
		_sample->is_ambient_ = true;
	}
	return ((SoundID)_sample);
}

SoundManager::SoundID SoundManagerOpenAL::LoadSound3D(const str& file_name, LoopMode loop_mode, int priority) {
	return LoadSound3D(file_name, 0, 0, loop_mode, priority);
}

SoundManager::SoundID SoundManagerOpenAL::LoadSound3D(const str& file_name, const void* data, size_t data_size, LoopMode loop_mode, int priority) {
	Sample* _sample = new Sample(loop_mode != kLoopNone, priority);
	bool ok = false;
	for (int x = 0; !ok && x < 3; ++x) {	// TRICKY: retry to avoid anti-virus glitches (for the file version) and sound driver glitches (for the raw data version).
		if (data) {
			ok = _sample->Load(data, data_size);
		} else {
			ok = _sample->Load(file_name);
		}
		if (!ok) {
			Thread::Sleep(0.05f);
		}
	}
	if (ok) {
		ScopeLock lock(&lock_);
		deb_assert(sample_set_.find(_sample) == sample_set_.end());
		sample_set_.insert(_sample);
	} else {
		log_.Errorf("Could not get load sound %s, thus not possible to create sound.", file_name.c_str());
		deb_assert(false);
		delete (_sample);
		_sample = 0;
	}
	return ((SoundID)_sample);
}

SoundStream* SoundManagerOpenAL::CreateSoundStream(const str& file_name, LoopMode loop_mode, int) {
	OAL_ASSERT();

	SoundStream* sound_stream;
	if (strutil::EndsWith(file_name, ".xm")) {
		sound_stream = new ChibiXmAlStream(this, file_name, loop_mode == kLoopForward);
	} else {
		sound_stream = new OggAlStream(this, SystemManager::GetDataDirectory()+file_name, loop_mode == kLoopForward);
	}
	if (!sound_stream->IsOpen()) {
		delete sound_stream;
		sound_stream = 0;
	}

	OAL_ASSERT();

	return sound_stream;;
}

void SoundManagerOpenAL::Release(SoundID sound_id) {
	ScopeLock lock(&lock_);
	Sample* _sample = GetSample(sound_id);
	if (!_sample) {
		return;
	}

	sample_set_.erase(sample_set_.find((Sample*)sound_id));
	delete (_sample);
}

double SoundManagerOpenAL::GetStreamTime(SoundID sound_id) {
	ScopeLock lock(&lock_);
	Sample* _sample = GetSample(sound_id);
	if (!_sample) {
		return (-1);
	}

	ALfloat source_time = 0;
	if (!_sample->source_list_.empty()) {
		Source* source = *_sample->source_list_.begin();
		ALint state = AL_PAUSED;
		alGetSourcei(source->sid_, AL_SOURCE_STATE, &state);
		if (state == AL_PLAYING) {
			::alGetSourcef(source->sid_, AL_SEC_OFFSET, &source_time);
		} else {
			source_time = 1e12f;	// Anything big is good, this sample is done.
		}
	}
	OAL_ASSERT();
	return (source_time);

}

SoundManager::SoundInstanceID SoundManagerOpenAL::CreateSoundInstance(SoundID sound_id) {
	ScopeLock lock(&lock_);
	Sample* _sample = GetSample(sound_id);
	if (!_sample) {
		log_.Errorf("Could not get sound sample %u, thus not possible to create sound instance.", sound_id);
		return (INVALID_SOUNDINSTANCEID);
	}

	Source* source = new Source;
	if (source->SetSample(_sample, roll_off_factor_)) {
		deb_assert(source_set_.find(source) == source_set_.end());
		source_set_.insert(source);
	} else {
		delete (source);
		source = 0;
	}
	return ((SoundInstanceID)source);
}

void SoundManagerOpenAL::DeleteSoundInstance(SoundInstanceID sound_iid) {
	ScopeLock lock(&lock_);
	Source* source = GetSource(sound_iid);
	if (!source) {
		deb_assert(false);
		return;
	}

	source_set_.erase(source_set_.find((Source*)sound_iid));
	delete (source);
}

bool SoundManagerOpenAL::Play(SoundInstanceID sound_iid, float volume, float pitch) {
	ScopeLock lock(&lock_);
	Source* source = GetSource(sound_iid);
	if (!source) {
		return (false);
	}

	::alSourcef(source->sid_, AL_GAIN, volume * master_volume_);
	::alSourcef(source->sid_, AL_PITCH, pitch);
	::alSourcePlay(source->sid_);
	if (alGetError() != AL_NO_ERROR) {
		is_irreparable_error_state_ = true;
	}
	return (true);
}

void SoundManagerOpenAL::StopAll() {
	ScopeLock lock(&lock_);
	SourceSet::iterator x = source_set_.begin();
	for (; x != source_set_.end(); ++x) {
		::alSourceStop((*x)->sid_);
	}
	OAL_ASSERT();
}

void SoundManagerOpenAL::Stop(SoundInstanceID sound_iid) {
	ScopeLock lock(&lock_);
	Source* source = GetSource(sound_iid);
	if (!source) {
		return;
	}
	::alSourceStop(source->sid_);
	OAL_ASSERT();
}

void SoundManagerOpenAL::TogglePause(SoundInstanceID sound_iid) {
	ScopeLock lock(&lock_);
	Source* source = GetSource(sound_iid);
	if (!source) {
		return;
	}
	ALint state = AL_PAUSED;
	alGetSourcei(source->sid_, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING) {
		::alSourcePlay(source->sid_);
	} else {
		::alSourcePause(source->sid_);
	}
	OAL_ASSERT();
}

bool SoundManagerOpenAL::IsPlaying(SoundInstanceID sound_iid) {
	ScopeLock lock(&lock_);
	Source* source = GetSource(sound_iid);
	if (!source) {
		return (false);
	}
	ALint state = AL_PAUSED;
	alGetSourcei(source->sid_, AL_SOURCE_STATE, &state);
	OAL_ASSERT();
	return (state == AL_PLAYING);
}

bool SoundManagerOpenAL::IsPaused(SoundInstanceID sound_iid) {
	return (!IsPlaying(sound_iid));
}

void SoundManagerOpenAL::SetPan(SoundInstanceID sound_iid, float) {
	ScopeLock lock(&lock_);
	Source* source = GetSource(sound_iid);
	if (!source) {
		return;
	}
	//source->SetPan(pan);
}

void SoundManagerOpenAL::SetVolume(SoundInstanceID sound_iid, float volume) {
	ScopeLock lock(&lock_);
	Source* source = GetSource(sound_iid);
	if (!source || volume <= 0) {
		return;
	}
	::alSourcef(source->sid_, AL_GAIN, volume * master_volume_);
	OAL_ASSERT();
}

void SoundManagerOpenAL::SetPitch(SoundInstanceID sound_iid, float pitch) {
	ScopeLock lock(&lock_);
	Source* source = GetSource(sound_iid);
	if (!source || pitch <= 0) {
		return;
	}
	::alSourcef(source->sid_, AL_PITCH, pitch);
	OAL_ASSERT();
}

void SoundManagerOpenAL::SetFrequency(SoundInstanceID, int) {
}

int SoundManagerOpenAL::GetFrequency(SoundInstanceID) {
	return (0);
}

void SoundManagerOpenAL::SetDopplerFactor(float factor) {
	ScopeLock lock(&lock_);
	::alDopplerFactor(factor);
	OAL_ASSERT();
}

void SoundManagerOpenAL::SetRollOffFactor(float factor) {
	ScopeLock lock(&lock_);
	roll_off_factor_ = factor;
	SourceSet::iterator x = source_set_.begin();
	for (; x != source_set_.end(); ++x) {
		::alSourcef((*x)->sid_, AL_ROLLOFF_FACTOR, factor);
	}
	OAL_ASSERT();
}

void SoundManagerOpenAL::SetChorus(SoundInstanceID, int, float, float, float, float, float) {
}

void SoundManagerOpenAL::SetFlanger(SoundInstanceID, int, float, float, float, float, float) {
}

void SoundManagerOpenAL::SetCompressor(SoundInstanceID, int, float, float, float, float) {
}

void SoundManagerOpenAL::SetEcho(SoundInstanceID, int, float, float, float) {
}

void SoundManagerOpenAL::SetParamEQ(SoundInstanceID, int, float, float, float) {
}



void SoundManagerOpenAL::SetFileOpener(FileOpener* opener) {
	ChibiXmAlStream::SetFileOpener(opener);
}



void SoundManagerOpenAL::DoSetSoundPosition(SoundInstanceID sound_iid, const vec3& pos, const vec3& vel) {
	ScopeLock lock(&lock_);
	Source* source = GetSource(sound_iid);
	if (!source) {
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
	::alSourcefv(source->sid_, AL_POSITION, _pos);
	::alSourcefv(source->sid_, AL_VELOCITY, _vel);

	OAL_ASSERT();
}



SoundManagerOpenAL::Sample* SoundManagerOpenAL::GetSample(SoundID sound_id) const {
	Sample* _sample = (Sample*)sound_id;
	SampleSet::const_iterator x = sample_set_.find(_sample);
	if (x == sample_set_.end()) {
		deb_assert(false);
		_sample = 0;
	}
	return (_sample);
}

SoundManagerOpenAL::Source* SoundManagerOpenAL::GetSource(SoundInstanceID sound_instance_id) const {
	Source* source = (Source*)sound_instance_id;
	SourceSet::const_iterator x = source_set_.find(source);
	if (x == source_set_.end()) {
		deb_assert(false);
		source = 0;
	}
	return (source);
}



SoundManagerOpenAL::Sample::Sample(bool looping, int priority):
	buffer_(AL_NONE),
	is_looping_(looping),
	priority_(priority),
	is_ambient_(false) {
}

SoundManagerOpenAL::Sample::~Sample() {
	OAL_ASSERT();
	if (buffer_ != AL_NONE) {
		::alDeleteBuffers(1, &buffer_);
		buffer_ = AL_NONE;
		const int error = ::alGetError();
		if (error != AL_NO_ERROR) {
			log_.Errorf("Could not delete OpenAL buffer (%4.4X).", error);
		}
		LEPRA_RELEASE_RESOURCE(alBuffer);
	}
	OAL_ASSERT();
}

bool SoundManagerOpenAL::Sample::Load(const str& file_name) {
	deb_assert(buffer_ == AL_NONE);
	LEPRA_ACQUIRE_RESOURCE(alBuffer);
	buffer_ = ::alutCreateBufferFromFile(file_name.c_str());
	OALUT_ASSERT();
	return (buffer_ != AL_NONE);
}

bool SoundManagerOpenAL::Sample::Load(const void* data, size_t data_size) {
	deb_assert(buffer_ == AL_NONE);
	LEPRA_ACQUIRE_RESOURCE(alBuffer);
	buffer_ = ::alutCreateBufferFromFileImage(data, data_size);
	OALUT_ASSERT();
	return (buffer_ != AL_NONE);
}



SoundManagerOpenAL::Source::Source():
	sample_(0),
	sid_((ALuint)-1) {
}

SoundManagerOpenAL::Source::~Source() {
	OAL_ASSERT();
	if (sample_) {
		sample_->source_list_.erase(this);
		sample_ = 0;
	}
	if (sid_ != (ALuint)-1) {
		::alSourceStop(sid_);
		OAL_ASSERT();
		::alDeleteSources(1, &sid_);
		OAL_ASSERT();
		sid_ = (ALuint)-1;
		LEPRA_RELEASE_RESOURCE(alSources);
	}
	OAL_ASSERT();
}

bool SoundManagerOpenAL::Source::SetSample(Sample* sample, float roll_off_factor) {
	deb_assert(sid_ == (ALuint)-1);
	LEPRA_ACQUIRE_RESOURCE(alSources);
	::alGenSources(1, &sid_);
	if (sid_ == (ALuint)-1) {
		const int error = ::alGetError();
		if (error != AL_NO_ERROR) {
			log_.Errorf("Could not generate OpenAL source (%4.4X), thus not possible to create sound instance.", error);
		}
		return false;
	}

	deb_assert(sample_ == 0);
	sample_ = sample;
	deb_assert(sample_->source_list_.find(this) == sample_->source_list_.end());
	sample_->source_list_.insert(this);

	::alSourcei(sid_, AL_LOOPING, sample_->is_looping_? 1 : 0);
	if (sample_->is_ambient_) {
		::alSourcei(sid_, AL_SOURCE_RELATIVE, AL_TRUE);
		::alSourcef(sid_, AL_ROLLOFF_FACTOR, 0);
		::alSource3f(sid_, AL_DIRECTION, 0,0,0);	// Omnidirectional sound source.
	} else {
		::alSourcei(sid_, AL_SOURCE_RELATIVE, AL_FALSE);
		::alSourcef(sid_, AL_ROLLOFF_FACTOR, roll_off_factor);
	}
	::alSourcei(sid_, AL_BUFFER, sample_->buffer_);
	OAL_ASSERT();
	return true;
}



loginstance(kUiSound, SoundManagerOpenAL);



}
