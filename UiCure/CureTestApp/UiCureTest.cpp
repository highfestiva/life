
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



#include "pch.h"
#ifndef CURE_TEST_WITHOUT_UI
#include "../../Lepra/Include/LepraAssert.h"
#include "../../Cure/Include/ContextManager.h"
#include "../../Cure/Include/GameManager.h"
#include "../../Cure/Include/GameTicker.h"
#include "../../Cure/Include/RuntimeVariable.h"
#include "../../Cure/Include/TimeManager.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Random.h"
#include "../Include/UiCppContextObject.h"
#include "../Include/UiGameUiManager.h"
#include "../Include/UiResourceManager.h"
#include "../Include/UiCure.h"



using namespace Lepra;
static int gResourceLoadCount = 0;
static int gResourceLoadErrorCount = 0;
bool TestCure();
void ReportTestResult(const Lepra::LogDecorator& pLog, const str& pTestName, const str& pContext, bool pbResult);



class ResourceTest
{
public:
	ResourceTest();
	virtual ~ResourceTest();

	bool TestAtom();
	bool TestClass();
	bool TestStress();
	bool TestReloadContextObject();

private:
	class TickRM
	{
	public:
		inline TickRM(Cure::ResourceManager* pRM, double pSleepTime = 0.001)
		{
			const int lTickCount = 2;
			for (int x = 0; x < lTickCount; ++x)
			{
				pRM->Tick();
			}
			Lepra::Thread::Sleep(pSleepTime);
			for (int x = 0; x < lTickCount; ++x)
			{
				pRM->Tick();
			}
		}
	};

	void RendererImageLoadCallback(UiCure::UserRendererImageResource* pResource)
	{
		LoadCallback(pResource);
	}
	void PainterImageLoadCallback(UiCure::UserPainterImageResource* pResource)
	{
		LoadCallback(pResource);
	}
	/*void TextureLoadCallback(UiCure::UserTextureResource* pResource)
	{
		LoadCallback(pResource);
	}*/
	void Sound2dLoadCallback(UiCure::UserSound2dResource* pResource)
	{
		LoadCallback(pResource);
	}
	void ClassLoadCallback(UiCure::UserClassResource* pResource)
	{
		LoadCallback(pResource);

		UiTbc::ChunkyClass* lClass = pResource->GetData();
		mPhysicsResource->LoadUnique(pResource->GetConstResource()->GetManager(),
			lClass->GetPhysicsBaseName()+".phys",
			Cure::UserPhysicsResource::TypeLoadCallback(this,
				&ResourceTest::PhysicsLoadCallback));
		const size_t lMeshCount = lClass->GetMeshCount();
		for (size_t x = 0; x < lMeshCount; ++x)
		{
			int lPhysIndex = -1;
			str lName;
			Lepra::xform lTransform;
			float lMeshScale;
			lClass->GetMesh(x, lPhysIndex, lName, lTransform, lMeshScale);
			mMeshResourceArray.push_back(new UiCure::UserGeometryReferenceResource(
				mUiManager, UiCure::GeometryOffset(lPhysIndex, lTransform, lMeshScale)));
			strutil::strvec v = strutil::Split(lName, ";");
			if (v.size() < 2) { v.push_back("0"); }
			mMeshResourceArray[x]->Load(pResource->GetConstResource()->GetManager(),
				v[0] + ".mesh;0" + v[1],
				UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,
					&ResourceTest::MeshRefLoadCallback));
		}
	}
	void PhysicsLoadCallback(Cure::UserPhysicsResource* pResource)
	{
		LoadCallback(pResource);
	}
	void MeshRefLoadCallback(UiCure::UserGeometryReferenceResource* pResource)
	{
		LoadCallback(pResource);
	}
	void MeshLoadCallback(UiCure::UserUiTypeResource<UiCure::GeometryResource>* pResource)
	{
		LoadCallback(pResource);
	}
	void DumbClassLoadCallback(UiCure::UserClassResource*)
	{
	}
	void LoadCallback(Cure::UserResource* pResource)
	{
		if (pResource->GetConstResource()->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE)
		{
			++gResourceLoadCount;
		}
		else
		{
			++gResourceLoadErrorCount;
		}
	}
	bool InternalLoadTransformation(Cure::ContextManager& pContextManager);

	Cure::ResourceManager* mResourceManager;
	UiCure::GameUiManager* mUiManager;
	Cure::UserPhysicsResource* mPhysicsResource;
	typedef std::vector<UiCure::UserGeometryReferenceResource*> MeshArray;
	MeshArray mMeshResourceArray;

	logclass();
};



