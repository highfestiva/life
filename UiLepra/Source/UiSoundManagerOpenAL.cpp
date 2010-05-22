
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiSoundManagerOpenAL.h"
#include <assert.h>
#include "../../ThirdParty/freealut-1.1.0/include/AL/alut.h"



#ifdef LEPRA_WINDOWS
extern "C" ALC_API void ALC_APIENTRY alc_init(void);	// Not intended for this type of use, but LGPL OpenAL can't load dsound.dll from DllMain.
extern "C" ALC_API void ALC_APIENTRY alc_deinit(void);	// Not intended for this type of use, but LGPL OpenAL can't load dsound.dll from DllMain.
#endif // Windows



namespace UiLepra
{



SoundManagerOpenAL::SoundManagerOpenAL(int pMixRate):
	mDevice(0),
	mContext(0),
	mRollOffFactor(1)
{
#ifdef LEPRA_WINDOWS
	alc_init();
	mDevice = ::alcOpenDevice(0);
#else // !Windows
	mDevice = ::alcOpenDevice("'((direction \"write\")) '((devices '(alsa sdl native null)))");
#endif // Windows / !Windows
	if (!mDevice)
	{
		mLog.AError("Could not open any sound device!");
		return;
	}

	const int lAttributes[3] = { ALC_FREQUENCY, pMixRate, 0 };
	mContext = ::alcCreateContext(mDevice, lAttributes);
	if (!mContext)
	{
		mLog.AError("Could not create sound context!");
		return;
	}
	::alcMakeContextCurrent(mContext);

	::alutInitWithoutContext(0, 0);

	const float lPos[3] = {0, 0, 0};
	const float lVel[3] = {0, 0, 0};
	const float lDirection[3+3] = {0, 1, 0, 0, 0, 1};	// Forward along Y, up along Z.
	::alListenerfv(AL_POSITION, lPos);
	::alListenerfv(AL_VELOCITY, lVel);
	::alListenerfv(AL_ORIENTATION, lDirection);
}

SoundManagerOpenAL::~SoundManagerOpenAL()
{
	StopAll();
	SourceSet::iterator x = mSourceSet.begin();
	for (; x != mSourceSet.end(); ++x)
	{
		delete (*x);
	}
	SampleSet::iterator y = mSampleSet.begin();
	for (; y != mSampleSet.end(); ++y)
	{
		delete (*y);
	}

	::alcMakeContextCurrent(0);
	if (mContext)
	{
		::alcDestroyContext(mContext);
		mContext = 0;
	}
	if (mDevice)
	{
		::alcCloseDevice(mDevice);
		mDevice = 0;
	}
#ifdef LEPRA_WINDOWS
	alc_deinit();
#endif // Windows
}

SoundManager::SoundID SoundManagerOpenAL::LoadSound2D(const str& pFileName, LoopMode pLoopMode, int pPriority)
{
	Sample* lSample = (Sample*)LoadSound3D(pFileName, pLoopMode, pPriority);
	if (lSample)
	{
		lSample->mIsAmbient = true;
	}
	return ((SoundID)lSample);
}

SoundManager::SoundID SoundManagerOpenAL::LoadSound3D(const str& pFileName, LoopMode pLoopMode, int pPriority)
{
	Sample* lSample = new Sample(pLoopMode != LOOP_NONE, pPriority);
	if (lSample->Load(pFileName))
	{
		mSampleSet.insert(lSample);
	}
	else
	{
		assert(false);
		delete (lSample);
		lSample = 0;
	}
	return ((SoundID)lSample);
}

SoundManager::SoundID SoundManagerOpenAL::LoadStream(const str& pFileName, LoopMode pLoopMode, int pPriority)
{
	// TODO: should read a stream, not buffer the thing.
	return (LoadSound2D(pFileName, pLoopMode, pPriority));
}

void SoundManagerOpenAL::Release(SoundID pSoundID)
{
	Sample* lSample = GetSample(pSoundID);
	if (!lSample)
	{
		return;
	}

	mSampleSet.erase(mSampleSet.find((Sample*)pSoundID));
	delete (lSample);
}

double SoundManagerOpenAL::GetStreamTime(SoundID pSoundID)
{
	Sample* lSample = GetSample(pSoundID);
	if (!lSample)
	{
		return (-1);
	}

	ALfloat lSourceTime = 0;
	if (!lSample->mSourceList.empty())
	{
		Source* lSource = *lSample->mSourceList.begin();
		::alGetSourcef(lSource->mSid, AL_SEC_OFFSET, &lSourceTime);
	}
	return (lSourceTime);

}

SoundManager::SoundInstanceID SoundManagerOpenAL::CreateSoundInstance(SoundID pSoundID)
{
	Sample* lSample = GetSample(pSoundID);
	if (!lSample)
	{
		return (INVALID_SOUNDINSTANCEID);
	}

	Source* lSource = new Source;
	if (lSource->SetSample(lSample, mRollOffFactor))
	{
		mSourceSet.insert(lSource);
	}
	else
	{
		assert(false);
		delete (lSource);
		lSource = 0;
	}
	return ((SoundInstanceID)lSource);
}

void SoundManagerOpenAL::DeleteSoundInstance(SoundInstanceID pSoundIID)
{
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return;
	}

