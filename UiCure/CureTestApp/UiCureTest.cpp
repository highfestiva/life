
// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#ifndef CURE_TEST_WITHOUT_UI

#include <assert.h>
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Random.h"
#include "../Include/UiGameUiManager.h"
#include "../Include/UiResourceManager.h"
#include "../Include/UiSoundManager.h"
#include "../Include/UiCure.h"

bool TestCure();



static int gResourceLoadCount = 0;
static int gResourceLoadErrorCount = 0;



void ReportTestResult(const Lepra::LogDecorator& pLog, const Lepra::String& pTestName, const Lepra::String& pContext, bool pbResult);



class ResourceTest
{
public:
	ResourceTest();
	virtual ~ResourceTest();

	bool TestAtom();
	bool TestClass();
	bool TestStress();

private:
	void RendererImageLoadCallback(UiCure::UserRendererImageResource* pResource)
	{
		LoadCallback(pResource);
	}
	void PainterImageLoadCallback(UiCure::UserPainterImageResource* pResource)
	{
		LoadCallback(pResource);
	}
	void TextureLoadCallback(UiCure::UserTextureResource* pResource)
	{
		LoadCallback(pResource);
	}
	void Sound2dLoadCallback(UiCure::UserSound2dResource* pResource)
	{
		LoadCallback(pResource);
	}
	void ClassLoadCallback(UiCure::UserClassResource* pResource)
	{
		LoadCallback(pResource);

		UiTbc::ChunkyClass* lClass = pResource->GetData();
		mPhysicsResource->LoadUnique(pResource->GetConstResource()->GetManager(),
			lClass->GetPhysicsBaseName()+_T(".phys"),
			Cure::UserPhysicsResource::TypeLoadCallback(this,
				&ResourceTest::PhysicsLoadCallback));
		const size_t lMeshCount = lClass->GetMeshCount();
		for (size_t x = 0; x < lMeshCount; ++x)
		{
			int lPhysIndex = -1;
			Lepra::String lName;
			Lepra::TransformationF lTransform;
			lClass->GetMesh(x, lPhysIndex, lName, lTransform);
			mMeshResourceArray.push_back(new UiCure::UserGeometryReferenceResource(
				mUiManager, UiCure::GeometryOffset(lPhysIndex, lTransform)));
			mMeshResourceArray[x]->Load(pResource->GetConstResource()->GetManager(),
				lName+_T(".mesh"),
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

	Cure::ResourceManager* mResourceManager;
	UiCure::GameUiManager* mUiManager;
	Cure::UserPhysicsResource* mPhysicsResource;
	typedef std::vector<UiCure::UserGeometryReferenceResource*> MeshArray;
	MeshArray mMeshResourceArray;

	LOG_CLASS_DECLARE();
};

ResourceTest::ResourceTest()
{
	mUiManager = new UiCure::GameUiManager(UiCure::GetSettings());
	mUiManager->Open();
	mUiManager->GetSoundManager()->SetMicrophonePosition(Lepra::Vector3DF(0, 0, 0));
	mResourceManager = new Cure::ResourceManager(1);
	mResourceManager->InitDefault();

	mPhysicsResource = new Cure::UserPhysicsResource();
}

ResourceTest::~ResourceTest()
{
	delete (mResourceManager);	// Must be first to allow resources suicide.
	delete (mUiManager);
}

bool ResourceTest::TestAtom()
{
	Lepra::String lContext;
	bool lTestOk = true;

	// Test loading a 2D image.
	if (lTestOk)
	{
		lContext = _T("load 2D image");
		UiCure::UserRendererImageResource lImage(mUiManager);
		lImage.Load(mResourceManager, _T("normalmap.tga"), UiCure::UserRendererImageResource::TypeLoadCallback(this, &ResourceTest::RendererImageLoadCallback));
		Lepra::Thread::Sleep(0.4);
		mResourceManager->Tick();
		mResourceManager->SafeRelease(&lImage);
		lTestOk = (gResourceLoadCount == 1 && gResourceLoadErrorCount == 0);
		assert(lTestOk);
	}

	// Test loading a 2D image into a texture.
	if (lTestOk)
	{
		lContext = _T("load 2D image into texture");
		// Free previous resource; it's a requirement since the name must be
		// unique throughout all resource types.
		mResourceManager->ForceFreeCache();
		UiCure::UserPainterImageResource lImage(mUiManager);
		lImage.Load(mResourceManager, _T("normalmap.tga"), UiCure::UserPainterImageResource::TypeLoadCallback(this, &ResourceTest::PainterImageLoadCallback));
		Lepra::Thread::Sleep(0.4);
		mResourceManager->Tick();
		mResourceManager->SafeRelease(&lImage);
		lTestOk = (gResourceLoadCount == 2 && gResourceLoadErrorCount == 0);
		assert(lTestOk);
	}

	// Test loading a 3D texture (own, mipmapped format).
	// We don't have a texture file, so this test should fail.
	if (lTestOk)
	{
		lContext = _T("load 3D texture");
		UiCure::UserTextureResource lTexture(mUiManager);
		lTexture.Load(mResourceManager, _T("NoSuchFile.tex"), UiCure::UserTextureResource::TypeLoadCallback(this, &ResourceTest::TextureLoadCallback));
		Lepra::Thread::Sleep(0.4);
		mResourceManager->Tick();
		mResourceManager->SafeRelease(&lTexture);
		lTestOk = (gResourceLoadCount == 2 && gResourceLoadErrorCount == 1);
		assert(lTestOk);
	}

	// Test loading a 3D texture that we actually do have (at least if we've ever run the UiTbc tests).
	if (lTestOk)
	{
		lContext = _T("load 3D texture");
		UiCure::UserTextureResource lTexture(mUiManager);
		lTexture.Load(mResourceManager, _T("Normalmap.tex"), UiCure::UserTextureResource::TypeLoadCallback(this, &ResourceTest::TextureLoadCallback));
		Lepra::Thread::Sleep(0.4);
		mResourceManager->Tick();
		mResourceManager->SafeRelease(&lTexture);
		lTestOk = (gResourceLoadCount == 3 && gResourceLoadErrorCount == 1);
		if (!lTestOk)
		{
			mLog.AError("texture not available, try running UiTbc test first!");
		}
		assert(lTestOk);
	}

	// Test loading a sound.
	if (lTestOk)
	{
		lContext = _T("load sound");
		UiCure::UserSound2dResource lSound(mUiManager);
		lSound.Load(mResourceManager, _T("Bark.wav"), UiCure::UserSound2dResource::TypeLoadCallback(this, &ResourceTest::Sound2dLoadCallback));
		Lepra::Thread::Sleep(0.4);
		mResourceManager->Tick();
		lTestOk = (gResourceLoadCount == 4 && gResourceLoadErrorCount == 1);
		if (lTestOk)
		{
			mUiManager->GetSoundManager()->GetSoundManager()->Play(lSound.GetData(), 1.0, 1.0);
			for (int x = 0; x < 5; ++x)
			{
				mUiManager->GetSoundManager()->GetSoundManager()->Update();
				Lepra::Thread::Sleep(0.1);
			}
		}
		mResourceManager->SafeRelease(&lSound);
		assert(lTestOk);
	}

	ReportTestResult(mLog, _T("ResourceAtom"), lContext, lTestOk);
	return (lTestOk);
}

bool ResourceTest::TestClass()
{
	Lepra::String lContext;
	bool lTestOk = true;

	if (lTestOk)
	{
		lContext = _T("load class");
		UiCure::UserClassResource lClass(mUiManager);
		lClass.Load(mResourceManager, _T("tractor_01.class"), UiCure::UserClassResource::TypeLoadCallback(this, &ResourceTest::ClassLoadCallback));
		for (int x = 0; gResourceLoadCount != 13 && x < 100; ++x)
		{
			Lepra::Thread::Sleep(0.01);
			mResourceManager->Tick();	// Continue loading of physics and meshes.
		}
		// Make sure nothing more makes it through.
		Lepra::Thread::Sleep(0.1);
		mResourceManager->Tick();
		lTestOk = (gResourceLoadCount == 13 && gResourceLoadErrorCount == 1);
		mResourceManager->SafeRelease(&lClass);
		assert(lTestOk);
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

	ReportTestResult(mLog, _T("ResourceClass"), lContext, lTestOk);
	return (lTestOk);
}

bool ResourceTest::TestStress()
{
	Lepra::String lContext;
	bool lTestOk = true;

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

	if (lTestOk)
	{
		lContext = _T("no loaded resources");
		lTestOk = (mResourceManager->QueryResourceCount() == 0);
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = _T("stress load/free unique");
		for (int x = 0; x <= 0x100; ++x)
		{
			UiCure::UserGeometryReferenceResource* lMesh0 =
				new UiCure::UserGeometryReferenceResource(mUiManager, UiCure::GeometryOffset(0));
			UiCure::UserGeometryReferenceResource* lMesh1 =
				new UiCure::UserGeometryReferenceResource(mUiManager, UiCure::GeometryOffset(0));
			lMesh0->Load(mResourceManager, _T("tractor_01_rear_wheel0.mesh"),
				UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,
					&ResourceTest::MeshRefLoadCallback));
			lMesh1->Load(mResourceManager, _T("tractor_01_rear_wheel0.mesh"),
				UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,
					&ResourceTest::MeshRefLoadCallback));
			delete (lMesh1);
			delete (lMesh0);
			if ((x&7) == 0)
			{
				TickRM lTick(mResourceManager);
				const size_t lCount = mResourceManager->QueryResourceCount();
				lTestOk = (lCount <= 2);
				assert(lTestOk);
			}
		}
	}

	if (lTestOk)
	{
		lContext = _T("caching 1");
		lTestOk = (mResourceManager->QueryCachedResourceCount() == 1);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("clearing cache 1");
		mResourceManager->ForceFreeCache();
		lTestOk = (mResourceManager->QueryResourceCount() == 0);
		assert(lTestOk);
	}

	if (lTestOk)
	{
		lContext = _T("stress load/free mass");
		for (int x = 0; x <= 0x100; ++x)
		{
			typedef UiCure::UserUiTypeResource<UiCure::GeometryResource> UserMesh;
			UserMesh* lMesh0 = new UserMesh(mUiManager);
			UserMesh* lMesh1 = new UserMesh(mUiManager);
			lMesh0->Load(mResourceManager, _T("tractor_01_rear_wheel0.mesh"),
				UserMesh::TypeLoadCallback(this, &ResourceTest::MeshLoadCallback));
			lMesh1->Load(mResourceManager, _T("tractor_01_rear_wheel0.mesh"),
				UserMesh::TypeLoadCallback(this, &ResourceTest::MeshLoadCallback));
			delete (lMesh1);
			delete (lMesh0);
			if ((x&7) == 0)
			{
				TickRM lTick(mResourceManager);
				const size_t lCount = mResourceManager->QueryResourceCount();
				lTestOk = (lCount <= 1);
				assert(lTestOk);
			}
		}
	}

	if (lTestOk)
	{
		lContext = _T("caching 2");
		lTestOk = (mResourceManager->QueryCachedResourceCount() == 1);
		assert(lTestOk);
	}
	if (lTestOk)
	{
		lContext = _T("clearing cache 2");
		mResourceManager->ForceFreeCache();
		lTestOk = (mResourceManager->QueryResourceCount() == 0);
		assert(lTestOk);
	}

	const int lLoopCount = 100;
	const int lAddCount = 10;
	const int lDecCount = lAddCount/3;

	if (lTestOk)
	{
		lContext = _T("stressing unique load");
		typedef std::list<UiCure::UserClassResource*> ClassList;
		ClassList lResources;
		for (int x = 0; x < lLoopCount; ++x)
		{
			for (int y = 0; y < lAddCount; ++y)
			{
				UiCure::UserClassResource* lClass = new UiCure::UserClassResource(mUiManager);
				lClass->LoadUnique(mResourceManager, _T("tractor_01.class"),
					UiCure::UserClassResource::TypeLoadCallback(this, &ResourceTest::DumbClassLoadCallback));
				lResources.push_back(lClass);
			}
			size_t c = mResourceManager->QueryResourceCount();
			assert(c == (size_t)(x*(lAddCount-lDecCount)+lAddCount));
			for (int z = 0; z < lDecCount; ++z)
			{
				int lDropIndex = Lepra::Random::GetRandomNumber()%lAddCount;
				ClassList::reverse_iterator u = lResources.rbegin();
				for (int v = 0; v < lDropIndex; ++u, ++v)
					;
				delete (*u);
				lResources.erase(--u.base());
			}
			TickRM lTick(mResourceManager);
			c = mResourceManager->QueryResourceCount();
			lTestOk = (c == (size_t)((x+1)*(lAddCount-lDecCount)));
			assert(lTestOk);
		}
		lTestOk = (mResourceManager->QueryResourceCount() == lLoopCount*(lAddCount-lAddCount/3));
		assert(lTestOk);
		if (lTestOk)
		{
			lContext = _T("unique load completeness");
			lTestOk = false;
			for (int z = 0; !lTestOk && z < 100; ++z)
			{
				TickRM lTick(mResourceManager, 0.1);

				lTestOk = true;
				ClassList::iterator y = lResources.begin();
				int u = 0;
				for (; lTestOk && y != lResources.end(); ++y, ++u)
				{
					lTestOk = ((*y)->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
					/*if (!lTestOk)
					{
						mLog.Warningf(_T("Failed on the %ith element; load state = %i."),
							u, (*y)->GetLoadState());
					}*/
				}
			}
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lContext = _T("unique load clear");
			ClassList::iterator y = lResources.begin();
			for (; y != lResources.end(); ++y)
			{
				delete (*y);
			}
			lResources.clear();
			lTestOk = (mResourceManager->QueryResourceCount() == 0);
			assert(lTestOk);
		}
	}

	if (lTestOk)
	{
		lContext = _T("stressing mass load");
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
				lMesh->Load(mResourceManager, _T("tractor_01_front_wheel0.mesh"),
					UiCure::UserGeometryReferenceResource::TypeLoadCallback(this,
						&ResourceTest::MeshRefLoadCallback));
				lResources.push_back(lMesh);
			}
			size_t c = mResourceManager->QueryResourceCount();
			assert(c == 1);
			for (int z = 0; z < lDecCount; ++z)
			{
				int lDropIndex = Lepra::Random::GetRandomNumber()%lAddCount;
				MeshList::reverse_iterator u = lResources.rbegin();
				for (int v = 0; v < lDropIndex; ++u, ++v)
					;
				delete (*u);
				lResources.erase(--u.base());
			}
			TickRM lTick(mResourceManager);
			c = mResourceManager->QueryResourceCount();
			lTestOk = (c == 1);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lContext = _T("mass load completeness");
			TickRM lTick(mResourceManager, 0.1);
			MeshList::iterator y = lResources.begin();
			int u = 0;
			for (; lTestOk && y != lResources.end(); ++y, ++u)
			{
				lTestOk = ((*y)->GetLoadState() == Cure::RESOURCE_LOAD_COMPLETE);
				if (!lTestOk)
				{
					mLog.Warningf(_T("Failed on the %ith element; load state = %i."),
						u, (*y)->GetLoadState());
				}
			}
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lContext = _T("mass ref clear");
			MeshList::iterator y = lResources.begin();
			for (; y != lResources.end(); ++y)
			{
				delete (*y);
			}
			lResources.clear();
			lTestOk = (mResourceManager->QueryResourceCount() == 1);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lContext = _T("mass cache clear");
			mResourceManager->ForceFreeCache();
			lTestOk = (mResourceManager->QueryResourceCount() == 0);
			assert(lTestOk);
		}
	}

	ReportTestResult(mLog, _T("ResourceStress"), lContext, lTestOk);
	return (lTestOk);
}

LOG_CLASS_DEFINE(TEST, ResourceTest);



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
	return (lTestOk);
}



#endif // !CURE_TEST_WITHOUT_UI
