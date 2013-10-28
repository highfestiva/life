
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/Vector3D.h"
#include "../../TBC/Include/PhysicsManager.h"
#include "../../UiCure/Include/UiResourceManager.h"
#include "UiCure.h"



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



class CollisionSoundManager
{
public:
	struct SoundResourceInfo
	{
		float mStrength;
		float mMinimumClamp;
		float mPitchFactor;
		SoundResourceInfo();
		SoundResourceInfo(float pStrength, float pMinimumClamp, float pPitchFactor);
	//private:
		//void operator=(const SoundResourceInfo&);
	};

	CollisionSoundManager(Cure::GameManager* pGameManager, UiCure::GameUiManager* pUiManager);
	virtual ~CollisionSoundManager();
	void SetScale(float pSmallMass, float pLightImpact, float pSoundCutoffDuration);

	void AddSound(const str& pName, const SoundResourceInfo& pInfo);
	void PreLoadSound(const str& pName);

	void Tick(const Vector3DF& pCameraPosition);
	void OnCollision(const Vector3DF& pForce, const Vector3DF& pTorque, const Vector3DF& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		TBC::PhysicsManager::BodyID pBody1Id, float pMaxDistance, bool pIsLoud);
	void OnCollision(float pImpact, const Vector3DF& pPosition, const TBC::ChunkyBoneGeometry* pKey, const str& pSoundName);

private:
	/*struct GeometryCombination
	{
		TBC::ChunkyBoneGeometry* mGeometry;
		GeometryCombination(TBC::ChunkyBoneGeometry* pGeometryA);
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
			vp.p = __x.mGeometry;
			return vp.s;
		}
	};*/

	struct SoundInfo
	{
		Vector3DF mPosition;
		float mBaseImpact;
		const SoundResourceInfo mResourceInfo;
		float mVolume;
		float mPitch;
		UiCure::UserSound3dResource* mSound;
		SoundInfo(const SoundResourceInfo& pResourceInfo);
		~SoundInfo();
		void UpdateImpact();
		static float GetVolume(float pBaseImpact, const SoundResourceInfo& pResourceInfo);
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

	SoundInfo* GetPlayingSound(const TBC::ChunkyBoneGeometry* pGeometryKey) const;
	void PlaySound(const TBC::ChunkyBoneGeometry* pGeometryKey, const str& pSoundName, const Vector3DF& pPosition, float pImpact);
	void OnSoundLoaded(UiCure::UserSound3dResource* pSoundResource);
	void OnSoundPreLoaded(UiCure::UserSound3dResource* pSoundResource);
	void UpdateSound(SoundInfo* pSoundInfo);
	void StopSound(const TBC::ChunkyBoneGeometry* pGeometryKey);

	typedef std::unordered_map<const TBC::ChunkyBoneGeometry*, SoundInfo*, LEPRA_VOIDP_HASHER> SoundMap;
	typedef std::unordered_map<str, SoundResourceInfo> SoundNameMap;

	Cure::GameManager* mGameManager;
	UiCure::GameUiManager* mUiManager;
	Vector3DF mCameraPosition;
	float mSmallMass;
	float mLightImpact;
	float mSoundCutoffDuration;
	SoundMap mSoundMap;
	SoundNameMap mSoundNameMap;
	mutable Lock mLock;

	LOG_CLASS_DECLARE();
};



}