	mSourceSet.erase(mSourceSet.find((Source*)pSoundIID));
	delete (lSource);
}

bool SoundManagerOpenAL::Play(SoundInstanceID pSoundIID, float pVolume, float pPitch)
{
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return (false);
	}

	::alSourcef(lSource->mSid, AL_GAIN, pVolume);
	::alSourcef(lSource->mSid, AL_PITCH, pPitch);
	::alSourcePlay(lSource->mSid);
	lSource->mIsPaused = false;
	return (true);
}

void SoundManagerOpenAL::StopAll()
{
	SourceSet::iterator x = mSourceSet.begin();
	for (; x != mSourceSet.end(); ++x)
	{
		::alSourceStop((*x)->mSid);
		(*x)->mIsPaused = true;
	}
}

void SoundManagerOpenAL::Stop(SoundInstanceID pSoundIID)
{
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return;
	}
	::alSourceStop(lSource->mSid);
	lSource->mIsPaused = true;
}

void SoundManagerOpenAL::TogglePause(SoundInstanceID pSoundIID)
{
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return;
	}
	if (lSource->mIsPaused)
	{
		::alSourcePlay(lSource->mSid);
	}
	else
	{
		::alSourcePause(lSource->mSid);
	}
	lSource->mIsPaused = !lSource->mIsPaused;
}

bool SoundManagerOpenAL::IsPlaying(SoundInstanceID pSoundIID)
{
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return (false);
	}
	return (!lSource->mIsPaused);
}

bool SoundManagerOpenAL::IsPaused(SoundInstanceID pSoundIID)
{
	return (!IsPlaying(pSoundIID));
}

void SoundManagerOpenAL::SetPan(SoundInstanceID pSoundIID, float)
{
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return;
	}
	//lSource->SetPan(pPan);
}

void SoundManagerOpenAL::SetVolume(SoundInstanceID pSoundIID, float pVolume)
{
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return;
	}
	::alSourcef(lSource->mSid, AL_GAIN, pVolume);
}

void SoundManagerOpenAL::SetPitch(SoundInstanceID pSoundIID, float pPitch)
{
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return;
	}
	::alSourcef(lSource->mSid, AL_PITCH, pPitch);
}

void SoundManagerOpenAL::SetFrequency(SoundInstanceID, int)
{
}

int SoundManagerOpenAL::GetFrequency(SoundInstanceID)
{
	return (0);
}

void SoundManagerOpenAL::SetDopplerFactor(float pFactor)
{
	::alDopplerFactor(pFactor);
}

void SoundManagerOpenAL::SetRollOffFactor(float pFactor)
{
	mRollOffFactor = pFactor;
	SourceSet::iterator x = mSourceSet.begin();
	for (; x != mSourceSet.end(); ++x)
	{
		::alSourcef((*x)->mSid, AL_ROLLOFF_FACTOR, pFactor);
	}
}

void SoundManagerOpenAL::SetChorus(SoundInstanceID, int, float, float, float, float, float)
{
}

