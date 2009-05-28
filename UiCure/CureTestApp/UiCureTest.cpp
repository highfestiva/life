
// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#include <assert.h>
#include "../../Lepra/Include/Log.h"
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
	bool Test();

private:
	void ResourceTest::RendererImageLoadCallback(UiCure::UserRendererImageResource* pResource)
	{
		LoadCallback(pResource);
	}
	void ResourceTest::PainterImageLoadCallback(UiCure::UserPainterImageResource* pResource)
	{
		LoadCallback(pResource);
	}
	void ResourceTest::TextureImageLoadCallback(UiCure::UserTextureResource* pResource)
	{
		LoadCallback(pResource);
	}
	void ResourceTest::Sound2dImageLoadCallback(UiCure::UserSound2dResource* pResource)
	{
		LoadCallback(pResource);
	}
	void ResourceTest::LoadCallback(Cure::UserResource* pResource)
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

	LOG_CLASS_DECLARE();
};

bool ResourceTest::Test()
{
	Lepra::String lContext;
	bool lTestOk = true;

	Cure::ResourceManager* lResourceManager = new Cure::ResourceManager(1);
	lResourceManager->InitDefault(0);
	UiCure::GameUiManager lUiManager(Cure::GetSettings());
	lUiManager.Open();
	lUiManager.GetSoundManager()->SetMicrophonePosition(Lepra::Vector3DF(0, 0, 0));

	// Test loading a 2D image.
	if (lTestOk)
	{
		lContext = _T("load 2D image");
		UiCure::UserRendererImageResource lImage(&lUiManager);
		lImage.Load(lResourceManager, _T("normalmap.tga"), UiCure::UserRendererImageResource::TypeLoadCallback(this, &ResourceTest::RendererImageLoadCallback));
		Lepra::Thread::Sleep(0.4);
		lResourceManager->Tick();
		lResourceManager->SafeRelease(&lImage);
		lTestOk = (gResourceLoadCount == 1 && gResourceLoadErrorCount == 0);
		assert(lTestOk);
	}

	// Test loading a 2D image into a texture.
	if (lTestOk)
	{
		lContext = _T("load 2D image into texture");
		// Free previous resource; it's a requirement since the name must be
		// unique throughout all resource types.
		lResourceManager->ForceFreeCache();
		UiCure::UserPainterImageResource lImage(&lUiManager);
		lImage.Load(lResourceManager, _T("normalmap.tga"), UiCure::UserPainterImageResource::TypeLoadCallback(this, &ResourceTest::PainterImageLoadCallback));
		Lepra::Thread::Sleep(0.4);
		lResourceManager->Tick();
		lResourceManager->SafeRelease(&lImage);
		lTestOk = (gResourceLoadCount == 2 && gResourceLoadErrorCount == 0);
		assert(lTestOk);
	}

	// Test loading a 3D texture (own, mipmapped format).
	// We don't have a texture file, so this test should fail.
	if (lTestOk)
	{
		lContext = _T("load 3D texture");
		UiCure::UserTextureResource lTexture(&lUiManager);
		lTexture.Load(lResourceManager, _T("NoSuchFile.tex"), UiCure::UserTextureResource::TypeLoadCallback(this, &ResourceTest::TextureImageLoadCallback));
		Lepra::Thread::Sleep(0.4);
		lResourceManager->Tick();
		lResourceManager->SafeRelease(&lTexture);
		lTestOk = (gResourceLoadCount == 2 && gResourceLoadErrorCount == 1);
		assert(lTestOk);
	}

	// Test loading a 3D texture that we actually do have (at least if we've ever run the UiTbc tests).
	if (lTestOk)
	{
		lContext = _T("load 3D texture");
		UiCure::UserTextureResource lTexture(&lUiManager);
		lTexture.Load(lResourceManager, _T("Normalmap.tex"), UiCure::UserTextureResource::TypeLoadCallback(this, &ResourceTest::TextureImageLoadCallback));
		Lepra::Thread::Sleep(0.4);
		lResourceManager->Tick();
		lResourceManager->SafeRelease(&lTexture);
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
		UiCure::UserSound2dResource lSound(&lUiManager);
		lSound.Load(lResourceManager, _T("Bark.wav"), UiCure::UserSound2dResource::TypeLoadCallback(this, &ResourceTest::Sound2dImageLoadCallback));
		Lepra::Thread::Sleep(0.4);
		lResourceManager->Tick();
		lTestOk = (gResourceLoadCount == 4 && gResourceLoadErrorCount == 1);
		if (lTestOk)
		{
			lUiManager.GetSoundManager()->GetSoundManager()->Play(lSound.GetData(), 1.0, 1.0);
			for (int x = 0; x < 5; ++x)
			{
				lUiManager.GetSoundManager()->GetSoundManager()->Update();
				Lepra::Thread::Sleep(0.1);
			}
		}
		lResourceManager->SafeRelease(&lSound);
		assert(lTestOk);
	}

	// RM should be deleted prior to its managers, so that it has a chance to free it's optimized resources.
	delete (lResourceManager);
	lUiManager.Close();

	ReportTestResult(mLog, _T("Resource"), lContext, lTestOk);
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
	if (lTestOk)
	{
		ResourceTest lResourceTest;
		lTestOk = lResourceTest.Test();
	}
	return (lTestOk);
}
