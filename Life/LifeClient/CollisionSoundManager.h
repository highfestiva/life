
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../Lepra/Include/Vector3D.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../../UiCure/Include/UiResourceManager.h"
#include "../Life.h"



namespace Cure
{
class ContextObject;
class GameManager;
}
namespace TBC
{
class ChunkyBoneGeometry;
}
namespace UiCure
{
class GameUiManager;
}



namespace Life
{



class CollisionSoundManager
{
public:
	CollisionSoundManager(Cure::GameManager* pGameManager, UiCure::GameUiManager* pUiManager);
	virtual ~CollisionSoundManager();

	void Tick();
	void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		TBC::PhysicsManager::BodyID pBody1Id, TBC::PhysicsManager::BodyID pBody2Id);

private:
	struct GeometryCombination
	{
		TBC::ChunkyBoneGeometry* mGeometry1;
		TBC::ChunkyBoneGeometry* mGeometry2;
		GeometryCombination(TBC::ChunkyBoneGeometry* pGeometryA, TBC::ChunkyBoneGeometry* pGeometryB);
		bool operator==(const GeometryCombination& pOther) const;
	};

	struct GeometryKeyHasher
	{
		size_t operator()(const GeometryCombination& __x) const
		{
			union __vp
			{
				size_t s;
				const void* p;
			};
			__vp vp;
			vp.p = __x.mGeometry2;
			vp.s <<= 1;
			vp.s = (uint8*)vp.p - (uint8*)__x.mGeometry1;
			return vp.s;
		}
	};

	struct SoundInfo
	{
		const GeometryCombination mKey;
		Vector3DF mPosition;
		float mImpact;
		float mVolume;
		float mPitch;
		UiCure::UserSound3dResource* mSound;
		SoundInfo(const GeometryCombination& pKey);
		~SoundInfo();
		void UpdateParams();
	private:
		void operator=(const SoundInfo&);
	};

	struct CollisionSoundResource: public UiCure::UserSound3dResource
	{
		typedef UiCure::UserSound3dResource Parent;
		SoundInfo* mSoundInfo;
		CollisionSoundResource(UiCure::GameUiManager* pUiManager, SoundInfo* pSoundInfo);
	private:
		void operator=(const CollisionSoundResource&);
	};

	SoundInfo* GetPlayingSound(const GeometryCombination& pGeometryKey) const;
	void PlaySound(const GeometryCombination& pGeometryKey, const Vector3DF& pPosition, float pImpact);
	void OnSoundLoaded(UiCure::UserSound3dResource* pSoundResource);
	void UpdateSound(SoundInfo* pSoundInfo);

	typedef std::hash_map<GeometryCombination, SoundInfo*, GeometryKeyHasher> SoundMap;
	typedef std::hash_map<GeometryCombination, str, GeometryKeyHasher> SoundNameMap;

	Cure::GameManager* mGameManager;
	UiCure::GameUiManager* mUiManager;
	SoundMap mSoundMap;
	SoundNameMap mSoundNameMap;

	LOG_CLASS_DECLARE();
};



}
