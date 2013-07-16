
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/UiSoundManagerOpenAL.h"
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Lepra/Include/ResourceTracker.h"
#include "../../ThirdParty/freealut-1.1.0/include/AL/alut.h"
#include "../Include/UiChibiXmAlStream.h"
#include "../Include/UiOggAlStream.h"



#ifdef LEPRA_WINDOWS
extern "C" ALC_API void ALC_APIENTRY alc_init(void);	// Not intended for this type of use, but LGPL OpenAL can't load dsound.dll from DllMain.
extern "C" ALC_API void ALC_APIENTRY alc_deinit(void);	// Not intended for this type of use, but LGPL OpenAL can't load dsound.dll from DllMain.
#endif // Windows

#ifdef LEPRA_DEBUG
#define OAL_ASSERT()	{ ALenum lAlError = alGetError(); assert(lAlError == AL_NO_ERROR); }
#else // !Debug
#define OAL_ASSERT()
#endif // Debug / !Debug



namespace UiLepra
{



SoundManagerOpenAL::SoundManagerOpenAL(int pMixRate):
	mDevice(0),
	mContext(0),
	mRollOffFactor(1),
	mMasterVolume(1)
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

	OAL_ASSERT();
}

SoundManagerOpenAL::~SoundManagerOpenAL()
{
	OAL_ASSERT();

	SetFileOpener(0);

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

	OAL_ASSERT();

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



void SoundManagerOpenAL::Suspend()
{
	::alcMakeContextCurrent(0);
	::alcSuspendContext(mContext);
}

void SoundManagerOpenAL::Resume()
{
	::alcMakeContextCurrent(mContext);
	::alcProcessContext(mContext);
}



float SoundManagerOpenAL::GetMasterVolume() const
{
	return mMasterVolume;
}

void SoundManagerOpenAL::SetMasterVolume(float pVolume)
{
	mMasterVolume = pVolume;
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

SoundManager::SoundID SoundManagerOpenAL::LoadSound2D(const str& pFileName, const void* pData, size_t pDataSize, LoopMode pLoopMode, int pPriority)
{
	Sample* lSample = (Sample*)LoadSound3D(pFileName, pData, pDataSize, pLoopMode, pPriority);
	if (lSample)
	{
		lSample->mIsAmbient = true;
	}
	return ((SoundID)lSample);
}

SoundManager::SoundID SoundManagerOpenAL::LoadSound3D(const str& pFileName, LoopMode pLoopMode, int pPriority)
{
	return LoadSound3D(pFileName, 0, 0, pLoopMode, pPriority);
}

SoundManager::SoundID SoundManagerOpenAL::LoadSound3D(const str& pFileName, const void* pData, size_t pDataSize, LoopMode pLoopMode, int pPriority)
{
	ScopeLock lLock(&mLock);

	(void)pFileName;

	Sample* lSample = new Sample(pLoopMode != LOOP_NONE, pPriority);
	bool lOk = false;
	for (int x = 0; !lOk && x < 3; ++x)	// TRICKY: retry to avoid anti-virus glitches (for the file version) and sound driver glitches (for the raw data version).
	{
		if (pData)
		{
			lOk = lSample->Load(pData, pDataSize);
		}
		else
		{
			lOk = lSample->Load(pFileName);
		}
		if (!lOk)
		{
			Thread::Sleep(0.05f);
		}
	}
	if (lOk)
	{
		mSampleSet.insert(lSample);
	}
	else
	{
		mLog.Errorf(_T("Could not get load sound %s, thus not possible to create sound."), pFileName.c_str());
		assert(false);
		delete (lSample);
		lSample = 0;
	}
	return ((SoundID)lSample);
}

SoundStream* SoundManagerOpenAL::CreateSoundStream(const str& pFileName, LoopMode pLoopMode, int)
{
	OAL_ASSERT();

	SoundStream* lSoundStream;
	if (strutil::EndsWith(pFileName, _T(".xm")))
	{
		lSoundStream = new ChibiXmAlStream(this, pFileName, pLoopMode == LOOP_FORWARD);
	}
	else
	{
		lSoundStream = new OggAlStream(this, pFileName, pLoopMode == LOOP_FORWARD);
	}
	if (!lSoundStream->IsOpen())
	{
		delete lSoundStream;
		lSoundStream = 0;
	}

	OAL_ASSERT();

	return lSoundStream;;
}

void SoundManagerOpenAL::Release(SoundID pSoundID)
{
	ScopeLock lLock(&mLock);
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
	ScopeLock lLock(&mLock);
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
	OAL_ASSERT();
	return (lSourceTime);

}

SoundManager::SoundInstanceID SoundManagerOpenAL::CreateSoundInstance(SoundID pSoundID)
{
	ScopeLock lLock(&mLock);
	Sample* lSample = GetSample(pSoundID);
	if (!lSample)
	{
		mLog.Errorf(_T("Could not get sound sample %u, thus not possible to create sound instance."), pSoundID);
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
	ScopeLock lLock(&mLock);
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		assert(false);
		return;
	}

	mSourceSet.erase(mSourceSet.find((Source*)pSoundIID));
	delete (lSource);
}

bool SoundManagerOpenAL::Play(SoundInstanceID pSoundIID, float pVolume, float pPitch)
{
	ScopeLock lLock(&mLock);
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return (false);
	}

	::alSourcef(lSource->mSid, AL_GAIN, pVolume * mMasterVolume);
	::alSourcef(lSource->mSid, AL_PITCH, pPitch);
	::alSourcePlay(lSource->mSid);
	OAL_ASSERT();
	return (true);
}

void SoundManagerOpenAL::StopAll()
{
	ScopeLock lLock(&mLock);
	SourceSet::iterator x = mSourceSet.begin();
	for (; x != mSourceSet.end(); ++x)
	{
		::alSourceStop((*x)->mSid);
	}
	OAL_ASSERT();
}

void SoundManagerOpenAL::Stop(SoundInstanceID pSoundIID)
{
	ScopeLock lLock(&mLock);
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return;
	}
	::alSourceStop(lSource->mSid);
	OAL_ASSERT();
}