void SoundManagerOpenAL::SetFlanger(SoundInstanceID, int, float, float, float, float, float)
{
}

void SoundManagerOpenAL::SetCompressor(SoundInstanceID, int, float, float, float, float)
{
}

void SoundManagerOpenAL::SetEcho(SoundInstanceID, int, float, float, float)
{
}

void SoundManagerOpenAL::SetParamEQ(SoundInstanceID, int, float, float, float)
{
}



void SoundManagerOpenAL::DoSetSoundPosition(SoundInstanceID pSoundIID, const Vector3DF& pPos, const Vector3DF& pVel)
{
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		assert(false);
		return;
	}

	float lPos[3];
	float lVel[3];
	lPos[0] = pPos.x;
	lPos[1] = pPos.y;
	lPos[2] = pPos.z;
	lVel[0] = pVel.x;
	lVel[1] = pVel.y;
	lVel[2] = pVel.z;
	::alSourcefv(lSource->mSid, AL_POSITION, lPos);
	::alSourcefv(lSource->mSid, AL_VELOCITY, lVel);
}



SoundManagerOpenAL::Sample* SoundManagerOpenAL::GetSample(SoundID pSoundID) const
{
	Sample* lSample = (Sample*)pSoundID;
	SampleSet::const_iterator x = mSampleSet.find(lSample);
	if (x == mSampleSet.end())
	{
		assert(false);
		lSample = 0;
	}
	return (lSample);
}

SoundManagerOpenAL::Source* SoundManagerOpenAL::GetSource(SoundInstanceID pSoundInstanceID) const
{
	Source* lSource = (Source*)pSoundInstanceID;
	SourceSet::const_iterator x = mSourceSet.find(lSource);
	if (x == mSourceSet.end())
	{
		assert(false);
		lSource = 0;
	}
	return (lSource);
}



SoundManagerOpenAL::Sample::Sample(bool pLooping, int pPriority):
	mBuffer(AL_NONE),
	mIsLooping(pLooping),
	mPriority(pPriority),
	mIsAmbient(false)
{
}

SoundManagerOpenAL::Sample::~Sample()
{
	if (mBuffer != AL_NONE)
	{
		::alDeleteBuffers(1, &mBuffer);
		mBuffer = AL_NONE;
	}
}

bool SoundManagerOpenAL::Sample::Load(const str& pFileName)
{
	assert(mBuffer == AL_NONE);
	mBuffer = ::alutCreateBufferFromFile(astrutil::Encode(pFileName).c_str());
	return (mBuffer != AL_NONE);
}



SoundManagerOpenAL::Source::Source():
	mSample(0),
	mSid((ALuint)-1),
	mIsPaused(true)
{
}

SoundManagerOpenAL::Source::~Source()
{
	if (mSample)
	{
		mSample->mSourceList.erase(this);
		mSample = 0;
	}
	if (mSid != (ALuint)-1)
	{
		::alDeleteSources(1, &mSid);
	}
}

bool SoundManagerOpenAL::Source::SetSample(Sample* pSample, float pRollOffFactor)
{
	assert(mSample == 0);
	mSample = pSample;
	mSample->mSourceList.insert(this);

	::alGenSources(1, &mSid);
	::alSourcei(mSid, AL_LOOPING, mSample->mIsLooping? 1 : 0);
	if (mSample->mIsAmbient)
	{
		::alSourcei(mSid, AL_SOURCE_RELATIVE, AL_TRUE);
		::alSourcef(mSid, AL_ROLLOFF_FACTOR, 0);
		::alSource3f(mSid, AL_DIRECTION, 0,0,0);	// Omnidirectional sound source.
	}
	else
	{
		::alSourcei(mSid, AL_SOURCE_RELATIVE, AL_FALSE);
		::alSourcef(mSid, AL_ROLLOFF_FACTOR, pRollOffFactor);
	}
	::alSourcei(mSid, AL_BUFFER, mSample->mBuffer);
	return (mSid != (ALuint)-1);
}



LOG_CLASS_DEFINE(UI_SOUND, SoundManagerOpenAL);



}