class TestGameTicker: public Cure::GameTicker
{
public:
	typedef Cure::GameTicker Parent;

	TestGameTicker():
		Parent(1, 1, 1)
	{
	}

	virtual bool Initialize() {return true;}
	virtual bool Tick() {return true;}
	virtual void PollRoundTrip() {}
	virtual float GetTickTimeReduction() const {return 0;}
	virtual float GetPowerSaveAmount() const {return 0;}
	virtual void WillMicroTick(float pTimeDelta) {(void)pTimeDelta;}
	virtual void DidPhysicsTick() {}
	virtual void OnTrigger(Tbc::PhysicsManager::BodyID, int, int, Tbc::PhysicsManager::BodyID, const vec3&, const vec3&) {}
	virtual void OnForceApplied(int, int,
		Tbc::PhysicsManager::BodyID, Tbc::PhysicsManager::BodyID,
		const vec3&, const vec3&,
		const vec3&, const vec3&) {}
};



class TestGameManager: public Cure::GameManager
{
public:
	typedef Cure::GameManager Parent;

	TestGameManager(const Cure::TimeManager* pTime, Cure::RuntimeVariableScope* pVariableScope, Cure::ResourceManager* pResourceManager):
		Parent(pTime, pVariableScope, pResourceManager)
	{
	}

	virtual Cure::ContextObject* CreateContextObject(const str& pClassId) const {(void)pClassId; return 0;}
	virtual void OnLoadCompleted(Cure::ContextObject* pObject, bool pOk) {(void)pObject; (void)pOk;}
	virtual void OnCollision(const vec3&, const vec3&, const vec3&,
		Cure::ContextObject*, Cure::ContextObject*,
		Tbc::PhysicsManager::BodyID, Tbc::PhysicsManager::BodyID) {}
	virtual bool OnPhysicsSend(Cure::ContextObject* pObject) {(void)pObject; return true;}
	virtual bool OnAttributeSend(Cure::ContextObject* pObject) {(void)pObject; return true;}
	virtual bool IsServer() {return false;}
	virtual void SendAttach(Cure::ContextObject*, unsigned, Cure::ContextObject*, unsigned) {}
	virtual void SendDetach(Cure::ContextObject*, Cure::ContextObject*) {}
	virtual void TickInput() {}
};



ResourceTest::ResourceTest()
{
	mUiManager = new UiCure::GameUiManager(UiCure::GetSettings(), 0);
	mUiManager->Open();
	mResourceManager = new Cure::ResourceManager(1);
	mResourceManager->InitDefault();
	mResourceManager->SetLoadIntermission(0);
	mResourceManager->SetInjectTimeLimit(0.3);

	mPhysicsResource = new Cure::UserPhysicsResource();
}

ResourceTest::~ResourceTest()
{
	delete (mResourceManager);	// Must be first to allow resources suicide.
	delete (mUiManager);
}

