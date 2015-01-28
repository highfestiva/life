
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "pch.h"
#include "../Include/UiSoundManagerMock.h"



namespace UiLepra
{



SoundManagerMock::SoundManagerMock()
{
}

SoundManagerMock::~SoundManagerMock()
{
}

SoundManager::SoundID SoundManagerMock::LoadSound2D(const str&, LoopMode, int )
{
	return (INVALID_SOUNDID);
}

SoundManager::SoundID SoundManagerMock::LoadSound3D(const str&, LoopMode, int)
{
	return (INVALID_SOUNDID);
}

SoundManager::SoundID SoundManagerMock::LoadStream(const str&, LoopMode, int)
{
	return (INVALID_SOUNDID);
}

void SoundManagerMock::Release(SoundID)
{
}

double SoundManagerMock::GetStreamTime(SoundID)
{
	return (0);
}

SoundManager::SoundInstanceID SoundManagerMock::CreateSoundInstance(SoundID)
{
	return (INVALID_SOUNDINSTANCEID);
}

void SoundManagerMock::DeleteSoundInstance(SoundInstanceID)
{
}

bool SoundManagerMock::Play(SoundInstanceID, float, float)
{
	return (false);
}

void SoundManagerMock::StopAll()
{
}

void SoundManagerMock::Stop(SoundInstanceID)
{
}

void SoundManagerMock::TogglePause(SoundInstanceID)
{
}

bool SoundManagerMock::IsPlaying(SoundInstanceID)
{
	return (false);
}

bool SoundManagerMock::IsPaused(SoundInstanceID)
{
	return (false);
}

void SoundManagerMock::SetPan(SoundInstanceID, float)
{
}

void SoundManagerMock::SetVolume(SoundInstanceID, float)
{
}

void SoundManagerMock::SetPitch(SoundInstanceID, float)
{
}

void SoundManagerMock::SetFrequency(SoundInstanceID, int)
{
}

int SoundManagerMock::GetFrequency(SoundInstanceID)
{
	return (0);
}

void SoundManagerMock::SetDopplerFactor(float)
{
}

void SoundManagerMock::SetRollOffFactor(float)
{
}

void SoundManagerMock::SetChorus(SoundInstanceID, int, float, float, float, float, float)
{
}

void SoundManagerMock::SetFlanger(SoundInstanceID, int, float, float, float, float, float)
{
}

void SoundManagerMock::SetCompressor(SoundInstanceID, int, float, float, float, float)
{
}

void SoundManagerMock::SetEcho(SoundInstanceID, int, float, float, float)
{
}

void SoundManagerMock::SetParamEQ(SoundInstanceID, int, float, float, float)
{
}



void SoundManagerMock::DoSetSoundPosition(SoundInstanceID, const vec3&, const vec3&)
{
}



}