void SoundManagerOpenAL::TogglePause(SoundInstanceID pSoundIID)
{
	ScopeLock lLock(&mLock);
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return;
	}
	ALint lState = AL_PAUSED;
	alGetSourcei(lSource->mSid, AL_SOURCE_STATE, &lState);
	if (lState != AL_PLAYING)
	{
		::alSourcePlay(lSource->mSid);
	}
	else
	{
		::alSourcePause(lSource->mSid);
	}
	OAL_ASSERT();
}

bool SoundManagerOpenAL::IsPlaying(SoundInstanceID pSoundIID)
{
	ScopeLock lLock(&mLock);
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return (false);
	}
	ALint lState = AL_PAUSED;
	alGetSourcei(lSource->mSid, AL_SOURCE_STATE, &lState);
	OAL_ASSERT();
	return (lState == AL_PLAYING);
}

bool SoundManagerOpenAL::IsPaused(SoundInstanceID pSoundIID)
{
	return (!IsPlaying(pSoundIID));
}

void SoundManagerOpenAL::SetPan(SoundInstanceID pSoundIID, float)
{
	ScopeLock lLock(&mLock);
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return;
	}
	//lSource->SetPan(pPan);
}

void SoundManagerOpenAL::SetVolume(SoundInstanceID pSoundIID, float pVolume)
{
	ScopeLock lLock(&mLock);
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return;
	}
	::alSourcef(lSource->mSid, AL_GAIN, pVolume * mMasterVolume);
	OAL_ASSERT();
}

void SoundManagerOpenAL::SetPitch(SoundInstanceID pSoundIID, float pPitch)
{
	ScopeLock lLock(&mLock);
	Source* lSource = GetSource(pSoundIID);
	if (!lSource)
	{
		return;
	}
	::alSourcef(lSource->mSid, AL_PITCH, pPitch);
	OAL_ASSERT();
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
	OAL_ASSERT();
}

void SoundManagerOpenAL::SetRollOffFactor(float pFactor)
{
	ScopeLock lLock(&mLock);
	mRollOffFactor = pFactor;
	SourceSet::iterator x = mSourceSet.begin();
	for (; x != mSourceSet.end(); ++x)
	{
		::alSourcef((*x)->mSid, AL_ROLLOFF_FACTOR, pFactor);
	}
	OAL_ASSERT();
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



void SoundManagerOpenAL::SetFileOpener(FileOpener* pOpener)
{
	ChibiXmAlStream::SetFileOpener(pOpener);
}



void SoundManagerOpenAL::DoSetSoundPosition(SoundInstanceID pSoundIID, const Vector3DF& pPos, const Vector3DF& pVel)
{
	ScopeLock lLock(&mLock);
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

	OAL_ASSERT();
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
	ScopeLock lLock(&mLock);
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
		LEPRA_RELEASE_RESOURCE(alBuffer);
	}
	OAL_ASSERT();
}

bool SoundManagerOpenAL::Sample::Load(const str& pFileName)
{
	assert(mBuffer == AL_NONE);
	LEPRA_ACQUIRE_RESOURCE(alBuffer);
	mBuffer = ::alutCreateBufferFromFile(astrutil::Encode(pFileName).c_str());
	OAL_ASSERT();
	return (mBuffer != AL_NONE);
}

bool SoundManagerOpenAL::Sample::Load(const void* pData, size_t pDataSize)
{
	assert(mBuffer == AL_NONE);
	LEPRA_ACQUIRE_RESOURCE(alBuffer);
	mBuffer = ::alutCreateBufferFromFileImage(pData, pDataSize);
	OAL_ASSERT();
	return (mBuffer != AL_NONE);
}



SoundManagerOpenAL::Source::Source():
	mSample(0),
	mSid((ALuint)-1)
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
		::alSourceStop(mSid);
		::alDeleteSources(1, &mSid);
		mSid = (ALuint)-1;
		LEPRA_RELEASE_RESOURCE(alSources);
	}
	OAL_ASSERT();
}

bool SoundManagerOpenAL::Source::SetSample(Sample* pSample, float pRollOffFactor)
{
	assert(mSid == (ALuint)-1);
	LEPRA_ACQUIRE_RESOURCE(alSources);
	::alGenSources(1, &mSid);
	if (mSid == (ALuint)-1)
	{
		assert(false);
		mLog.AError("Could not generate OpenAL source, thus not possible to create sound instance.");
		return false;
	}

	assert(mSample == 0);
	mSample = pSample;
	mSample->mSourceList.insert(this);

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
	OAL_ASSERT();
	return true;
}



LOG_CLASS_DEFINE(UI_SOUND, SoundManagerOpenAL);



}
