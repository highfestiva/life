
// Thanks Jesse Maurais for "OpenAL Lesson 8: OggVorbis Streaming Using The Source Queue"!



#include "pch.h"
#include "../Include/UiOggAlStream.h"
#include <ogg/ogg.h>
#include "../Include/UiSoundManager.h"

#ifdef LEPRA_MSVC
#pragma warning(disable: 4996)	// fopen unsafe.
#endif // Visual
#define BUFFER_SIZE	(4096 * 8)
#define AL_CHECK()	if (alGetError() != AL_NO_ERROR) return false;



namespace UiLepra
{



OggAlStream::OggAlStream(SoundManager* pSoundManager, const str& pFilename, bool pLoop):
	Parent(pSoundManager)
{
	mIsLooping = pLoop;
	Open(pFilename);
}

OggAlStream::~OggAlStream()
{
	Release();
}

bool OggAlStream::Playback()
{
	if (IsPlaying())
	{
		return true;
	}

	if (!Stream(mAlBuffers[0]) || !Stream(mAlBuffers[1]))
	{
		return false;
	}

	alSourceQueueBuffers(mAlSource, 2, mAlBuffers);
	alSourcef(mAlSource, AL_GAIN, mVolume * mSoundManager->GetMusicVolume());
	alSourcePlay(mAlSource);
	return true;
}

bool OggAlStream::Rewind()
{
	return (ov_raw_seek(&mOggStream, 0) == 0);
}

bool OggAlStream::IsPlaying() const
{
	ALenum state;
	alGetSourcei(mAlSource, AL_SOURCE_STATE, &state);
	return (state == AL_PLAYING);
}

bool OggAlStream::Stop()
{
	Pause();
	return Rewind();
}

bool OggAlStream::Pause()
{
	alSourceStop(mAlSource);
	return Clear();
}

bool OggAlStream::Update()
{
	bool lIsActive = true;
	int lProcessedBufferCount;
	alGetSourcei(mAlSource, AL_BUFFERS_PROCESSED, &lProcessedBufferCount);
	alSourcef(mAlSource, AL_GAIN, mVolume * mSoundManager->GetMusicVolume());
	AL_CHECK();
	while (lProcessedBufferCount--)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(mAlSource, 1, &buffer);
		AL_CHECK();

		lIsActive = Stream(buffer);

		alSourceQueueBuffers(mAlSource, 1, &buffer);
		AL_CHECK();
	}
	if (!lIsActive)
	{
		if (mIsLooping)
		{
			Clear();
			lIsActive = Rewind();
			TimeoutAutoResume();
		}
		else
		{
			Stop();
		}
	}
	else
	{
		TimeoutAutoResume();
	}
	return lIsActive;
}

bool OggAlStream::Open(const str& pFilename)
{
	Release();

	if ((mOggFile = fopen(pFilename.c_str(), "rb")) == 0)
	{
		return mIsOpen;
	}

	int lResult;
	if ((lResult = ov_open(mOggFile, &mOggStream, NULL, 0)) < 0)	// Ogg/Vorbis takes ownership of file.
	{
		fclose(mOggFile);
		mOggFile = 0;
		return mIsOpen;
	}

	mVorbisInfo = ov_info(&mOggStream, -1);

	mAlFormat = (mVorbisInfo->channels == 1)? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	alGenBuffers(2, mAlBuffers);
	AL_CHECK();
	alGenSources(1, &mAlSource);
	AL_CHECK();

	alSource3f(mAlSource, AL_POSITION,		0.0, 0.0, 0.0);
	alSource3f(mAlSource, AL_VELOCITY,		0.0, 0.0, 0.0);
	alSource3f(mAlSource, AL_DIRECTION,		0.0, 0.0, 0.0);
	alSourcef (mAlSource, AL_ROLLOFF_FACTOR,	0.0);
	alSourcei (mAlSource, AL_SOURCE_RELATIVE,	AL_TRUE);
	mIsOpen = true;
	return mIsOpen;
}

bool OggAlStream::Release()
{
	if (!mIsOpen)
	{
		return false;
	}

	alSourceStop(mAlSource);
	if (!Clear())
	{
		return false;
	}

	alDeleteSources(1, &mAlSource);
	AL_CHECK();
	alDeleteBuffers(1, mAlBuffers);
	AL_CHECK();

	ov_clear(&mOggStream);
	mIsOpen = false;
	return true;
}

bool OggAlStream::Stream(ALuint buffer)
{
	char lData[BUFFER_SIZE];
	int lSize = 0;
	while (lSize < BUFFER_SIZE)
	{
		int lSection;
		int lResult = ov_read(&mOggStream, lData + lSize, BUFFER_SIZE - lSize, 0, 2, 1, & lSection);
		if (lResult > 0)
		{
			lSize += lResult;
		}
		else if (lResult < 0)
		{
			return false;
		}
		else
		{
			break;
		}
	}

	if (lSize == 0)
	{
		return false;
	}

	alBufferData(buffer, mAlFormat, lData, lSize, mVorbisInfo->rate);
	AL_CHECK();
	return true;
}

bool OggAlStream::Clear()
{
	int lQueuedBufferCount = 0;
	alGetSourcei(mAlSource, AL_BUFFERS_QUEUED, &lQueuedBufferCount);
	while (lQueuedBufferCount--)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(mAlSource, 1, &buffer);
		AL_CHECK();
	}
	return true;
}

void OggAlStream::TimeoutAutoResume()
{
	ALenum state;
	alGetSourcei(mAlSource, AL_SOURCE_STATE, &state);
	if (state == AL_STOPPED)
	{
		alSourcePlay(mAlSource);
	}
}



}
