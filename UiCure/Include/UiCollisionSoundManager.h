
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../Lepra/Include/Vector3D.h"
#include "../../Tbc/Include/PhysicsManager.h"
#include "../../UiCure/Include/UiResourceManager.h"
#include "UiCure.h"



namespace Cure
{
class ContextObject;
class GameManager;
}
namespace Tbc
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
	void SetScale(float pSmallMass, float pLightImpact, float pImpactVolumeFactor, float pSoundCutoffDuration);

	void AddSound(const str& pName, const SoundResourceInfo& pInfo);
	void PreLoadSound(const str& pName);

	void Tick(const vec3& pCameraPosition);
	void OnCollision(const vec3& pForce, const vec3& pTorque, const vec3& pPosition,
		Cure::ContextObject* pObject1, Cure::ContextObject* pObject2,
		Tbc::PhysicsManager::BodyID pBody1Id, float pMaxDistance, bool pIsLoud);
	void OnCollision(float pImpact, const vec3& pPosition, const Tbc::ChunkyBoneGeometry* pKey, const str& pSoundName);

private:
	/*struct GeometryCombination
	{
		Tbc::ChunkyBoneGeometry* mGeometry;
		GeometryCombination(Tbc::ChunkyBoneGeometry* pGeometryA);
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
		vec3 mPosition;
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

	SoundInfo* GetPlayingSound(const Tbc::ChunkyBoneGeometry* pGeometryKey) const;
	void PlaySound(const Tbc::ChunkyBoneGeometry* pGeometryKey, const str& pSoundName, const vec3& pPosition, float pImpact);
	void OnSoundLoaded(UiCure::UserSound3dResource* pSoundResource);
	void OnSoundPreLoaded(UiCure::UserSound3dResource* pSoundResource);
	void UpdateSound(SoundInfo* pSoundInfo);
	void StopSound(const Tbc::ChunkyBoneGeometry* pGeometryKey);

	typedef std::unordered_map<const Tbc::ChunkyBoneGeometry*, SoundInfo*, LEPRA_VOIDP_HASHER> SoundMap;
	typedef std::unordered_map<str, SoundResourceInfo> SoundNameMap;

	Cure::GameManager* mGameManager;
	UiCure::GameUiManager* mUiManager;
	vec3 mCameraPosition;
	float mSmallMass;
	float mLightImpact;
	float mImpactVolumeFactor;
	float mSoundCutoffDuration;
	SoundMap mSoundMap;
	SoundNameMap mSoundNameMap;
	mutable Lock mLock;

	logclass();
};



}
