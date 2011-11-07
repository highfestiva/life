
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "Cutie.h"
#include "../Lepra/Include/Math.h"
/*#include "../Cure/Include/ContextManager.h"
#include "../Cure/Include/TimeManager.h"
#include "../TBC/Include/ChunkyBoneGeometry.h"
#include "../UiCure/Include/UiGameUiManager.h"
#include "../UiCure/Include/UiProps.h"
#include "Game.h"*/



namespace GrenadeRun
{



Cutie::Cutie(Cure::ResourceManager* pResourceManager, const str& pClassId, UiCure::GameUiManager* pUiManager):
	Parent(pResourceManager, pClassId, pUiManager),
	mHealth(1)
{
}

Cutie::~Cutie()
{
}



void Cutie::DrainHealth(float pDrain)
{
	mHealth = Math::Clamp(mHealth-pDrain, 0.0f, 1.0f);
}

float Cutie::GetHealth() const
{
	return mHealth;
}

bool Cutie::QueryFlip()
{
	if (!IsUpsideDown())
	{
		return false;
	}

	// Reset vehicle in the direction it was heading.
	const Cure::ObjectPositionalData* lOriginalPositionData;
	UpdateFullPosition(lOriginalPositionData);
	Cure::ObjectPositionalData lPositionData;
	lPositionData.CopyData(lOriginalPositionData);
	lPositionData.Stop();
	TransformationF& lTransform = lPositionData.mPosition.mTransformation;
	lTransform.SetPosition(GetPosition() + Vector3DF(0, 0, 3));
	Vector3DF lEulerAngles;
	GetOrientation().GetEulerAngles(lEulerAngles);
	lTransform.GetOrientation().SetEulerAngles(lEulerAngles.x, 0, 0);
	lTransform.GetOrientation() *= GetPhysics()->GetOriginalBoneTransformation(0).GetOrientation();
	SetFullPosition(lPositionData);
	return true;
}

bool Cutie::IsUpsideDown() const
{
	Vector3DF lUp(0, 0, 1);
	lUp = GetOrientation() * lUp;
	if (lUp.z > 0.4f ||
		GetVelocity().GetLengthSquared() > 0.1f ||
		GetAngularVelocity().GetLengthSquared() > 0.1f)
	{
		// Nope, still standing, or at least moving. Might be drunken style,
		// but at least not on it's head yet.
		return false;
	}
	return true;
}


LOG_CLASS_DEFINE(GAME_CONTEXT_CPP, Cutie);



}
