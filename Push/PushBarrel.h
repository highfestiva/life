
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../Lepra/Include/Transformation.h"
#include "Push.h"



namespace Cure
{
class GameManager;
}



namespace Push
{

class PushBarrel
{
public:
	static void GetInfo(const Cure::GameManager* pGameManager, Cure::GameObjectId pAvatarId, TransformationF& pTransform, Vector3DF& pVelocity);
};

}
