
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/UiChunkyClass.h"



namespace UiTbc
{



ChunkyClass::ChunkyClass()
{
}

ChunkyClass::~ChunkyClass()
{
}



void ChunkyClass::AddMesh(int pPhysIndex, const Lepra::String& pMeshBaseName, const Lepra::TransformationF& pTransform)
{
	mMeshArray.push_back(PhysMeshEntry(pPhysIndex, pMeshBaseName, pTransform));
}



LOG_CLASS_DEFINE(GAME_CONTEXT, ChunkyClass);



}