bool ResourceTest::TestAtom()
{
	str lContext;
	bool lTestOk = true;

	// Test loading a 2D image.
	if (lTestOk)
	{
		lContext = "load 2D image";
		UiCure::UserRendererImageResource lImage(mUiManager, UiCure::ImageProcessSettings(Canvas::RESIZE_FAST, true));
		lImage.Load(mResourceManager, "blue_eye.png", UiCure::UserRendererImageResource::TypeLoadCallback(this, &ResourceTest::RendererImageLoadCallback));
		Lepra::Thread::Sleep(0.4);
		mResourceManager->Tick();
		mResourceManager->SafeRelease(&lImage);
		lTestOk = (gResourceLoadCount == 1 && gResourceLoadErrorCount == 0);
		deb_assert(lTestOk);
	}

	// Test loading a 2D image into a texture.
	if (lTestOk)
	{
		lContext = "load 2D image into texture";
		// Free previous resource; it's a requirement since the name must be
		// unique throughout all resource types.
		mResourceManager->ForceFreeCache();
		UiCure::UserPainterImageResource lImage(mUiManager);
		lImage.Load(mResourceManager, "blue_eye.png", UiCure::UserPainterImageResource::TypeLoadCallback(this, &ResourceTest::PainterImageLoadCallback));
		Lepra::Thread::Sleep(0.4);
		mResourceManager->Tick();
		mResourceManager->SafeRelease(&lImage);
		lTestOk = (gResourceLoadCount == 2 && gResourceLoadErrorCount == 0);
		deb_assert(lTestOk);
	}

	// Test loading a 3D texture (own, mipmapped format).
	// We don't have a texture file, so this test should fail.
	if (lTestOk)
	{
		lContext = "load 3D texture";
		UiCure::UserRendererImageResource lTexture(mUiManager, UiCure::ImageProcessSettings(Canvas::RESIZE_FAST, false));
		lTexture.Load(mResourceManager, "NoSuchFile.tga", UiCure::UserRendererImageResource::TypeLoadCallback(this, &ResourceTest::RendererImageLoadCallback));
		Lepra::Thread::Sleep(0.4);
		mResourceManager->Tick();
		mResourceManager->SafeRelease(&lTexture);
		lTestOk = (gResourceLoadCount == 2 && gResourceLoadErrorCount == 1);
		deb_assert(lTestOk);
	}

	/*// Test loading a 3D texture that we actually do have (at least if we've ever run the UiTbc tests).
	if (lTestOk)
	{
		lContext = "load 3D texture";
		UiCure::UserRendererImageResource lTexture(mUiManager);
		lTexture.Load(mResourceManager, "normalmap.tex", UiCure::UserRendererImageResource::TypeLoadCallback(this, &ResourceTest::TextureLoadCallback));
		Lepra::Thread::Sleep(0.4);
		mResourceManager->Tick();
		mResourceManager->SafeRelease(&lTexture);
		lTestOk = (gResourceLoadCount == 3 && gResourceLoadErrorCount == 1);
		if (!lTestOk)
		{
			mLog.AError("texture not available, try running UiTbc test first!");
		}
		deb_assert(lTestOk);
	}*/

	// Test loading a sound.
	if (lTestOk)
	{
		lContext = "load sound";
		UiCure::UserSound2dResource lSound(mUiManager, UiLepra::SoundManager::LOOP_NONE);
		lSound.Load(mResourceManager, "logo_trumpet.wav", UiCure::UserSound2dResource::TypeLoadCallback(this, &ResourceTest::Sound2dLoadCallback));
		Lepra::Thread::Sleep(0.4);
		mResourceManager->Tick();
		lTestOk = (gResourceLoadCount == 3 && gResourceLoadErrorCount == 1);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			lContext = "play sound";
			lTestOk = mUiManager->GetSoundManager()->Play(lSound.GetData(), 1.0, 1.0);
			deb_assert(lTestOk);
			Lepra::Thread::Sleep(0.5);
		}
		mResourceManager->SafeRelease(&lSound);
		deb_assert(lTestOk);
	}

	ReportTestResult(mLog, "ResourceAtom", lContext, lTestOk);
	return (lTestOk);
}

bool ResourceTest::TestClass()
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = "load class";
		gResourceLoadCount = 0;
		gResourceLoadErrorCount = 0;
		UiCure::UserClassResource lClass(mUiManager);
		lClass.Load(mResourceManager, "UI:hover_tank_01.class", UiCure::UserClassResource::TypeLoadCallback(this, &ResourceTest::ClassLoadCallback));
		for (int x = 0; gResourceLoadCount != 7 && x < 100; ++x)
		{
			Lepra::Thread::Sleep(0.01);
			mResourceManager->Tick();	// Continue loading of physics and meshes.
		}
		// Make sure nothing more makes it through.
		Lepra::Thread::Sleep(0.1);
		mResourceManager->Tick();
		lTestOk = (gResourceLoadCount == 7 && gResourceLoadErrorCount == 0);
		mResourceManager->SafeRelease(&lClass);
		deb_assert(lTestOk);
	}

	delete (mPhysicsResource);
	mPhysicsResource = 0;
	for (size_t x = 0; x < mMeshResourceArray.size(); ++x)
	{
		delete (mMeshResourceArray[x]);
	}
	mMeshResourceArray.clear();
	mResourceManager->StopClear();
	mResourceManager->InitDefault();

	ReportTestResult(mLog, "ResourceClass", lContext, lTestOk);
	return (lTestOk);
}

