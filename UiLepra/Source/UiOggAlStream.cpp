
// Thanks Jesse Maurais for "OpenAL Lesson 8: OggVorbis Streaming Using The Source Queue"!



#include "pch.h"
#include "../include/uioggalstream.h"
#include <ogg/ogg.h>
#include "../include/uisoundmanager.h"

#ifdef LEPRA_MSVC
#pragma warning(disable: 4996)	// fopen unsafe.
#endif // Visual
#define kBufferSize	(4096 * 8)
#define AL_CHECK()	if (alGetError() != AL_NO_ERROR) return false;



namespace uilepra {



OggAlStream::OggAlStream(SoundManager* sound_manager, const str& filename, bool loop):
	Parent(sound_manager) {
	is_looping_ = loop;
	Open(filename);
}

OggAlStream::~OggAlStream() {
	Release();
}

bool OggAlStream::Playback() {
	if (IsPlaying()) {
		return true;
	}

	if (!Stream(al_buffers_[0]) || !Stream(al_buffers_[1])) {
		return false;
	}

	alSourceQueueBuffers(al_source_, 2, al_buffers_);
	alSourcef(al_source_, AL_GAIN, volume_ * sound_manager_->GetMusicVolume());
	alSourcePlay(al_source_);
	return true;
}

bool OggAlStream::Rewind() {
	return (ov_raw_seek(&ogg_stream_, 0) == 0);
}

bool OggAlStream::IsPlaying() const {
	ALenum state;
	alGetSourcei(al_source_, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

bool OggAlStream::Stop() {
	Pause();
	return Rewind();
}

bool OggAlStream::Pause() {
	alSourceStop(al_source_);
	return Clear();
}

bool OggAlStream::Update() {
	bool is_active = true;
	int processed_buffer_count;
	alGetSourcei(al_source_, AL_BUFFERS_PROCESSED, &processed_buffer_count);
	alSourcef(al_source_, AL_GAIN, volume_ * sound_manager_->GetMusicVolume());
	AL_CHECK();
	while (processed_buffer_count--) {
		ALuint buffer;
		alSourceUnqueueBuffers(al_source_, 1, &buffer);
		AL_CHECK();

		is_active = Stream(buffer);

		alSourceQueueBuffers(al_source_, 1, &buffer);
		AL_CHECK();
	}
	if (!is_active) {
		if (is_looping_) {
			Clear();
			is_active = Rewind();
			TimeoutAutoResume();
		} else {
			Stop();
		}
	} else {
		TimeoutAutoResume();
	}
	return is_active;
}

bool OggAlStream::Open(const str& filename) {
	Release();

	if ((ogg_file_ = fopen(filename.c_str(), "rb")) == 0) {
		return is_open_;
	}

	int result;
	if ((result = ov_open(ogg_file_, &ogg_stream_, NULL, 0)) < 0) {	// Ogg/Vorbis takes ownership of file.
		fclose(ogg_file_);
		ogg_file_ = 0;
		return is_open_;
	}

	vorbis_info_ = ov_info(&ogg_stream_, -1);

	al_format_ = (vorbis_info_->channels == 1)? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	alGenBuffers(2, al_buffers_);
	AL_CHECK();
	alGenSources(1, &al_source_);
	AL_CHECK();

	alSource3f(al_source_, AL_POSITION,		0.0, 0.0, 0.0);
	alSource3f(al_source_, AL_VELOCITY,		0.0, 0.0, 0.0);
	alSource3f(al_source_, AL_DIRECTION,		0.0, 0.0, 0.0);
	alSourcef (al_source_, AL_ROLLOFF_FACTOR,	0.0);
	alSourcei (al_source_, AL_SOURCE_RELATIVE,	AL_TRUE);
	is_open_ = true;
	return is_open_;
}

bool OggAlStream::Release() {
	if (!is_open_) {
		return false;
	}

	alSourceStop(al_source_);
	if (!Clear()) {
		return false;
	}

	alDeleteSources(1, &al_source_);
	AL_CHECK();
	alDeleteBuffers(1, al_buffers_);
	AL_CHECK();

	ov_clear(&ogg_stream_);
	is_open_ = false;
	return true;
}

bool OggAlStream::Stream(ALuint buffer) {
	char data[kBufferSize];
	int size = 0;
	while (size < kBufferSize) {
		int section;
		int result = ov_read(&ogg_stream_, data + size, kBufferSize - size, 0, 2, 1, & section);
		if (result > 0) {
			size += result;
		} else if (result < 0) {
			return false;
		} else {
			break;
		}
	}

	if (size == 0) {
		return false;
	}

	alBufferData(buffer, al_format_, data, size, vorbis_info_->rate);
	AL_CHECK();
	return true;
}

bool OggAlStream::Clear() {
	int queued_buffer_count = 0;
	alGetSourcei(al_source_, AL_BUFFERS_QUEUED, &queued_buffer_count);
	while (queued_buffer_count--) {
		ALuint buffer;
		alSourceUnqueueBuffers(al_source_, 1, &buffer);
		AL_CHECK();
	}
	return true;
}

void OggAlStream::TimeoutAutoResume() {
	ALenum state;
	alGetSourcei(al_source_, AL_SOURCE_STATE, &state);
	if (state == AL_STOPPED) {
		alSourcePlay(al_source_);
	}
}



}