bool ResourceTest::TestStress()
{
	str lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = "no loaded resources";
		lTestOk = (mResourceManager->QueryResourceCount() == 0);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "caching 0";
		lTestOk = (mResourceManager->QueryCachedResourceCount() == 0);
		deb_assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = "stress load/free unique";
		for (int x = 0; x <= 0x100; ++x)
		{
			UiCure::UserGeometryReferenceResource* lMesh0 =
				new UiCure::UserGeometryReferenceResource(mUiManager, UiCure::GeometryOffset(0));
			UiCure::UserGeometryReferenceResource* lMesh1 =
				new UiCure::UserGeometryReferenceResource(mUiManager, UiCure::GeometryOffset(0));
			lMesh0->LoadUnique(mResourceManager, "hover_tank_01_skid.mesh;0",
				UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,
					&ResourceTest::MeshRefLoadCallback));
			lMesh1->LoadUnique(mResourceManager, "hover_tank_01_skid.mesh;0",
				UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,
					&ResourceTest::MeshRefLoadCallback));
			delete (lMesh1);
			delete (lMesh0);
			if ((x&7) == 0)
			{
				lTestOk = false;
				size_t lCount = 0;
				for (int i = 0; i < 250 && !lTestOk; ++i)
				{
					TickRM lTick(mResourceManager);
					lCount = mResourceManager->QueryResourceCount();
					lTestOk = (lCount <= 2);
				}
				deb_assert(lTestOk);
			}
		}
		TickRM lTick(mResourceManager);
	}

	if (lTestOk)
	{
		lContext = "caching 1";
		lTestOk = (mResourceManager->QueryCachedResourceCount() == 0);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "clearing cache 1";
		lTestOk = (mResourceManager->ForceFreeCache() == 0);
		deb_assert(lTestOk);
		if (lTestOk)
		{
			Thread::Sleep(0.01);
			lTestOk = (mResourceManager->QueryResourceCount() == 0);
			deb_assert(lTestOk);
		}
	}

	if (lTestOk)
	{
		lContext = "stress load/free mass";
		for (int x = 0; x <= 0x100; ++x)
		{
			typedef UiCure::UserUiTypeResource<UiCure::GeometryResource> UserMesh;
			UserMesh* lMesh0 = new UserMesh(mUiManager);
			UserMesh* lMesh1 = new UserMesh(mUiManager);
			lMesh0->Load(mResourceManager, "hover_tank_01_skid.mesh",
				UserMesh::TypeLoadCallback(this, &ResourceTest::MeshLoadCallback));
			lMesh1->Load(mResourceManager, "hover_tank_01_skid.mesh",
				UserMesh::TypeLoadCallback(this, &ResourceTest::MeshLoadCallback));
			delete (lMesh1);
			delete (lMesh0);
			if ((x&7) == 0)
			{
				lTestOk = false;
				for (int i = 0; i < 250 && !lTestOk; ++i)
				{
					TickRM lTick(mResourceManager);
					const size_t lCount = mResourceManager->QueryResourceCount();
					lTestOk = (lCount <= 1);
				}
				deb_assert(lTestOk);
			}
		}
	}

	if (lTestOk)
	{
		lContext = "caching 2";
		lTestOk = (mResourceManager->QueryCachedResourceCount() == 0);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "clearing cache 2";
		mResourceManager->ForceFreeCache();
		lTestOk = (mResourceManager->QueryResourceCount() == 0);
		deb_assert(lTestOk);
	}

	const int lLoopCount = 30;
	const int lAddCount = 10;
	const int lDecCount = lAddCount/3;

	if (lTestOk)
	{
		lContext = "stressing unique load";
		typedef std::list<UiCure::UserClassResource*> ClassList;
		ClassList lResources;
		for (int x = 0; x < lLoopCount; ++x)
		{
			for (int y = 0; y < lAddCount; ++y)
			{
				UiCure::UserClassResource* lClass = new UiCure::UserClassResource(mUiManager);
				lClass->LoadUnique(mResourceManager, "UI:hover_tank_01.class",
					UiCure::UserClassResource::TypeLoadCallback(this, &ResourceTest::DumbClassLoadCallback));
				lResources.push_back(lClass);
			}
			size_t c = mResourceManager->QueryResourceCount();
			deb_assert(c == (size_t)(x*(lAddCount-lDecCount)+lAddCount));
			for (int z = 0; z < lDecCount; ++z)
			{
				int lDropIndex = Lepra::Random::GetRandomNumber()%lResources.size();
				ClassList::reverse_iterator u = lResources.rbegin();
				for (int v = 0; v < lDropIndex; ++u, ++v)
					;
				UiCure::UserClassResource* lClass = *u;
				lResources.erase((++u).base());
				delete lClass;
			}
			lTestOk = false;
			for (int i = 0; i < 250 && !lTestOk; ++i)
			{
				TickRM lTick(mResourceManager);
				c = mResourceManager->QueryResourceCount();
				lTestOk = (c == (size_t)((x+1)*(lAddCount-lDecCount)));
			}
			deb_assert(lTestOk);
		}
		lTestOk = (mResourceManager->QueryResourceCount() == lLoopCount*(lAddCount-lAddCount/3));
		deb_assert(lTestOk);
		if (lTestOk)
		{
			lContext = "unique load completeness";
			lTestOk = false;
			for (int z = 0; !lTestOk && z < 100; ++z)
			{
				TickRM lTick(mResourceManager, 0.1);

				lTestOk = true;
				ClassList::iterator y = lResources.begin();
				for (int u = 0; lTestOk && y != lResources.end(); ++y, ++u)
				{
					lTestOk = ((*y)->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
					if (!lTestOk && z == 99)
					{
						mLog.Warningf("Failed on the %ith element; load state = %i.", u, (*y)->GetLoadState());
					}
				}
			}
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lContext = "unique load clear";
			ClassList::iterator y = lResources.begin();
			for (; y != lResources.end(); ++y)
			{
				delete (*y);
			}
			lResources.clear();
			lTestOk = (mResourceManager->QueryResourceCount() == 0);
			deb_assert(lTestOk);
		}
	}

	if (lTestOk)
	{
		lContext = "stressing mass load";
		gResourceLoadCount = 0;
		typedef std::list<UiCure::UserGeometryReferenceResource*> MeshList;
		MeshList lResources;
		const int lLoopCount = 100;
		const int lAddCount = 10;
		const int lDecCount = lAddCount/3;
		for (int x = 0; x < lLoopCount; ++x)
		{
			for (int y = 0; y < lAddCount; ++y)
			{
				UiCure::UserGeometryReferenceResource* lMesh =
					new UiCure::UserGeometryReferenceResource(mUiManager, UiCure::GeometryOffset(0));
				lMesh->Load(mResourceManager, "hover_tank_01_tower.mesh;0",
					UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,
						&ResourceTest::MeshRefLoadCallback));
				lResources.push_back(lMesh);
			}
			size_t c = mResourceManager->QueryResourceCount();
			deb_assert(c <= 2);
			for (int z = 0; z < lDecCount; ++z)
			{
				int lDropIndex = Lepra::Random::GetRandomNumber()%lResources.size();
				MeshList::reverse_iterator u = lResources.rbegin();
				for (int v = 0; v < lDropIndex; ++u, ++v)
					;
				delete (*u);
				lResources.erase(--u.base());
			}
			lTestOk = false;
			for (int i = 0; i < 250 && !lTestOk; ++i)
			{
				TickRM lTick(mResourceManager);
				c = mResourceManager->QueryResourceCount();
				lTestOk = (c <= 2);
			}
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lContext = "resource load count";
			TickRM lTick(mResourceManager, 0.1);
			size_t c = mResourceManager->QueryResourceCount();
			lTestOk = (c == 2);
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lContext = "mass load completeness";
			MeshList::iterator y = lResources.begin();
			int u = 0;
			for (; lTestOk && y != lResources.end(); ++y, ++u)
			{
				lTestOk = ((*y)->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
				if (!lTestOk)
				{
					mLog.Warningf("Failed on the %ith element (of %u; load state = %i."),
						u, lResources.size(), (*y)->GetLoadState());
				}
			}
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lContext = "mass ref clear";
			MeshList::iterator y = lResources.begin();
			for (; y != lResources.end(); ++y)
			{
				delete (*y);
			}
			lResources.clear();
			lTestOk = (mResourceManager->QueryResourceCount() == 2);
			deb_assert(lTestOk);
		}
		if (lTestOk)
		{
			lContext = "mass cache clear";
			mResourceManager->ForceFreeCache();
			lTestOk = (mResourceManager->QueryResourceCount() == 0);
			deb_assert(lTestOk);
		}
	}

	ReportTestResult(mLog, "ResourceStress", lContext, lTestOk);
	return (lTestOk);
}

/// This is a pretty complete application test, with game tickers, managers, loading and so forth. Wow!
bool ResourceTest::TestReloadContextObject()
{
	str lContext;
	bool lTestOk = true;

	delete mResourceManager;
	mResourceManager = new Cure::ResourceManager(1);
	mResourceManager->InitDefault();
	TestGameTicker lTicker;
	Cure::TimeManager lTimeManager;
	TestGameManager lGameManager(&lTimeManager, new Cure::RuntimeVariableScope(UiCure::GetSettings()), mResourceManager);
	lGameManager.SetTicker(&lTicker);
	Cure::ContextManager lContextManager(&lGameManager);
	if (lTestOk)
	{
		lContext = "init transformation suxx";
		lTestOk = InternalLoadTransformation(lContextManager);
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = "reload transformation suxx";
		lTestOk = InternalLoadTransformation(lContextManager);
		deb_assert(lTestOk);
	}

	ReportTestResult(mLog, "ReloadTransform", lContext, lTestOk);
	return (lTestOk);
}

bool ResourceTest::InternalLoadTransformation(Cure::ContextManager& pContextManager)
{
	// Generate orientation to set and to test against.
	quat q;
	q.RotateAroundOwnX(PIF/5);
	q.RotateAroundOwnY(PIF/5);
	q.RotateAroundOwnZ(PIF/5);
	quat lFlip;
	lFlip.RotateAroundOwnZ(PIF);
	lFlip.RotateAroundOwnX(PIF/2);
	quat Q = -(q * lFlip);

	UiCure::CppContextObject lObject(mResourceManager, "hover_tank_01", mUiManager);
	pContextManager.AddLocalObject(&lObject);
	xform lTransform(q, vec3(1, 2, 3));
	lObject.SetInitialTransform(lTransform);
	lObject.StartLoading();
	for (int x = 0; x < 10 && !lObject.IsLoaded(); ++x)
	{
		TickRM lTick(mResourceManager, 0.1);
	}
	bool lTestOk = lObject.IsLoaded();
	deb_assert(lTestOk);
	if (lTestOk)
	{
		vec3 p = lObject.GetPosition();
		lTestOk = (p.x == 1 && Math::IsInRange(p.y, 1.0f, 3.0f) && Math::IsInRange(p.z, 2.0f, 4.0f));
		deb_assert(lTestOk);
	}
	if (lTestOk)
	{
		quat r = lObject.GetOrientation();
		lTestOk = (Math::IsEpsEqual(r.a, Q.a, 0.1f) &&
			Math::IsEpsEqual(r.b, Q.b, 0.1f) &&
			Math::IsEpsEqual(r.c, Q.c, 0.1f) &&
			Math::IsEpsEqual(r.d, Q.d, 0.1f));
		deb_assert(lTestOk);
	}
	return lTestOk;
}


loginstance(TEST, ResourceTest);



bool TestUiCure()
{
	bool lTestOk = true;
	if (lTestOk)
	{
		lTestOk = TestCure();
	}
	ResourceTest lResourceTest;
	if (lTestOk)
	{
		lTestOk = lResourceTest.TestAtom();
	}
	if (lTestOk)
	{
		lTestOk = lResourceTest.TestClass();
	}
	if (lTestOk)
	{
		lTestOk = lResourceTest.TestStress();
	}
	if (lTestOk)
	{
		lTestOk = lResourceTest.TestReloadContextObject();
	}
	return (lTestOk);
}



#endif // !CURE_TEST_WITHOUT_UI
