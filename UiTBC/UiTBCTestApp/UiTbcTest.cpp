
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#ifndef CURE_TEST_WITHOUT_UI

#include <assert.h>
#include "../../Lepra/Include/ArchiveFile.h"
#include "../../Lepra/Include/CubicDeCasteljauSpline.h"
#include "../../Lepra/Include/HiResTimer.h"
#include "../../Lepra/Include/IOError.h"
#include "../../Lepra/Include/Log.h"
#include "../../Lepra/Include/Math.h"
#include "../../Lepra/Include/Number.h"
#include "../../Lepra/Include/Performance.h"
#include "../../Lepra/Include/Random.h"
#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/SystemManager.h"
#include "../../Lepra/Include/PngLoader.h"
#include "../../Lepra/Include/Thread.h"
#include "../../Lepra/Include/Timer.h"
#include "../../Lepra/Include/ZipArchive.h"
#include "../../TBC/Include/ChunkyPhysics.h"
#include "../../TBC/Include/GeometryReference.h"
#include "../../TBC/Include/TerrainFunction.h"
#include "../../TBC/Include/TerrainPatch.h"
#include "../../UiLepra/Include/UiCore.h"
#include "../../UiLepra/Include/UiDisplayManager.h"
#include "../../UiLepra/Include/UiInput.h"
#include "../Include/GUI/UiDesktopWindow.h"
#include "../Include/GUI/UiGUIImageManager.h"
#include "../Include/GUI/UiButton.h"
#include "../Include/GUI/UiCaption.h"
#include "../Include/GUI/UiRadioButton.h"
#include "../Include/GUI/UiCheckButton.h"
#include "../Include/GUI/UiListControl.h"
#include "../Include/GUI/UiFileNameField.h"
#include "../Include/GUI/UiProgressBar.h"
#include "../Include/GUI/UiScrollBar.h"
#include "../Include/GUI/UiWindow.h"
#include "../Include/GUI/UiFloatingLayout.h"
#include "../Include/GUI/UiGridLayout.h"
#include "../Include/GUI/UiTreeNode.h"
#include "../Include/UiBasicMeshCreator.h"
#include "../Include/UiChunkyClass.h"
#include "../Include/UiChunkyLoader.h"
#include "../Include/UiFontManager.h"
#include "../Include/UiGeometryBatch.h"
#include "../Include/UiGraphicalModel.h"
#include "../Include/UiOpenGLPainter.h"
#include "../Include/UiOpenGLRenderer.h"
#include "../Include/UiTEXLoader.h"
#include "../Include/UiTriangleBasedGeometry.h"
#include "../Include/UiUVMapper.h"
#include "RotationalAgreementTest.h"



using namespace Lepra;
void ReportTestResult(const Lepra::LogDecorator& pLog, const str& pTestName, const str& pContext, bool pResult);
bool ResetAndClearFrame();
bool CloseRenderer();



//#define TEST_D3D_RENDERER		// Uncomment to drop software renderer test.
#define OBJECT_DISTANCE		30

#define TEXTUREMAP 0
#define LIGHTMAP 1
#define ENVMAP 2
#define SPECMAP 3
#define NORMALMAP 4
#define MULTIMAP 5


UiLepra::DisplayManager* gDisplay = 0;
Lepra::Canvas* gScreen = 0;
UiTbc::Renderer* gRenderer = 0;
UiTbc::Painter* gPainter = 0;
UiTbc::FontManager* gFontManager = 0;

UiLepra::InputManager* gInput = 0;
int gTextureMapCount = 0;
int gTextureIndex = 0;
UiTbc::Renderer::TextureID gTextureId[10];
UiTbc::Painter::ImageID gImageId[10];
int gX = 0;
int gY = 0;
double gTotalFps = 0;


class UiTbcTest
{
};
Lepra::LogDecorator gUiTbcLog(Lepra::LogType::GetLog(Lepra::LogType::SUB_TEST), typeid(UiTbcTest));

class GUITestWindow: public UiTbc::Window
{
public:
	GUITestWindow() :
		Window(UiTbc::Window::BORDER_LINEARSHADING | UiTbc::Window::BORDER_RESIZABLE, 
			10, Lepra::LIGHT_GREEN, _T("TestWindow1"), new UiTbc::GridLayout(5, 1))
	{
		SetPreferredSize(200, 300);

		UiTbc::Caption* lCaption = new UiTbc::Caption(Lepra::BLUE, Lepra::DARK_BLUE, Lepra::BLUE, Lepra::DARK_BLUE,
			Lepra::LIGHT_GRAY, Lepra::GRAY, Lepra::LIGHT_GRAY, Lepra::GRAY, 20);
		lCaption->SetText(_T("My Test Window"), Lepra::WHITE, Lepra::BLACK, Lepra::OFF_BLACK, Lepra::BLACK);
		SetCaption(lCaption);

		UiTbc::Button* lCloseButton = new UiTbc::Button(UiTbc::BorderComponent::ZIGZAG, 2, Lepra::RED, _T("Close Button"));
		lCloseButton->SetText(_T("y"), Lepra::WHITE, Lepra::BLACK);
		lCloseButton->SetPreferredSize(16, 16);
		lCloseButton->SetMinSize(16, 16);

		lCloseButton->SetOnClick(GUITestWindow, OnClose);

		lCaption->SetRightButton(lCloseButton);

/*
		UiTbc::ListControl* lListControl = new UiTbc::ListControl(UiTbc::ListControl::BORDER_LINEARSHADING | UiTbc::ListControl::BORDER_SUNKEN, 3, Lepra::LIGHT_GRAY);
		lListControl->SetStyle(UiTbc::ListControl::MULTI_SELECT);
		UiTbc::TreeNode::UseFont(Lepra::BLACK, Lepra::LIGHT_GRAY, Lepra::BLACK, Lepra::LIGHT_BLUE);
		UiTbc::TreeNode* lTopNode = new UiTbc::TreeNode(_T("TopNode"), _T("TopNode"));

		int i;
		for (i = 0; i < 10; i++)
		{
			UiTbc::TreeNode* lTreeNode;

			if (i == 5)
			{
				lTreeNode = new UiTbc::TreeNode(Lepra::strutil::Format(_T("SubDirectory"), i), _T("TreeNode"));

				for (int j = 0; j < 5; j++)
				{
					UiTbc::TreeNode* lChildNode = new UiTbc::TreeNode(Lepra::strutil::Format(_T("List Item %i"), i), _T("TreeNode"));
					lTreeNode->AddChildNode(lChildNode);
				}
			}
			else
			{
				lTreeNode = new UiTbc::TreeNode(Lepra::strutil::Format(_T("List Item %i"), Lepra::Random::GetRandomNumber() % 1024), _T("TreeNode"));
			}

			lTopNode->AddChildNode(lTreeNode);
			//lListControl->AddChild(lTreeNode);
		}

		lListControl->AddChild(lTopNode);

		AddChild(lListControl);
*/
		UiTbc::FileNameField* lTextField = new UiTbc::FileNameField(GetClientRectComponent(), UiTbc::Window::BORDER_SUNKEN | UiTbc::Window::BORDER_LINEARSHADING,
			3, Lepra::WHITE, _T("TextField"));
		lTextField->SetPreferredSize(0, 24);
		lTextField->SetFontColor(Lepra::OFF_BLACK);
		lTextField->SetText(_T("Hullo!"));
		AddChild(lTextField);

		UiTbc::Label* lLabel = new UiTbc::Label(Lepra::LIGHT_GRAY, Lepra::RED);
		lLabel->SetPreferredSize(0, 24, false);
		lLabel->SetText(_T("A Row List:"), Lepra::OFF_BLACK, Lepra::BLACK);
		AddChild(lLabel);

		UiTbc::ListControl* lListControl = new UiTbc::ListControl(UiTbc::ListControl::BORDER_LINEARSHADING | UiTbc::ListControl::BORDER_SUNKEN, 3, Lepra::LIGHT_GRAY, UiTbc::ListLayout::ROW);
		lListControl->SetPreferredHeight(30);
		for (int i = 0; i < 20; i++)
		{
			UiTbc::Label* lListItem = new UiTbc::Label(Lepra::LIGHT_GRAY, Lepra::LIGHT_BLUE);
			lListItem->SetText(Lepra::strutil::Format(_T("Apa %i"), i), Lepra::OFF_BLACK, Lepra::BLACK);
			lListItem->SetPreferredWidth(12 * 6);
			lListControl->AddChild(lListItem);
		}
		AddChild(lListControl);

		UiTbc::Button* lButton = new UiTbc::Button(UiTbc::BorderComponent::ZIGZAG, 3, Lepra::GRAY, _T("NewWindow"));
		lButton->SetText(_T("New Window"), Lepra::OFF_BLACK, Lepra::BLACK);
		lButton->SetPreferredSize(0, 40);
		lButton->SetMinSize(20, 20);
		lButton->SetOnClick(GUITestWindow, OnNewWindow);
		AddChild(lButton);
	}

	void OnClose(UiTbc::Button*)
	{
		UiTbc::DesktopWindow* lDesktopWindow = (UiTbc::DesktopWindow*)GetParentOfType(UiTbc::Component::DESKTOPWINDOW);
		lDesktopWindow->PostDeleteComponent(this, 0);
	}

	void OnNewWindow(UiTbc::Button*)
	{
		UiTbc::DesktopWindow* lDesktopWindow = (UiTbc::DesktopWindow*)GetParentOfType(UiTbc::Component::DESKTOPWINDOW);
		lDesktopWindow->AddChild(new GUITestWindow);
	}
};


class SceneTest
{
public:
	SceneTest(const Lepra::LogDecorator& pLog,
		const str& pSceneName, 
		float pCamRotSpeed = 1.0f, float pCamMoveSpeed = 8.0f): 
		mTestOk(true),
		mLog(pLog),
		mSceneName(pSceneName),
		mCamRotSpeed(pCamRotSpeed), 
		mCamMoveSpeed(pCamMoveSpeed) 
	{
		ResetAndClearFrame();

		mDesktopWindow = new UiTbc::DesktopWindow(gInput, gPainter, new UiTbc::FloatingLayout(), 0, 0);
		mDesktopWindow->SetPreferredSize(gScreen->GetWidth(), gScreen->GetHeight());
		mDesktopWindow->AddChild(new GUITestWindow);
		//mDesktopWindow->AddChild(new UiTbc::ASEFileConverter(mDesktopWindow));
	}
	virtual ~SceneTest() 
	{
		delete mDesktopWindow;
		gRenderer->RemoveAllGeometry();
		gRenderer->RemoveAllLights();
	}
	bool Run(double pTime);
	virtual void UpdateScene(double pTotalTime, double pDeltaTime) = 0;

protected:
	static void RenderGDITestImage(const Lepra::tchar* pText);
	static void PrintFps(double pFps);

	bool mTestOk;
	const Lepra::LogDecorator& mLog;
	str mContext;
	str mExtraInfo;

private:
	void operator=(const SceneTest&)
	{
		assert(false);
	}
	str mSceneName;
	float mCamRotSpeed;
	float mCamMoveSpeed;
	UiTbc::DesktopWindow* mDesktopWindow;
};

bool SceneTest::Run(double pTime)
{
	gInput->ActivateAll();

	if (mTestOk)
	{
		Lepra::Timer lTotalTimer;
		Lepra::HiResTimer lDeltaTimer;
		Lepra::Timer lFPSTimer;

		mContext = _T("clear");

		if (mTestOk == true)
		{
			mContext = _T("running");
		}

		while (mTestOk && lTotalTimer.GetTimeDiff() < pTime && gInput->ReadKey(UiLepra::InputManager::IN_KBD_ESC) == false)
		{
			double lDeltaTime = lDeltaTimer.GetTimeDiff();
			lDeltaTimer.ClearTimeDiff();

			UiLepra::Core::ProcessMessages();
			if (Lepra::SystemManager::GetQuitRequest())
			{
				CloseRenderer();
				Lepra::SystemManager::ExitProcess(0);
			}

			gInput->PollEvents();
			gScreen->SetBuffer(0);

			Lepra::TransformationF lCam = gRenderer->GetCameraTransformation();
			if (gInput->ReadKey(UiLepra::InputManager::IN_KBD_UP))
				lCam.RotatePitch(mCamRotSpeed * (float)lDeltaTime);
			if (gInput->ReadKey(UiLepra::InputManager::IN_KBD_DOWN))
				lCam.RotatePitch(-mCamRotSpeed * (float)lDeltaTime);
			if (gInput->ReadKey(UiLepra::InputManager::IN_KBD_LEFT))
				lCam.RotateYaw(mCamRotSpeed * (float)lDeltaTime);
			if (gInput->ReadKey(UiLepra::InputManager::IN_KBD_RIGHT))
				lCam.RotateYaw(-mCamRotSpeed * (float)lDeltaTime);
			if (gInput->ReadKey(UiLepra::InputManager::IN_KBD_Q))
				lCam.RotateRoll(-mCamRotSpeed * (float)lDeltaTime);
			if (gInput->ReadKey(UiLepra::InputManager::IN_KBD_E))
				lCam.RotateRoll(mCamRotSpeed * (float)lDeltaTime);

			if (gInput->ReadKey(UiLepra::InputManager::IN_KBD_W))
				lCam.MoveForward(mCamMoveSpeed * (float)lDeltaTime);
			if (gInput->ReadKey(UiLepra::InputManager::IN_KBD_S))
				lCam.MoveBackward(mCamMoveSpeed * (float)lDeltaTime);
			if (gInput->ReadKey(UiLepra::InputManager::IN_KBD_A))
				lCam.MoveLeft(mCamMoveSpeed * (float)lDeltaTime);
			if (gInput->ReadKey(UiLepra::InputManager::IN_KBD_D))
				lCam.MoveRight(mCamMoveSpeed * (float)lDeltaTime);
			if (gInput->ReadKey(UiLepra::InputManager::IN_KBD_R))
				lCam.MoveUp(mCamMoveSpeed * (float)lDeltaTime);
			if (gInput->ReadKey(UiLepra::InputManager::IN_KBD_F))
				lCam.MoveDown(mCamMoveSpeed * (float)lDeltaTime);

			gRenderer->SetCameraTransformation(lCam);

			{
				LEPRA_MEASURE_SCOPE(RenderScene);
				//gRenderer->ResetClippingRect();
				gRenderer->RenderScene();
				gPainter->ResetClippingRect();
				gPainter->PrePaint(true);

				gPainter->SetColor(Lepra::WHITE);
				const Lepra::Vector3DF& lPos = lCam.GetPosition();
				gPainter->PrintText(Lepra::strutil::Format(_T("(%.2f, %.2f, %.2f)"), lPos.x, lPos.y, lPos.z)+mExtraInfo, 10, 10);

				mDesktopWindow->Repaint();
			}

			UpdateScene(lTotalTimer.GetTimeDiff(), lDeltaTime);

			{
				LEPRA_MEASURE_SCOPE(UpdateScreen);
				gDisplay->UpdateScreen();
			}

			{
				LEPRA_MEASURE_SCOPE(ClearScreen);
				gRenderer->Clear(UiTbc::Renderer::CLEAR_COLORBUFFER |
						  UiTbc::Renderer::CLEAR_DEPTHBUFFER |
						  UiTbc::Renderer::CLEAR_STENCILBUFFER);
			}

			//RenderGDITestImage();
			if (lDeltaTime != 0)
			{
				PrintFps(1/lDeltaTime);
			}

			lTotalTimer.UpdateTimer();
			lDeltaTimer.UpdateTimer();
			Lepra::Thread::YieldCpu();
		}
	}

	gInput->ClearFunctors();

	ReportTestResult(mLog, mSceneName, mContext, mTestOk);
	return (mTestOk);
}

void SceneTest::RenderGDITestImage(const Lepra::tchar* pText = 0)
{
	LEPRA_MEASURE_SCOPE(TextOutput);

	static int x = 0;
	static int dir = 1;

	gFontManager->SetColor(Lepra::GREEN);
	if (!pText)
	{
		pText = _T("This text is printed using GDI!");
	}
	gPainter->PrintText(pText, x, 20);

	if (x >= 100 || x < 0)
	{
		dir = -dir;
	}

	x += dir;
}

void SceneTest::PrintFps(double pFps)
{
	static int lFrameCount = 0;
	static double lFps = 0;
	if (++lFrameCount > lFps/2)
	{
		lFrameCount = 0;
		lFps = pFps;
	}
	str lText = Lepra::strutil::Format(_T("%f FPS"), lFps);
	RenderGDITestImage(lText.c_str());
}



class BumpMapSceneTest: public SceneTest
{
public:
	// All funtions are defined where TestScene() used to be.
	BumpMapSceneTest(const Lepra::LogDecorator& pLog);
	virtual ~BumpMapSceneTest();

	void UpdateScene(double pTotalTime, double pDeltaTime);
private:

	void InitTerrain();

	TBC::TerrainPatch* mTPatch[17];
	UiTbc::TriangleBasedGeometry* mSphere;
	UiTbc::TriangleBasedGeometry* mTorus;
	UiTbc::TriangleBasedGeometry* mCone;

	TBC::BoneHierarchy* mTransformBones;
	TBC::BoneAnimation* mAnimation;
	TBC::BoneAnimator* mAnimator;
	UiTbc::DefaultStaticGeometryHandler* mTorusHandler;
	UiTbc::GraphicalModel mModel;

	Lepra::Vector3DF mLightPos;
	Lepra::Vector3DF mLightDir;
	Lepra::Vector3DF mLightColor;

	UiTbc::Renderer::LightID mLightID;
};

class TerrainFunctionTest: public SceneTest
{
public:
	TerrainFunctionTest(const Lepra::LogDecorator& pLog);
	virtual ~TerrainFunctionTest();

	void UpdateScene(double pTotalTime, double pDeltaTime);

private:
	str mTriangleCountInfo;
	LOG_CLASS_DECLARE();
};
LOG_CLASS_DEFINE(TEST, TerrainFunctionTest);



class GeometryReferenceTest: public SceneTest
{
public:
	GeometryReferenceTest(const Lepra::LogDecorator& pLog);
	virtual ~GeometryReferenceTest();

	void UpdateScene(double pTotalTime, double pDeltaTime);

private:
	UiTbc::TriangleBasedGeometry* mSphere;
	TBC::GeometryReference* mSphereReference;

	LOG_CLASS_DECLARE();
};
LOG_CLASS_DEFINE(TEST, GeometryReferenceTest);



bool CloseRenderer()
{
	delete (gInput);
	gInput = 0;
	delete (gRenderer);
	gRenderer = 0;
	delete (gPainter);
	gPainter = 0;
	delete (gFontManager);
	gFontManager = 0;
	delete (gScreen);
	gScreen = 0;
	delete (gDisplay);
	gDisplay = 0;
	return (true);
}

bool OpenRenderer(const Lepra::LogDecorator& pLog, UiLepra::DisplayManager::ContextType pContext)
{
	bool lOk = true;
	str lContext;

	if (lOk)
	{
		lContext = _T("create display manager");
		Lepra::SystemManager::AddQuitRequest(-1);
		gDisplay = UiLepra::DisplayManager::CreateDisplayManager(pContext);
		lOk = (gDisplay != 0);
	}
	UiLepra::DisplayMode lDisplayMode;
	if (lOk)
	{
		lContext = _T("find display mode");
		lOk = gDisplay->FindDisplayMode(lDisplayMode, 640, 480, 32);
	}
	if (lOk)
	{
		lContext = _T("open screen");
		//lOk = gDisplay->OpenScreen(lDisplayMode, UiLepra::DisplayManager::FULLSCREEN, UiLepra::DisplayManager::ORIENTATION_ALLOW_ANY);
		lOk = gDisplay->OpenScreen(lDisplayMode, UiLepra::DisplayManager::WINDOWED, UiLepra::DisplayManager::ORIENTATION_ALLOW_ANY);
	}
	if (lOk)
	{
		lContext = _T("open input");
		gInput = UiLepra::InputManager::CreateInputManager(gDisplay);
	}

	gScreen = 0;
	gRenderer = 0;
	gPainter = 0;
	gFontManager = 0;
	if (lOk)
	{
		lContext = _T("create renderer");
		gScreen = new Lepra::Canvas(gDisplay->GetWidth(), gDisplay->GetHeight(), Lepra::Canvas::IntToBitDepth(gDisplay->GetBitDepth()));
		gScreen->SetBuffer(0);
		if (pContext == UiLepra::DisplayManager::OPENGL_CONTEXT)
		{
			gRenderer = new UiTbc::OpenGLRenderer(gScreen);
			gPainter = new UiTbc::OpenGLPainter;
		}
		else
		{
			assert(false);
		}
		gRenderer->SetMipMappingEnabled(true);

		gFontManager = UiTbc::FontManager::Create(gDisplay);
		UiTbc::FontManager::FontId lFontId = gFontManager->AddFont(_T("Arial"), 16, 0, UiTbc::FontManager::NATIVE);
		gFontManager->SetActiveFont(lFontId);
		gPainter->SetFontManager(gFontManager);

		gPainter->SetDestCanvas(gScreen);
		gPainter->SetRenderMode(UiTbc::Painter::RM_ALPHABLEND);
	}

	if (lOk)
	{
		lContext = _T("disable vsync");
		lOk = gDisplay->SetVSyncEnabled(false);
	}

	UiTbc::Texture lTextureMap;
	if (lOk)
	{
		lContext = _T("load texturemap (TEX)");
		UiTbc::TEXLoader lTEXLoader;
		lOk = (lTEXLoader.Load(_T("texturemap.tex"), lTextureMap, true) == UiTbc::TEXLoader::STATUS_SUCCESS);
		if (!lOk)
		{
			lContext = _T("load texturemap");
			Lepra::Canvas lCanvas;
			Lepra::PngLoader lLoader;
			lOk = (lLoader.Load(_T("Data/nuclear.png"), lCanvas) == Lepra::PngLoader::STATUS_SUCCESS);

			lTextureMap.Set(lCanvas);
			if (lOk)
			{
				UiTbc::TEXLoader lTEXLoader;
				lTEXLoader.Save(_T("texturemap.tex"), lTextureMap, false);
			}
			assert(lOk);
		}
	}

	UiTbc::Texture lLightMap;
	if (lOk)
	{
		lContext = _T("load lightmap (TEX)");
		UiTbc::TEXLoader lTEXLoader;
		lOk = (lTEXLoader.Load(_T("lightmap.tex"), lLightMap, false) == UiTbc::TEXLoader::STATUS_SUCCESS);
		if (!lOk)
		{
			lContext = _T("load lightmap");
			Lepra::Canvas lCanvas;
			Lepra::PngLoader lLoader;
			lOk = (lLoader.Load(_T("Data/env.png"), lCanvas) == Lepra::PngLoader::STATUS_SUCCESS);

			lLightMap.Set(lCanvas);

			if (lOk)
			{
				UiTbc::TEXLoader lTEXLoader;
				lTEXLoader.Save(_T("lightmap.tex"), lLightMap, false);
			}
			assert(lOk);
		}
	}

	UiTbc::Texture lEnvMap;
	if (lOk)
	{
		lContext = _T("load envmap (TEX)");
		UiTbc::TEXLoader lTEXLoader;
		lOk = (lTEXLoader.Load(_T("envmap.tex"), lEnvMap, false) == UiTbc::TEXLoader::STATUS_SUCCESS);
		if (!lOk)
		{
			lContext = _T("load envmap");
			Lepra::Canvas lCanvas;
			Lepra::PngLoader lLoader;
			lOk = (lLoader.Load(_T("Data/env.png"), lCanvas) == Lepra::PngLoader::STATUS_SUCCESS);

			lEnvMap.Set(lCanvas);

			if (lOk)
			{
				UiTbc::TEXLoader lTEXLoader;
				lTEXLoader.Save(_T("envmap.tex"), lEnvMap, false);
			}
			assert(lOk);
		}
	}

	UiTbc::Texture lSpecMap;
	if (lOk)
	{
		lContext = _T("load specmap (TEX)");
		UiTbc::TEXLoader lTEXLoader;
		lOk = (lTEXLoader.Load(_T("specmap.tex"), lSpecMap, false) == UiTbc::TEXLoader::STATUS_SUCCESS);
		if (!lOk)
		{
			lContext = _T("load specmap");
			Lepra::Canvas lCanvas;
			Lepra::PngLoader lLoader;
			lOk = (lLoader.Load(_T("Data/env.png"), lCanvas) == Lepra::PngLoader::STATUS_SUCCESS);

			lSpecMap.Set(lCanvas);

			if (lOk)
			{
				UiTbc::TEXLoader lTEXLoader;
				lTEXLoader.Save(_T("specmap.tex"), lSpecMap, false);
			}
			assert(lOk);
		}
	}

	UiTbc::Texture lNormalMap;
	if (lOk)
	{
		lContext = _T("load normalmap (TEX)");
		UiTbc::TEXLoader lTEXLoader;
		lOk = (lTEXLoader.Load(_T("normalmap.tex"), lNormalMap, false) == UiTbc::TEXLoader::STATUS_SUCCESS);
		if (!lOk)
		{
			lContext = _T("load normalmap");
			Lepra::Canvas lCanvas;
			Lepra::PngLoader lLoader;
			lOk = (lLoader.Load(_T("Data/env.png"), lCanvas) == Lepra::PngLoader::STATUS_SUCCESS);

			lNormalMap.Set(lCanvas);

			if (lOk)
			{
				UiTbc::TEXLoader lTEXLoader;
				lTEXLoader.Save(_T("normalmap.tex"), lNormalMap, false);
			}
			assert(lOk);
		}
	}

	UiTbc::Texture lMultiMap;
	if (lOk)
	{
		lContext = _T("load multimap (TEX)");
		UiTbc::TEXLoader lTEXLoader;
		lOk = lTEXLoader.Load(_T("multiMap.tex"), lMultiMap, false) == UiTbc::TEXLoader::STATUS_SUCCESS;
		if (!lOk)
		{
			lMultiMap.Set(*lTextureMap.GetColorMap(0), Canvas::RESIZE_FAST, 0, 0, lNormalMap.GetColorMap(0), lSpecMap.GetColorMap(0));
			UiTbc::TEXLoader lTEXLoader;
			lTEXLoader.Save(_T("multiMap.tex"), lMultiMap, false);
			lOk = true;
		}
		assert(lOk);
	}

	if (lOk)
	{
		lContext = _T("add texturemap");
		if (pContext == UiLepra::DisplayManager::OPENGL_CONTEXT)
			lTextureMap.SwapRGBOrder();

		gTextureId[TEXTUREMAP] = gRenderer->AddTexture(&lTextureMap);
		lOk = (gTextureId[TEXTUREMAP] != 0);
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("add texturemap to painter");
		gImageId[TEXTUREMAP] = gPainter->AddImage(lTextureMap.GetColorMap(0), lTextureMap.GetAlphaMap(0));
		lOk = (gImageId[TEXTUREMAP] != 0);
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("add lightmap");
		if (pContext == UiLepra::DisplayManager::OPENGL_CONTEXT)
			lLightMap.SwapRGBOrder();

		gTextureId[LIGHTMAP] = gRenderer->AddTexture(&lLightMap);
		lOk = (gTextureId[LIGHTMAP] != 0);
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("add envmap");
		if (pContext == UiLepra::DisplayManager::OPENGL_CONTEXT)
			lEnvMap.SwapRGBOrder();

		gTextureId[ENVMAP] = gRenderer->AddTexture(&lEnvMap);
		lOk = (gTextureId[ENVMAP] != 0);
		if (lOk)
		{
			gRenderer->SetEnvironmentMap(gTextureId[ENVMAP]);
		}
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("add specmap");
		if (pContext == UiLepra::DisplayManager::OPENGL_CONTEXT)
			lSpecMap.SwapRGBOrder();

		gTextureId[SPECMAP] = gRenderer->AddTexture(&lSpecMap);
		lOk = (gTextureId[SPECMAP] != 0);
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("add normalmap");
		if (pContext == UiLepra::DisplayManager::OPENGL_CONTEXT)
			lNormalMap.SwapRGBOrder();

		gTextureId[NORMALMAP] = gRenderer->AddTexture(&lNormalMap);
		lOk = (gTextureId[NORMALMAP] != 0);
		assert(lOk);
	}
	if (lOk)
	{
		lContext = _T("add multimap");
		if (pContext == UiLepra::DisplayManager::OPENGL_CONTEXT)
			lMultiMap.SwapRGBOrder();

		gTextureId[MULTIMAP] = gRenderer->AddTexture(&lMultiMap);
		lOk = (gTextureId[MULTIMAP] != 0);
		assert(lOk);
	}

	if (lOk)
	{
		lContext = _T("initialize renderer");
		gRenderer->SetLightsEnabled(false);
		gRenderer->SetAmbientLight(0.5, 0.5, 0.5);
		gRenderer->SetTrilinearFilteringEnabled(true);
	}
	if (!lOk)
	{
		CloseRenderer();
	}


	str lRendererType = _T("OpenGL");
	if (pContext != UiLepra::DisplayManager::OPENGL_CONTEXT)
	{
		lRendererType = _T("D3D");
	}
	ReportTestResult(pLog, lRendererType+_T("Renderer"), lContext, lOk);

	return (lOk);
}

bool ClearBackground()
{
	LEPRA_MEASURE_SCOPE(ClearScreen);
	gRenderer->SetViewport(Lepra::PixelRect(0, 0, gScreen->GetWidth(), gScreen->GetHeight()));
	gRenderer->SetViewFrustum(90, 0.1f, 10000);
	gRenderer->SetClippingRect(Lepra::PixelRect(0, 0, gScreen->GetWidth(), gScreen->GetHeight()));
	gRenderer->SetClearColor(Lepra::Color(0, 0, 0, 255));
	gRenderer->Clear();
	return (true);
}

bool ClearSubframe(int pXBox = 0, int pYBox = 0, int pSplitsX = 1, int pSplitsY = 1, int pFrameFatso = 4, bool lClearColorOnly = true)
{
	LEPRA_MEASURE_SCOPE(ClearFrame);

	const int lFrameXSize = gScreen->GetWidth()/pSplitsX - pSplitsX + 1;
	const int lFrameYSize = gScreen->GetHeight()/pSplitsY - pSplitsY + 1;
	const int xo = lFrameXSize*pXBox;
	const int yo = lFrameYSize*pYBox;
	const int lFrameThickness = lFrameXSize/pFrameFatso;
	gX = xo+lFrameThickness;
	gY = yo+lFrameThickness;
	int lRight  = xo + lFrameXSize-lFrameThickness-1;
	int lBottom = yo + lFrameYSize-lFrameThickness-1;

	gRenderer->SetViewport(Lepra::PixelRect(gX, gY, lRight, lBottom));
	gRenderer->SetViewFrustum(gRenderer->CalcFOVAngle(90, (float)(lRight - gX) / (float)(lBottom - gY)), 1, 1000);
	gRenderer->SetClippingRect(Lepra::PixelRect(gX, gY, lRight, lBottom));


	gRenderer->SetClearColor(Lepra::Color(
		(Lepra::uint8)(128-128*pXBox/pSplitsX),
		(Lepra::uint8)(128*pYBox/pSplitsY),
		(Lepra::uint8)(128*pXBox/pSplitsX),
		255));

	// Set camera transformation according to to placement.
/*	Lepra::RotationMatrixF lRotation;
	lRotation.MakeIdentity();
//	lRotation.RotateAroundOwnX(1.57);
	Lepra::Vector3DF lTransformation(0, 0, 0);
	lTransformation.y -= (1.0+pXBox*2.0-pSplitsX)/pSplitsX*OBJECT_DISTANCE;
	lTransformation.z -= (1.0+pYBox*2.0-pSplitsY)/pSplitsY*0.8*OBJECT_DISTANCE;
	Lepra::TransformationF lCameraTrans(lRotation, lTransformation);
	gRenderer->SetCameraTransformation(lCameraTrans);
*/
	unsigned lClearFlags = UiTbc::Renderer::CLEAR_COLORBUFFER;
	if (!lClearColorOnly)
	{
		lClearFlags |= UiTbc::Renderer::CLEAR_DEPTHBUFFER;
	}
	gRenderer->Clear(lClearFlags);
	return (true);
}

bool ResetAndClearFrame()
{
	bool lCleared = ClearBackground();
	return (lCleared);
}

void PrintInfo(const str& pInfo)
{
	gPainter->ResetClippingRect();
	gPainter->SetColor(Lepra::Color(255, 255, 255, 255), 0);
	gPainter->SetColor(Lepra::Color(0, 0, 0, 0), 1);
	gPainter->PrintText(pInfo, gX+2, gY+2);
}

bool InitializeGeometry(TBC::GeometryBase* pGeometry)
{
	Lepra::RotationMatrixF lRotation;
	lRotation.MakeIdentity();
	Lepra::TransformationF lObjectTrans(lRotation, Lepra::Vector3DF(0, OBJECT_DISTANCE, 0));
	pGeometry->SetTransformation(lObjectTrans);
	TBC::GeometryBase::BasicMaterialSettings lMaterial(
		Lepra::Vector3DF(0, 0, 0),
		Lepra::Vector3DF(0.8f, 0.9f, 0.7f),
		Lepra::Vector3DF(0.1f, 0.1f, 0.1f),
		0.8f, 1.0f, true);
	pGeometry->SetBasicMaterialSettings(lMaterial);
	return (true);
}

bool QuickRender(TBC::GeometryBase* pGeometry, const UiTbc::Renderer::MaterialType pMaterial, bool pUpdateScreen,
		 const Lepra::TransformationF* pTransform = 0, double pDuration = 0.5, const Lepra::tchar* /*pText*/ = 0,
		 bool pClearScreen = true, float pRotationSpeed = 0.1f, const Lepra::Vector3DF pRotationAxis = Lepra::Vector3DF(1, 1, 1))
{
	bool lOk = true;

	UiTbc::Renderer::GeometryID lGraphicId = UiTbc::Renderer::INVALID_GEOMETRY;
	if (lOk)
	{
		lGraphicId = gRenderer->AddGeometry(pGeometry, pMaterial, UiTbc::Renderer::NO_SHADOWS);
		lOk = (lGraphicId != UiTbc::Renderer::INVALID_GEOMETRY);
		assert(lOk);
		for (int x = 0; lOk && x < gTextureMapCount; ++x)
		{
			lOk = gRenderer->TryAddGeometryTexture(lGraphicId, gTextureId[x]);
			assert(lOk);
		}
	}
	if (lOk)
	{
		Lepra::Timer lTotalTimer;
		Lepra::HiResTimer lFrameTimer;

		int i = 0;

		//
		// Test Quaternion and RotationMatrix. They should give equal results.
		//

		Lepra::QuaternionF lQRot;
		Lepra::RotationMatrixF lMRot;
		lQRot.RotateAroundWorldX(1.7f);
		lMRot.RotateAroundWorldX(1.7f);

		lQRot.RotateAroundWorldY(1.23f);
		lMRot.RotateAroundWorldY(1.23f);

		lQRot.RotateAroundWorldZ(1.14f);
		lMRot.RotateAroundWorldZ(1.14f);

		lQRot.RotateAroundOwnX(1.7f);
		lMRot.RotateAroundOwnX(1.7f);

		lQRot.RotateAroundOwnY(1.23f);
		lMRot.RotateAroundOwnY(1.23f);

		lQRot.RotateAroundOwnZ(1.14f);
		lMRot.RotateAroundOwnZ(1.14f);

		lQRot.RotateAroundVector(Lepra::Vector3DF(1, 0, 0), 0.2f);
		lMRot.RotateAroundVector(Lepra::Vector3DF(1, 0, 0), 0.2f);

		do
		{
			if (pClearScreen)
			{
				LEPRA_MEASURE_SCOPE(ClearScreen);
				gRenderer->Clear();
			}

			// Change between quaternions and matrices every second frame.
			// If everything works the user will not see the difference.
			Lepra::TransformationF lObjectMovement;
			if (i & 2)
			{
				Lepra::QuaternionF lRotation;
				lRotation.RotateAroundVector(pRotationAxis, (float)lFrameTimer.GetTimeDiff()*pRotationSpeed*Lepra::PIF*2/1.5f);

				// Test conversion to and from RotationMatrix.
				Lepra::RotationMatrixF lMtx(lRotation.GetAsRotationMatrix());
				lRotation.Set(lMtx);

				lObjectMovement.SetOrientation(lRotation * lQRot);
				lObjectMovement.SetPosition((lRotation * lQRot) * Lepra::Vector3DF(0, 5, 0) + Lepra::Vector3DF(0, OBJECT_DISTANCE, 0));
			}
			//TODO:
			//	This is a bug. When switching rotation algo every other frame (uncomment this and the if-statement above),
			//	the whole mesh starts flickering in the terrain test case.*/
			else
			{
				Lepra::RotationMatrixF lRotation;
				lRotation.RotateAroundVector(pRotationAxis, (float)lFrameTimer.GetTimeDiff()*pRotationSpeed*Lepra::PIF*2/1.5f);
				lObjectMovement.SetOrientation(lRotation * lMRot);
				lObjectMovement.SetPosition((lRotation * lMRot) * Lepra::Vector3DF(0, 5, 0) + Lepra::Vector3DF(0, OBJECT_DISTANCE, 0));
			}
			if (pTransform)
			{
				lObjectMovement = lObjectMovement*(*pTransform);
			}
			pGeometry->SetTransformation(lObjectMovement);
			pGeometry->SetLastFrameVisible(gRenderer->GetCurrentFrame());
			{
				LEPRA_MEASURE_SCOPE(RenderScene);
				gRenderer->RenderScene();
			}
			i++;

			if (pUpdateScreen)
			{
				//RenderGDITestImage(pText);

				LEPRA_MEASURE_SCOPE(UpdateScreen);
				gDisplay->UpdateScreen();
				UiLepra::Core::ProcessMessages();
				if (Lepra::SystemManager::GetQuitRequest())
				{
					CloseRenderer();
					Lepra::SystemManager::ExitProcess(0);
				}
			}

			lTotalTimer.UpdateTimer();
			lFrameTimer.UpdateTimer();
		}
		while (pUpdateScreen && lTotalTimer.GetTimeDiff() < pDuration);
	}
	if (lGraphicId != UiTbc::Renderer::INVALID_GEOMETRY)
	{
		gRenderer->RemoveGeometry(lGraphicId);
	}
	return lOk;
}

bool AddRandomVertexColor(UiTbc::TriangleBasedGeometry* pGeometry)
{
	bool lOk = true;
	if (lOk)
	{
		Lepra::uint8* lVertexColor = new Lepra::uint8[pGeometry->GetVertexCount()*4];
		for (unsigned y = 0; y < pGeometry->GetVertexCount()*4; y += 4)
		{
			lVertexColor[y+0] = (Lepra::uint8)Lepra::Random::GetRandomNumber();
			lVertexColor[y+1] = (Lepra::uint8)Lepra::Random::GetRandomNumber();
			lVertexColor[y+2] = (Lepra::uint8)Lepra::Random::GetRandomNumber();
			lVertexColor[y+3] = (Lepra::uint8)Lepra::Random::GetRandomNumber();
		}
		pGeometry->SetColorData(lVertexColor, TBC::GeometryBase::COLOR_RGBA);
		delete[] (lVertexColor);
	}
	return (lOk);
}

bool AddMappingCoords(UiTbc::TriangleBasedGeometry* pGeometry)
{
	bool lOk = true;
	pGeometry->SplitVertices();
	pGeometry->AddEmptyUVSet();
	Lepra::Vector2DD lUVOffset(0.5, 0.5);
	UiTbc::UVMapper::ApplyCubeMapping(pGeometry, 0, pGeometry->GetBoundingRadius(), lUVOffset);
	pGeometry->DupUVSet(0);

	/*if (lOk)
	{
		unsigned lCorner = 0;
		unsigned lVertexCount = pGeometry->GetVertexCount();
		float* lUVCoords = new float[lVertexCount*2];
		for (unsigned z = 0; z < lVertexCount; ++z)
		{
			switch (lCorner)
			{
				case 0:
				{
					lUVCoords[z*2+0] = 0;
					lUVCoords[z*2+1] = 0;
				}
				break;
				case 1:
				{
					lUVCoords[z*2+0] = 1;
					lUVCoords[z*2+1] = 0;
				}
				break;
				case 2:
				{
					lUVCoords[z*2+0] = 1;
					lUVCoords[z*2+1] = 1;
				}
				break;
				case 3:
				{
					lUVCoords[z*2+0] = 0;
					lUVCoords[z*2+1] = 1;
				}
				break;
			}
			lCorner = (lCorner+1)&3;
		}
		pGeometry->AddUVSet(lUVCoords);	// Add for texturemap.
		pGeometry->AddUVSet(lUVCoords);	// Add for lightmap.
		delete[] (lUVCoords);
	}*/

	return (lOk);
}

bool AddUVAnimation(TBC::GeometryBase* pGeometry)
{
	// TODO: Fix the memory leaks.

	TBC::BoneHierarchy* lBones = new TBC::BoneHierarchy;
	lBones->SetBoneCount(1);
	lBones->FinalizeInit(TBC::BoneHierarchy::TRANSFORM_NONE);

	TBC::BoneAnimation* lAnimation = new TBC::BoneAnimation;
	lAnimation->SetKeyframeCount(4, true);
	lAnimation->SetBoneCount(1);
	lAnimation->SetTimeTag(0, 0);
	lAnimation->SetTimeTag(1, 0.2f);
	lAnimation->SetTimeTag(2, 0.4f);
	lAnimation->SetTimeTag(3, 0.6f);
	lAnimation->SetTimeTag(4, 0.8f);
	lAnimation->SetBoneTransformation(0, 0, 
		Lepra::TransformationF(Lepra::g3x3IdentityMatrixF,
				       Lepra::Vector3DF(0.5f, 0.5f, 0)));
	lAnimation->SetBoneTransformation(1, 0, 
		Lepra::TransformationF(Lepra::g3x3IdentityMatrixF,
				       Lepra::Vector3DF(0.5f, -0.5f, 0)));
	lAnimation->SetBoneTransformation(2, 0, 
		Lepra::TransformationF(Lepra::g3x3IdentityMatrixF,
				       Lepra::Vector3DF(-0.5f, -0.5f, 0)));
	lAnimation->SetBoneTransformation(3, 0, 
		Lepra::TransformationF(Lepra::g3x3IdentityMatrixF,
				       Lepra::Vector3DF(-0.5, 0.5, 0)));

	TBC::BoneAnimator* lAnimator = new TBC::BoneAnimator(lBones);
	str lAnimName(_T("UVAnimation"));
	lAnimator->AddAnimation(lAnimName, lAnimation);
	lAnimator->StartAnimation(lAnimName, 0, TBC::BoneAnimation::MODE_PLAY_LOOP);

	pGeometry->SetUVAnimator(lAnimator);

	return true;
}

bool TestSkinningSaveLoad(const Lepra::LogDecorator& pLog, double pShowTime)
{
	// Performs the following steps:
	//  1. Create+save mesh 1.
	//  2. Load mesh 1.
	//  3. Create+save mesh 2.
	//  4. Load mesh 2.
	//  5. Create+save skin 1.
	//  6. Load skin 1.
	//  7. Create+save skin 2.
	//  8. Load skin 2.
	//  9. Create+save animation.
	// 10. Load animation.
	// 11. Create+save structure.
	// 12. Load structure.
	// 13. Set two different materials on the two meshes.
	// 14. Render+animate the bloody thing!

	str lContext;
	bool lTestOk = true;
	const str lFileName(_T("chain"));

	const float lCuboidLength = 40.0f;
	UiTbc::TriangleBasedGeometry lGeometry[2];
	for (int lMeshIndex = 0; lMeshIndex < 2; ++lMeshIndex)
	{
		const str lThisMeshName = lFileName+Lepra::strutil::Format(_T("%i.mesh"), lMeshIndex);
		if (lTestOk)
		{
			lContext = _T("save chain mesh");
			//UiTbc::TriangleBasedGeometry* lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(lCuboidLength/4.0f, lCuboidLength/2.0f, lCuboidLength/4.0f, 1, 2, 1);
			UiTbc::TriangleBasedGeometry* lGeometry = UiTbc::BasicMeshCreator::CreateTorus(lCuboidLength/4.0f, lCuboidLength/10.0f, lCuboidLength/10.0f, 32, 24);
			//UiTbc::TriangleBasedGeometry* lGeometry = UiTbc::BasicMeshCreator::CreateCylinder(lCuboidLength/10.0f, lCuboidLength/10.0f, lCuboidLength/2.0f, 32);
			Lepra::RotationMatrixF lRotation;
			lRotation.RotateAroundVector(Lepra::Vector3DF(1, 0, 0), -Lepra::PIF/2.0f);
			lGeometry->Rotate(lRotation);
			float lDisplacement = (lMeshIndex*2-1)*lCuboidLength/4.0f;
			lGeometry->Translate(Lepra::Vector3DF(0, lDisplacement, 0));
			Lepra::DiskFile lFile;
			lTestOk = lFile.Open(lThisMeshName, Lepra::DiskFile::MODE_WRITE);
			assert(lTestOk);
			if (lTestOk)
			{
				lTestOk = InitializeGeometry(lGeometry);
				assert(lTestOk);
			}
			if (lTestOk)
			{
				AddRandomVertexColor(lGeometry);
			}
			if (lTestOk)
			{
				UiTbc::ChunkyMeshLoader lMeshLoader(&lFile, false);
				lTestOk = lMeshLoader.Save(lGeometry, true);
				assert(lTestOk);
			}
			delete (lGeometry);
		}
		if (lTestOk)
		{
			lContext = _T("load chain mesh");
			Lepra::DiskFile lFile;
			lTestOk = lFile.Open(lThisMeshName, Lepra::DiskFile::MODE_READ);
			assert(lTestOk);
			if (lTestOk)
			{
				UiTbc::ChunkyMeshLoader lMeshLoader(&lFile, false);
				bool lCastsShadows;
				lTestOk = lMeshLoader.Load(&lGeometry[lMeshIndex], lCastsShadows);
				assert(lTestOk && lCastsShadows);
			}
		}
		Lepra::DiskFile::Delete(lThisMeshName);
	}

	UiTbc::AnimatedGeometry lSkin[2];
	for (int lSkinIndex = 0; lSkinIndex < 2; ++lSkinIndex)
	{
		const str lThisSkinName = lFileName+Lepra::strutil::Format(_T("%i.skin"), lSkinIndex);
		if (lTestOk)
		{
			lContext = _T("save chain skin");
			UiTbc::BasicMeshCreator::CreateYBonedSkin(-lCuboidLength/2.0f, +lCuboidLength/2.0f, &lGeometry[lSkinIndex], &lSkin[lSkinIndex], 2);
			Lepra::DiskFile lFile;
			lTestOk = lFile.Open(lThisSkinName, Lepra::DiskFile::MODE_WRITE);
			assert(lTestOk);
			if (lTestOk)
			{
				UiTbc::ChunkySkinLoader lSkinLoader(&lFile, false);
				lTestOk = lSkinLoader.Save(&lSkin[lSkinIndex]);
				assert(lTestOk);
			}
		}
		if (lTestOk)
		{
			lContext = _T("load chain skin");
			Lepra::DiskFile lFile;
			lTestOk = lFile.Open(lThisSkinName, Lepra::DiskFile::MODE_READ);
			assert(lTestOk);
			if (lTestOk)
			{
				UiTbc::ChunkySkinLoader lSkinLoader(&lFile, false);
				lTestOk = lSkinLoader.Load(&lSkin[lSkinIndex]);
				assert(lTestOk);
			}
		}
		Lepra::DiskFile::Delete(lThisSkinName);
	}

	const str lAnimationName(lFileName+_T(".animation"));
	if (lTestOk)
	{
		lContext = _T("save chain animation");
		TBC::BoneAnimation lAnimation;
		lAnimation.SetDefaultMode(TBC::BoneAnimation::MODE_PLAY_LOOP);
		lAnimation.SetRootNodeName(_T("any_friggen_node"));
		lAnimation.SetKeyframeCount(2, true);
		lAnimation.SetBoneCount(2);
		lAnimation.SetTimeTag(0, 0);
		lAnimation.SetTimeTag(1, 0.5f);
		lAnimation.SetTimeTag(2, 1.0f);
		Lepra::TransformationF lTransform0 = Lepra::gIdentityTransformationF;
		lAnimation.SetBoneTransformation(0, 0, lTransform0);
		Lepra::TransformationF lTransform1 = Lepra::gIdentityTransformationF;
		lTransform1.RotatePitch(-0.8f);
		lAnimation.SetBoneTransformation(0, 1, lTransform1);
		lAnimation.SetBoneTransformation(1, 0, lTransform0);
		lTransform1 = Lepra::gIdentityTransformationF;
		lTransform1.RotatePitch(+0.8f);
		lAnimation.SetBoneTransformation(1, 1, lTransform1);
		Lepra::DiskFile lFile;
		lTestOk = lFile.Open(lAnimationName, Lepra::DiskFile::MODE_WRITE);
		assert(lTestOk);
		if (lTestOk)
		{
			TBC::ChunkyAnimationLoader lAnimationLoader(&lFile, false);
			lTestOk = lAnimationLoader.Save(&lAnimation);
			assert(lTestOk);
		}
	}
	TBC::BoneAnimation lAnimation;
	if (lTestOk)
	{
		lContext = _T("load chain animation");
		Lepra::DiskFile lFile;
		lTestOk = lFile.Open(lAnimationName, Lepra::DiskFile::MODE_READ);
		assert(lTestOk);
		if (lTestOk)
		{
			TBC::ChunkyAnimationLoader lAnimationLoader(&lFile, false);
			lTestOk = lAnimationLoader.Load(&lAnimation);
			assert(lTestOk);
		}
	}
	Lepra::DiskFile::Delete(lAnimationName);

	const str lStructureName(lFileName+_T(".structure"));
	if (lTestOk)
	{
		lContext = _T("save chain structure");
		TBC::ChunkyPhysics lStructure(TBC::BoneHierarchy::TRANSFORM_NONE, TBC::ChunkyPhysics::DYNAMIC);
		lStructure.SetBoneCount(2);
		lStructure.SetBoneChildCount(0, 1);
		lStructure.SetChildIndex(0, 0, 1);
		Lepra::TransformationF lTransform = Lepra::gIdentityTransformationF;
		lStructure.SetOriginalBoneTransformation(0, lTransform);
		lStructure.SetOriginalBoneTransformation(1, lTransform);
		lStructure.SetPhysicsType(TBC::ChunkyPhysics::DYNAMIC);
		lStructure.BoneHierarchy::FinalizeInit(TBC::BoneHierarchy::TRANSFORM_NONE);
		lTestOk = (lStructure.GetBoneChildCount(lStructure.GetRootBone()) == 1);
		assert(lTestOk);
		Lepra::DiskFile lFile;
		lTestOk = lFile.Open(lStructureName, Lepra::DiskFile::MODE_WRITE);
		assert(lTestOk);
		if (lTestOk)
		{
			TBC::ChunkyPhysicsLoader lStructureLoader(&lFile, false);
			lTestOk = lStructureLoader.Save(&lStructure);
			assert(lTestOk);
		}
	}
	TBC::ChunkyPhysics lStructure(TBC::BoneHierarchy::TRANSFORM_NONE, TBC::ChunkyPhysics::DYNAMIC);
	if (lTestOk)
	{
		lContext = _T("load chain structure");
		Lepra::DiskFile lFile;
		lTestOk = lFile.Open(lStructureName, Lepra::DiskFile::MODE_READ);
		assert(lTestOk);
		if (lTestOk)
		{
			TBC::ChunkyPhysicsLoader lStructureLoader(&lFile, false);
			lTestOk = lStructureLoader.Load(&lStructure);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = lStructure.BoneHierarchy::FinalizeInit(TBC::BoneHierarchy::TRANSFORM_NONE);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (lStructure.GetBoneChildCount(lStructure.GetRootBone()) == 1);
			assert(lTestOk);
		}
	}
	Lepra::DiskFile::Delete(lStructureName);

	if (lTestOk)
	{
		lContext = _T("clear screen");
		lTestOk = ResetAndClearFrame();
		assert(lTestOk);
	}
	TBC::BoneAnimator lAnimator(&lStructure);
	if (lTestOk)
	{
		lSkin[0].SetBoneHierarchy(&lStructure);
		lSkin[1].SetBoneHierarchy(&lStructure);
		lSkin[0].SetGeometry(&lGeometry[0]);
		lSkin[1].SetGeometry(&lGeometry[1]);
		lAnimator.AddAnimation(lAnimationName, &lAnimation);
		lAnimator.StartAnimation(lAnimationName, 0, TBC::BoneAnimation::MODE_PLAY_DEFAULT);
	}
	Lepra::Timer lTotalTimer;
	Lepra::HiResTimer lFrameTimer;
	Lepra::TransformationF lObjectTransform;
	while (lTestOk && lTotalTimer.GetTimeDiff() < pShowTime)
	{
		lContext = _T("render");
		if (lTestOk)
		{
			lTestOk = QuickRender(&lSkin[0], UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID, false, &lObjectTransform, -1.0, 0, true);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = QuickRender(&lSkin[1], UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, true, &lObjectTransform, -1.0, _T("Loaded from disk!"), false);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			Lepra::QuaternionF lRotation;
			lRotation.RotateAroundVector(Lepra::Vector3DF(0.0f, 0.0f, 1), (float)lTotalTimer.GetTimeDiff()*Lepra::PIF*2/6.7f);
			lObjectTransform.SetOrientation(lRotation);

			lSkin[0].UpdateAnimatedGeometry();
			lSkin[1].UpdateAnimatedGeometry();
			lAnimator.Step((float)lFrameTimer.PopTimeDiff());
			lTotalTimer.UpdateTimer();
		}
	}

	ReportTestResult(pLog, _T("SkinningLoadSave"), lContext, lTestOk);
	return (lTestOk);
}

bool TestMeshImport(const Lepra::LogDecorator& pLog, double pShowTime)
{
	str lContext;
	bool lTestOk = true;

	UiTbc::TriangleBasedGeometry lGeometry;
	if (lTestOk)
	{
		lContext = _T("load imported mesh");
		const str lMeshName = _T("Data/tractor_01_rear_wheel0.mesh");
		Lepra::DiskFile lFile;
		lTestOk = lFile.Open(lMeshName, Lepra::DiskFile::MODE_READ);
		assert(lTestOk);
		if (lTestOk)
		{
			UiTbc::ChunkyMeshLoader lMeshLoader(&lFile, false);
			bool lCastsShadows;
			lTestOk = lMeshLoader.Load(&lGeometry, lCastsShadows);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			lTestOk = (lGeometry.GetVertexCount() == 442 && lGeometry.GetIndexCount() == 2640);
			assert(lTestOk);
		}
		if (lTestOk)
		{
			float* lVertices = lGeometry.GetVertexData();
			unsigned vc = lGeometry.GetVertexCount()*3;
			for (unsigned x = 0; x < vc; ++x)
			{
				lVertices[x] *= 7;
			}
			AddRandomVertexColor(&lGeometry);
		}
	}

	if (lTestOk)
	{
		lContext = _T("clear screen");
		lTestOk = ResetAndClearFrame();
	}
	Lepra::Timer lTotalTimer;
	Lepra::TransformationF lObjectTransform;
	while (lTestOk && lTotalTimer.GetTimeDiff() < pShowTime)
	{
		lContext = _T("render");
		if (lTestOk)
		{
			lTestOk = QuickRender(&lGeometry, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, true, &lObjectTransform, -1.0, 0, true);
		}
		if (lTestOk)
		{
			Lepra::QuaternionF lRotation;
			lRotation.RotateAroundVector(Lepra::Vector3DF(0.1f, -0.1f, 1), (float)lTotalTimer.GetTimeDiff()*Lepra::PIF*2/6.7f);
			lObjectTransform.SetOrientation(lRotation);
			lTotalTimer.UpdateTimer();
		}
	}

	ReportTestResult(pLog, _T("MeshImport"), lContext, lTestOk);
	return (lTestOk);
}

bool TestLoadClass(const Lepra::LogDecorator& pLog)
{
	str lContext;
	bool lTestOk = true;

	Lepra::DiskFile lFile;
	if (lTestOk)
	{
		lContext = _T("open file");
		const str lClassName = _T("Data/tractor_01.class");
		lTestOk = lFile.Open(lClassName, Lepra::DiskFile::MODE_READ);
		assert(lTestOk);
	}
	UiTbc::ChunkyClass lClass;
	if (lTestOk)
	{
		lContext = _T("load class");
		UiTbc::ChunkyClassLoader lLoader(&lFile, false);
		lTestOk = lLoader.Load(&lClass);
		assert(lTestOk);
	}

	ReportTestResult(pLog, _T("LoadClass"), lContext, lTestOk);
	return (lTestOk);
}

TerrainFunctionTest::TerrainFunctionTest(const Lepra::LogDecorator& pLog) :
	SceneTest(pLog, _T("TerrainFunctionTest"))
{
	const float lPatchLeft = 5;
	const float lPatchSize = 20;
	const Lepra::Vector2DF lVolcanoPosition(lPatchSize-lPatchLeft, 2*lPatchLeft);
	const Lepra::Vector2DF lDunePosition(-lPatchLeft, 2*lPatchLeft);

	TBC::TerrainPatch::SetDimensions(6, lPatchSize);

	int lTriangleCount = 0;
	const int lPatchCountSide = 10;
	const int lPatchCount = lPatchCountSide*lPatchCountSide;
	TBC::TerrainPatch* lPatch[lPatchCount];
	for (int y = 0; y < lPatchCountSide; ++y)
	{
		for (int x = 0; x < lPatchCountSide; ++x)
		{
			const int lHalfSideCount = lPatchCountSide/2;
			lPatch[y*lPatchCountSide+x] = new TBC::TerrainPatch(Lepra::Vector2D<int>(x-lHalfSideCount, y-lHalfSideCount), 0, 0, 0, 0, 8, 8, 0);
			lTriangleCount += lPatch[y*lPatchCountSide+x]->GetTriangleCount();
		}
	}
	mTriangleCountInfo = _T(" - ") + Lepra::Number::ConvertToPostfixNumber(lTriangleCount, 1) + _T(" triangles in scene.");
	log_volatile(mLog.Debug(mTriangleCountInfo));
	if (mTestOk)
	{
		mContext = _T("cone function");
		const float lConeAmplitude = 4.0f;
		TBC::TerrainConeFunction lConeFunction(lConeAmplitude, lVolcanoPosition, (lPatchLeft+1)*2, lPatchLeft*3);
		TBC::TerrainWidthFunction lWidthFunction(0.5f, &lConeFunction);
		const float lConePushVector[] = {0.0f, -2.0f, -1.5f, -3.0f, -0.0f};
		TBC::TerrainPushFunction lPushFunction(lConePushVector, 5, &lWidthFunction);
		for (int x = 0; x < lPatchCount; ++x)
		{
			lPushFunction.AddFunction(*lPatch[x]);
		}
		//mTestOk = (lGrid[2*lVertexCountX*((lVertexCountY+1)/4)+(lVertexCountX-1)*2].z >= lConeAmplitude*0.8);
		assert(mTestOk);
	}
	if (mTestOk)
	{
		mContext = _T("hemisphere functions");
		const float lHemisphereAmplitude = 4.0f;
		TBC::TerrainHemisphereFunction lHemisphereFunction(-lHemisphereAmplitude, lVolcanoPosition, lPatchLeft-1, lPatchLeft-1);
		for (int x = 0; x < lPatchCount; ++x)
		{
			lHemisphereFunction.AddFunction(*lPatch[x]);
		}
		TBC::TerrainHemisphereFunction lHemisphereFunction2(lHemisphereAmplitude, lVolcanoPosition, lPatchLeft/2, lPatchLeft/2);
		for (int x = 0; x < lPatchCount; ++x)
		{
			lHemisphereFunction2.AddFunction(*lPatch[x]);
		}
		//mTestOk = (lGrid[2*lVertexCountX*((lVertexCountY+1)/4)+(lVertexCountX-1)*2].z >= lConeAmplitude*0.8);
		assert(mTestOk);
	}
	if (mTestOk)
	{
		mContext = _T("dune function");
		const float lDuneAmplitude = 4.0f;
		TBC::TerrainDuneFunction lDuneFunction(0.1f, 1.0f, lDuneAmplitude, lDunePosition, lPatchLeft*1.5f, lPatchLeft*3);
		const float lDuneAmplitudeVector[] = {1.0f, 2.0f, 1.0f, 1.5f, 1.0f, 1.0f};
		TBC::TerrainAmplitudeFunction lAmplitudeFunction(lDuneAmplitudeVector, 6, &lDuneFunction);
		const float lDunePushVector[] = {0.0f, 1.5f, 2.5f, -1.0f, -2.0f, 0.0f};
		TBC::TerrainPushFunction lPushFunction(lDunePushVector, 6, &lAmplitudeFunction);
		TBC::TerrainRotateFunction lRotateFunction(-Lepra::PIF*3/4, &lPushFunction);
		for (int x = 0; x < lPatchCount; ++x)
		{
			lRotateFunction.AddFunction(*lPatch[x]);
		}
		//mTestOk = (lGrid[2*lVertexCountX*((lVertexCountY+1)/4)+(lVertexCountX-1)*2].z >= lConeAmplitude*0.9);
		assert(mTestOk);
	}
	if (mTestOk)
	{
		mContext = _T("zero effect validation");
		//mTestOk = (lGrid[0].z == 0 && lGrid[1].z == 0);
		assert(mTestOk);
	}
	if (mTestOk)
	{
		mContext = _T("initializing patch geometry");
		for (int x = 0; mTestOk && x < lPatchCount; ++x)
		{
			mTestOk = InitializeGeometry(lPatch[x]);
			assert(mTestOk);
		}
	}
	if (mTestOk)
	{
		mContext = _T("rendering terrain");
		Lepra::TransformationF lTransform;
		lTransform.RotatePitch(-0.4f);
		lTransform.MoveUp(3.0f);
		lTransform.MoveBackward(20.0f);

		for (int x = 0; x < lPatchCount; ++x)
		{
			lPatch[x]->SetAlwaysVisible(true);
			UiTbc::Renderer::GeometryID lGeometry = gRenderer->AddGeometry(lPatch[x], UiTbc::Renderer::MAT_SINGLE_TEXTURE_SOLID, UiTbc::Renderer::NO_SHADOWS);
			gRenderer->TryAddGeometryTexture(lGeometry, gTextureId[TEXTUREMAP]);
		}
		gRenderer->SetCameraTransformation(lTransform);
	}
}

TerrainFunctionTest::~TerrainFunctionTest()
{
}

void TerrainFunctionTest::UpdateScene(double, double)
{
	mExtraInfo = mTriangleCountInfo;
}

bool TestMaterials(const Lepra::LogDecorator& pLog, double pShowTime)
{
	gTotalFps = 0;

	str lContext = _T("clear");
	bool lTestOk = ResetAndClearFrame();

	//UiTbc::TriangleBasedGeometry* lGeometry = UiTbc::BasicMeshCreator::CreateCone(15, 40, 24);
	UiTbc::TriangleBasedGeometry* lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(10, 10, 10, 1, 1, 4);
	//UiTbc::TriangleBasedGeometry* lGeometry = UiTbc::BasicMeshCreator::CreateEllipsoid(10, 11, 12, 10, 10);
	if (lTestOk)
	{
		lContext = _T("initialize geometry");
		lTestOk = InitializeGeometry(lGeometry);
	}
	//UiTbc::Renderer::GeometryID lGeometryId = UiTbc::Renderer::INVALID_GEOMETRY;
	if (lTestOk)
	{
		lContext = _T("add vertex color data");
		lTestOk = AddRandomVertexColor(lGeometry);
	}
	if (lTestOk)
	{
		lContext = _T("add texture coords");
		lTestOk = AddMappingCoords(lGeometry);
	}
	/*if (lTestOk)
	{
		lContext = _T("add geometry");
		lGeometryId = gRenderer->AddGeometry(lGeometry, UiTbc::Renderer::MAT_SINGLE_TEXTURE_SOLID, gTextureId, 1, UiTbc::Renderer::NO_SHADOWS);
		lTestOk = (lGeometryId != UiTbc::Renderer::INVALID_GEOMETRY);
	}*/

	int lFrameCount = 0;
	Lepra::HiResTimer lTotalTimer;
	bool lFirstTime = true;
	Lepra::HiResTimer lFrameTimer;
	Lepra::TransformationF lObjectTransform;
	do
	{
		lTestOk = !Lepra::SystemManager::GetQuitRequest();

		if (lTestOk)
		{
			lTestOk = ClearBackground();
			assert(lTestOk);
		}

		// ----------------------- Draw all frames and geometry within. -----------------------
		UiTbc::Renderer::LightID lLights[2];
		lLights[0] = UiTbc::Renderer::INVALID_LIGHT;
		lLights[1] = UiTbc::Renderer::INVALID_LIGHT;
		for (int y = 0; y < 4; y += 2)
		{
			str lModeInfo;
			// Set lighting according to what row we're rendering.
			if (y == 0)
			{
				lModeInfo = _T("Ambient");
				gRenderer->SetLightsEnabled(false);
				gRenderer->SetAmbientLight(0.5, 0.5, 0.5);
			}
			else
			{
				lModeInfo = _T("2 point lights");
				if (lTestOk)
				{
					lContext = _T("add point light 0");
					gRenderer->SetAmbientLight(0.1f, 0.1f, 0.1f);
					gRenderer->SetLightsEnabled(true);
					lLights[0] = gRenderer->AddPointLight(UiTbc::Renderer::LIGHT_STATIC, Vector3DF(30, OBJECT_DISTANCE - 60, -30), Vector3DF(50, 100, 50), OBJECT_DISTANCE, OBJECT_DISTANCE*10);
					lTestOk = (lLights[0] != UiTbc::Renderer::INVALID_LIGHT);
					assert(lTestOk);
				}
				if (lTestOk)
				{
					lContext = _T("add point light 1");
					lLights[1] = gRenderer->AddPointLight(UiTbc::Renderer::LIGHT_STATIC, Vector3DF(-30, OBJECT_DISTANCE - 60, 30), Vector3DF(50, 50, 100), OBJECT_DISTANCE, OBJECT_DISTANCE*10);
					lTestOk = (lLights[1] != UiTbc::Renderer::INVALID_LIGHT);
					assert(lTestOk);
				}
				if (lTestOk)
				{
					lContext = _T("add # lights");
					lTestOk = (gRenderer->GetLightCount() == 2);
					assert(lTestOk);
				}
			}

			// Render the different scenes.
			if (lTestOk)
			{
				lContext = _T("single solid");
				ClearSubframe(0, 0+y, 5, 4, 30, false);
				lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID, false, &lObjectTransform);
				assert(lTestOk);
				PrintInfo(_T("Flat\nSolid\n") + lModeInfo);
			}
			if (lTestOk)
			{
				lContext = _T("vertex solid");
				ClearSubframe(1, 0+y, 5, 4, 30, false);
				// TODO: make this render correctly with vertex colors.
				lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, false, &lObjectTransform);
				assert(lTestOk);
				PrintInfo(_T("Vertex color\nSolid\n") + lModeInfo);
			}
			if (lTestOk)
			{
				lContext = _T("single texture blended");
				ClearSubframe(2, 0+y, 5, 4, 30, false);
				gTextureMapCount = 1;
				lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_SINGLE_TEXTURE_BLENDED, false, &lObjectTransform);
				assert(lTestOk);
				gTextureMapCount = 0;
				PrintInfo(_T("Tex\nTransparent\n") + lModeInfo);
			}
			if (lTestOk)
			{
				lContext = _T("texture and lightmap");
				ClearSubframe(3, 0+y, 5, 4, 30, false);
				gTextureMapCount = 2;
				lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_TEXTURE_AND_LIGHTMAP, false, &lObjectTransform);
				assert(lTestOk);
				gTextureMapCount = 0;
				PrintInfo(_T("Tex&lightmap\nSolid\n") + lModeInfo);
			}
			if (lTestOk)
			{
				lContext = _T("single envmap solid");
				lGeometry->GenerateVertexNormalData();
				ClearSubframe(4, 0+y, 5, 4, 30, false);
				gTextureMapCount = 1;
				gTextureIndex = ENVMAP;
				lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_SINGLE_COLOR_ENVMAP_SOLID, false, &lObjectTransform);
				assert(lTestOk);
				gTextureMapCount = 0;
				gTextureIndex = TEXTUREMAP;
				PrintInfo(_T("Envmap\nSolid\n") + lModeInfo);
			}
			if (lTestOk)
			{
				lContext = _T("single solid PXS");
				ClearSubframe(0, 1+y, 5, 4, 30, false);
				lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID_PXS, false, &lObjectTransform);
				assert(lTestOk);
				PrintInfo(_T("Flat PXS\nSolid\n") + lModeInfo);
			}
			if (lTestOk)
			{
				lContext = _T("single texture solid PXS");
				ClearSubframe(2, 1+y, 5, 4, 30, false);
				gTextureMapCount = 1;
				lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_SINGLE_TEXTURE_SOLID_PXS, false, &lObjectTransform);
				assert(lTestOk);
				gTextureMapCount = 0;
				PrintInfo(_T("Texture PXS\nSolid\n") + lModeInfo);
			}
			if (lTestOk)
			{
				lContext = _T("texture and lightmap PXS");
				ClearSubframe(3, 1+y, 5, 4, 30, false);
				gTextureMapCount = 2;
				lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_TEXTURE_AND_LIGHTMAP_PXS, false, &lObjectTransform);
				assert(lTestOk);
				gTextureMapCount = 0;
				PrintInfo(_T("Tex&light PXS\nSolid\n") + lModeInfo);
			}
			if (lTestOk)
			{
				lContext = _T("texture sbmap PXS");
				ClearSubframe(4, 1+y, 5, 4, 30, false);
				gTextureMapCount = 1;
				gTextureIndex = MULTIMAP;
				lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_TEXTURE_SBMAP_PXS, false, &lObjectTransform);
				assert(lTestOk);
				gTextureMapCount = 0;
				gTextureIndex = TEXTUREMAP;
				PrintInfo(_T("Tex&sbmap PXS\nSolid\n") + lModeInfo);
			}

			// Remove lights.
			if (lLights[0] != UiTbc::Renderer::INVALID_LIGHT)
			{
				gRenderer->RemoveLight(lLights[0]);
				gRenderer->RemoveLight(lLights[1]);
				lLights[0] = UiTbc::Renderer::INVALID_LIGHT;
				lLights[1] = UiTbc::Renderer::INVALID_LIGHT;
				if (lTestOk)
				{
					lContext = _T("remove # lights");
					lTestOk = (gRenderer->GetLightCount() == 0);
					assert(lTestOk);
				}
			}
		}

		// ----------------------- Update geometry orientation. -----------------------
		if (lTestOk)
		{
			LEPRA_MEASURE_SCOPE(UpdateScreen);

			lFrameTimer.UpdateTimer();
			gDisplay->UpdateScreen();
			UiLepra::Core::ProcessMessages();
			Lepra::RotationMatrixF lRotation;
			lRotation.RotateAroundVector(Lepra::Vector3DF(1, -1, 1), (float)(lFrameTimer.GetTimeDiff()*1.79));
			lObjectTransform.SetOrientation(lRotation);
		}

		if (lFirstTime)
		{
			lFirstTime = false;
			lTotalTimer.PopTimeDiff();
		}
		else
		{
			lTotalTimer.UpdateTimer();
			++lFrameCount;
		}
	}
	while (lTestOk && lTotalTimer.GetTimeDiff() < pShowTime);

	//gRenderer->RemoveGeometry(lGeometryId);

	delete (lGeometry);

	// Put renderer back in 
	gRenderer->SetLightsEnabled(false);
	gRenderer->SetAmbientLight(0.5, 0.5, 0.5);

	// Assign FPS meter for next test.
	gTotalFps = lFrameCount/lTotalTimer.GetTimeDiff();

	ReportTestResult(pLog, _T("RenderModes"), lContext, lTestOk);
	return (lTestOk);
}

bool TestFps(const Lepra::LogDecorator& pLog, double pAverageFps)
{
	str lContext = _T("too low");
	bool lTestOk = (gTotalFps > pAverageFps/8.0);
	assert(lTestOk);
	if (lTestOk)
	{
		lContext = _T("too high");
		lTestOk = (gTotalFps < pAverageFps*8.0);
		assert(lTestOk);
	}
	ReportTestResult(pLog, Lepra::strutil::Format(_T("FPS (%.1f)"), gTotalFps), lContext, lTestOk);
	return (lTestOk);
}

bool TestGenerate(const Lepra::LogDecorator& pLog, double pShowTime)
{
	str lContext;
	bool lTestOk = true;

	float lRotationSpeed = Lepra::PIF * 0.25f;

	if (lTestOk)
	{
		lContext = _T("box");
		UiTbc::TriangleBasedGeometry* lGeometry = UiTbc::BasicMeshCreator::CreateFlatBox(20, 10, 5, 1, 1, 1);
		lTestOk = InitializeGeometry(lGeometry);
		assert(lTestOk);
		if (lTestOk)
		{
			AddRandomVertexColor(lGeometry);
			ClearSubframe();
			lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, true, 0, pShowTime/5, 0, true, lRotationSpeed);
			assert(lTestOk);
		}
		delete (lGeometry);
	}

	if (lTestOk)
	{
		lContext = _T("cone");
		UiTbc::TriangleBasedGeometry* lGeometry = UiTbc::BasicMeshCreator::CreateCone(10, 20, 11);
		lTestOk = InitializeGeometry(lGeometry);
		assert(lTestOk);
		if (lTestOk)
		{
			AddRandomVertexColor(lGeometry);
			ClearSubframe();
			lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, true, 0, pShowTime/5, 0, true, lRotationSpeed);
			assert(lTestOk);
		}
		delete (lGeometry);
	}

	if (lTestOk)
	{
		lContext = _T("cylinder");
		UiTbc::TriangleBasedGeometry* lGeometry = UiTbc::BasicMeshCreator::CreateCylinder(10, 15, 25, 5);
		lTestOk = InitializeGeometry(lGeometry);
		assert(lTestOk);
		if (lTestOk)
		{
			AddRandomVertexColor(lGeometry);
			ClearSubframe();
			lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, true, 0, pShowTime/5, 0, true, lRotationSpeed);
			assert(lTestOk);
		}
		delete (lGeometry);
	}

	if (lTestOk)
	{
		lContext = _T("ellipsoid");
		UiTbc::TriangleBasedGeometry* lGeometry = UiTbc::BasicMeshCreator::CreateEllipsoid(10, 15, 20, 8, 8);
		lTestOk = InitializeGeometry(lGeometry);
		assert(lTestOk);
		if (lTestOk)
		{
			AddRandomVertexColor(lGeometry);
			ClearSubframe();
			lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, true, 0, pShowTime/5, 0, true, lRotationSpeed);
			assert(lTestOk);
		}
		delete (lGeometry);
	}

	if (lTestOk)
	{
		lContext = _T("torus");
		UiTbc::TriangleBasedGeometry* lGeometry = UiTbc::BasicMeshCreator::CreateTorus(15, 4, 8, 14, 10);
		lTestOk = InitializeGeometry(lGeometry);
		assert(lTestOk);
		if (lTestOk)
		{
			AddRandomVertexColor(lGeometry);
			ClearSubframe();
			lTestOk = QuickRender(lGeometry, UiTbc::Renderer::MAT_VERTEX_COLOR_SOLID, true, 0, pShowTime/5, 0, true, lRotationSpeed);
			assert(lTestOk);
		}
		delete (lGeometry);
	}

	ReportTestResult(pLog, _T("GeomGeneration"), lContext, lTestOk);
	return (lTestOk);
}


GeometryReferenceTest::GeometryReferenceTest(const Lepra::LogDecorator& pLog) :
	SceneTest(pLog, _T("GeometryReferenceTest"))
{
	gRenderer->SetClearColor(Lepra::YELLOW);

	// Default material...
	TBC::GeometryBase::BasicMaterialSettings lMaterial(
		Lepra::Vector3DF(0, 0, 0),
		Lepra::Vector3DF(1.0f, 1.0f, 1.0f),
		Lepra::Vector3DF(0.1f, 0.1f, 0.1f),
		0.8f, 1.0f, true);

	mSphere = UiTbc::BasicMeshCreator::CreateEllipsoid(1, 1, 1, 16, 8);
	lMaterial.mAmbient.x = 1.0f;
	lMaterial.mAmbient.y = 1.0f;
	lMaterial.mAmbient.z = 1.0f;
	mSphere->SetBasicMaterialSettings(lMaterial);

	mSphere->SetAlwaysVisible(false);
	UiTbc::Renderer::GeometryID lGraphicId = gRenderer->AddGeometry(mSphere, UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID, UiTbc::Renderer::CAST_SHADOWS);
	gRenderer->TryAddGeometryTexture(lGraphicId, gTextureId[TEXTUREMAP]);
	Lepra::TransformationF lTransf;
	lTransf.SetPosition(Lepra::Vector3DF(1000, 0, 1000));
	mSphere->SetTransformation(lTransf);

	mSphereReference = new TBC::GeometryReference(mSphere);
	mSphereReference->SetAlwaysVisible(true);
	lGraphicId = gRenderer->AddGeometry(mSphereReference, UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID, UiTbc::Renderer::CAST_SHADOWS);
	lTransf.SetIdentity();
	mSphereReference->SetTransformation(lTransf);

	lTransf.SetIdentity();
	lTransf.RotatePitch(-Lepra::PIF / 8.0f);
	lTransf.SetPosition(Lepra::Vector3DF(0, -2, 4));
	gRenderer->SetCameraTransformation(lTransf);
}

GeometryReferenceTest::~GeometryReferenceTest()
{
	delete (mSphere);
	delete (mSphereReference);
}

void GeometryReferenceTest::UpdateScene(double pTotalTime, double)
{
	Lepra::TransformationF lTransf;
	lTransf.SetPosition(Lepra::Vector3DF(0, 0, (float)::sin(pTotalTime)*3));
	mSphereReference->SetTransformation(lTransf);
}



BumpMapSceneTest::BumpMapSceneTest(const Lepra::LogDecorator& pLog) :
	SceneTest(pLog, _T("BumpMapScene"))
{
	gRenderer->SetClearColor(Lepra::RED);

	// Default material...
	TBC::GeometryBase::BasicMaterialSettings lMaterial(
		Lepra::Vector3DF(0, 0, 0),
		Lepra::Vector3DF(1.0f, 1.0f, 1.0f),
		Lepra::Vector3DF(0.1f, 0.1f, 0.1f),
		0.8f, 1.0f, true);

	InitTerrain();

	mSphere = UiTbc::BasicMeshCreator::CreateEllipsoid(1, 1, 1, 16, 8);
	//mTorus  = UiTbc::BasicMeshCreator::CreateTorus(0.4f, 0.06f, 0.12f, 16, 12);
	mTorus  = UiTbc::BasicMeshCreator::CreateFlatBox(1, 1, 1);
	mCone   = UiTbc::BasicMeshCreator::CreateCone(1, 5, 8);

	mTorus->SplitVertices();

	// Setup sphere.
	lMaterial.mAmbient = Lepra::Vector3DF(1,1,1);
	mSphere->SetBasicMaterialSettings(lMaterial);
	lMaterial.mAmbient = Lepra::Vector3DF(0,0,0);
	//AddRandomVertexColor(mSphere);
	AddMappingCoords(mSphere);
	mSphere->SetAlwaysVisible(true);
	UiTbc::Renderer::GeometryID lGraphicId = gRenderer->AddGeometry(mSphere, UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID, UiTbc::Renderer::NO_SHADOWS);
	gRenderer->TryAddGeometryTexture(lGraphicId, gTextureId[TEXTUREMAP]);

	// Setup cone forest.
	lMaterial.mDiffuse = Lepra::Vector3DF(0.2f,1,0.2f);
	mCone->SetBasicMaterialSettings(lMaterial);
	UiTbc::GeometryBatch* lConeBatch = new UiTbc::GeometryBatch(mCone);
	Lepra::TransformationF lPositions[100];
	for (int i = 0; i < 100; i++)
	{
		lPositions[i].GetPosition().x = Lepra::Random::Uniform(0.0f, 50.0f);
		lPositions[i].GetPosition().y = Lepra::Random::Uniform(0.0f, 50.0f);
		lPositions[i].GetPosition().z = 0;
	}

	Lepra::TransformationF lTransf;
	lTransf.SetPosition(Lepra::Vector3DF(0, 0, -1));

	lConeBatch->SetInstances(lPositions, Vector3DF(), 100, 0, 0.8f, 1.2f, 0.8f, 1.2f, 0.8f, 1.2f);
	lConeBatch->SetAlwaysVisible(true);
	lConeBatch->SetTransformation(lTransf);
	lConeBatch->CalculateBoundingRadius();
	UiTbc::Renderer::GeometryID lGraphicId2 = gRenderer->AddGeometry(lConeBatch, UiTbc::Renderer::MAT_SINGLE_COLOR_SOLID, UiTbc::Renderer::NO_SHADOWS);
	gRenderer->TryAddGeometryTexture(lGraphicId2, gTextureId[TEXTUREMAP]);

	// Setup torus using the Model-class.
	lMaterial.mDiffuse = Lepra::Vector3DF(1,1,1);
	mTorus->SetBasicMaterialSettings(lMaterial);
	AddUVAnimation(mTorus);
	AddRandomVertexColor(mTorus);
	int lUVSet = mTorus->AddEmptyUVSet();
	UiTbc::UVMapper::ApplyCubeMapping(mTorus, lUVSet, 1, Lepra::Vector2DD(0.5, 0.5));

	lTransf.SetIdentity();
	lTransf.SetPosition(Lepra::Vector3DF(-0.5, 2, 0));
	lTransf.RotatePitch(-Lepra::PIF / 3.0f);
	lTransf.RotateWorldZ(Lepra::PIF / 4.0f);
	lTransf.RotateWorldX(Lepra::PIF / 4.0f);
	mModel.SetTransformation(lTransf);

	// Setup a transform animation bone.
	mTransformBones = new TBC::BoneHierarchy;
	mTransformBones->SetBoneCount(1);
	mTransformBones->FinalizeInit(TBC::BoneHierarchy::TRANSFORM_NONE);

	mAnimation = new TBC::BoneAnimation;
	mAnimation->SetKeyframeCount(8, true);
	mAnimation->SetBoneCount(1);
	mAnimation->SetTimeTag(0, 0);
	mAnimation->SetTimeTag(1, 0.5f);
	mAnimation->SetTimeTag(2, 1.0f);
	mAnimation->SetTimeTag(3, 1.5f);
	mAnimation->SetTimeTag(4, 2.0f);
	mAnimation->SetTimeTag(5, 2.5f);
	mAnimation->SetTimeTag(6, 3.0f);
	mAnimation->SetTimeTag(7, 3.5f);
	mAnimation->SetTimeTag(8, 4.0f);
	mAnimation->SetBoneTransformation(0, 0, 
		Lepra::TransformationF(Lepra::g3x3IdentityMatrixF,
				       Lepra::Vector3DF(-1, -1, -1)));
	mAnimation->SetBoneTransformation(1, 0, 
		Lepra::TransformationF(Lepra::g3x3IdentityMatrixF,
				       Lepra::Vector3DF(-1, -1, 1)));
	mAnimation->SetBoneTransformation(2, 0, 
		Lepra::TransformationF(Lepra::g3x3IdentityMatrixF,
				       Lepra::Vector3DF(-1, 1, 1)));
	mAnimation->SetBoneTransformation(3, 0, 
		Lepra::TransformationF(Lepra::g3x3IdentityMatrixF,
				       Lepra::Vector3DF(-1, 1, -1)));
	mAnimation->SetBoneTransformation(4, 0, 
		Lepra::TransformationF(Lepra::g3x3IdentityMatrixF,
				       Lepra::Vector3DF(1, 1, -1)));
	mAnimation->SetBoneTransformation(5, 0, 
		Lepra::TransformationF(Lepra::g3x3IdentityMatrixF,
				       Lepra::Vector3DF(1, 1, 1)));
	mAnimation->SetBoneTransformation(6, 0, 
		Lepra::TransformationF(Lepra::g3x3IdentityMatrixF,
				       Lepra::Vector3DF(1, -1, 1)));
	mAnimation->SetBoneTransformation(7, 0, 
		Lepra::TransformationF(Lepra::g3x3IdentityMatrixF,
				       Lepra::Vector3DF(1, -1, -1)));

	mAnimator = new TBC::BoneAnimator(mTransformBones);
	mAnimator->AddAnimation(_T("TransformAnimation"), mAnimation);

	mModel.AddAnimator(_T("TransformAnimator"), mAnimator);

	mTorusHandler = new UiTbc::DefaultStaticGeometryHandler(mTorus, 1, &gTextureId[MULTIMAP], 1, UiTbc::Renderer::MAT_TEXTURE_AND_DIFFUSE_BUMPMAP_PXS, UiTbc::Renderer::CAST_SHADOWS, gRenderer);
	mModel.AddGeometry(_T("Torus"), mTorusHandler, _T("TransformAnimator"));
	mModel.SetAlwaysVisible(true);

	mModel.StartAnimation(_T("TransformAnimation"), 0, TBC::BoneAnimation::MODE_PLAY_LOOP);



	Lepra::QuaternionF lQ1;
	Lepra::QuaternionF lQ2;
	Lepra::QuaternionF lQ3;
	Lepra::QuaternionF lQ4;

	lQ1.RotateAroundWorldZ(Lepra::PIF / 5.33f);
	lQ1.RotateAroundWorldX(Lepra::PIF / 1.234f);
	lQ2.RotateAroundWorldY(Lepra::PIF / 5.33f);
	lQ2.RotateAroundWorldX(-Lepra::PIF / 1.234f);

	// Setup lights.

	lTransf.SetIdentity();
	lTransf.RotatePitch(-Lepra::PIF / 8.0f);
	lTransf.SetPosition(Lepra::Vector3DF(0, -0.5, 1));
	gRenderer->SetCameraTransformation(lTransf);

	gRenderer->SetLightsEnabled(true);
	gRenderer->SetShadowMode(UiTbc::Renderer::CAST_SHADOWS, UiTbc::Renderer::SH_VOLUMES_ONLY);
	gRenderer->SetAmbientLight(0.4f, 0.4f, 0.4f);
	gRenderer->SetViewFrustum(90, 0.01f, 300.0f);

	mLightPos.Set(-8, 10, 4);
	mLightDir.Set(1, -1, -1);
	mLightColor.Set(1, 1, 1);

	lTransf.SetIdentity();
	lTransf.SetPosition(mLightPos);
	mSphere->SetTransformation(lTransf);

	//mLightID = gRenderer->AddSpotLight(UiTbc::Renderer::LIGHT_MOVABLE, lLightPos, lLightDir, lLightColor, 30, 32, 100, 500);
	//mLightID = gRenderer->AddDirectionalLight(UiTbc::Renderer::LIGHT_MOVABLE, lLightDir, lLightColor, 500);
	mLightID = gRenderer->AddPointLight(UiTbc::Renderer::LIGHT_MOVABLE, mLightPos, Vector3DF(2.0f, 2.0f, 2.0f), 1000, 500);
}

BumpMapSceneTest::~BumpMapSceneTest()
{
	delete mTorusHandler;
	delete mAnimator;
	delete mAnimation;
	delete mTransformBones;
}

void BumpMapSceneTest::InitTerrain()
{
	TBC::GeometryBase::BasicMaterialSettings lMaterial(
		Lepra::Vector3DF(0, 0, 0),
		Lepra::Vector3DF(1.0f, 1.0f, 1.0f),
		Lepra::Vector3DF(0.1f, 0.1f, 0.1f),
		0.7f, 1.0f, true);

	TBC::TerrainFunction* lTF[3];
	lTF[0] = new TBC::TerrainConeFunction(15.0f, Lepra::Vector2DF(-50, -50), 20.0f, 30.0f);
	lTF[1] = new TBC::TerrainHemisphereFunction(-10.0f, Lepra::Vector2DF(-50, -40), 10.0f, 15.0f);
	lTF[2] = new TBC::TerrainHemisphereFunction(10.0f, Lepra::Vector2DF(100, 100), 40.0f, 50.0f);
	//lTF[1] = new TBC::TerrainDuneFunction(2, 1, 10, Lepra::Vector2DF(100, 100), 20.0f, 30.0f);
	TBC::TerrainFunctionGroup lTFGroup(lTF, 3, Lepra::COPY_REFERENCE, Lepra::TAKE_SUBDATA_OWNERSHIP);

	TBC::TerrainPatch::SetDimensions(4, 100);

	Lepra::TransformationF lTransf;
	lTransf.SetPosition(Lepra::Vector3DF(0, 0, -1));

	mTPatch[0] = new TBC::TerrainPatch(Lepra::Vector2D<int>(0, 0));
	lTFGroup.AddFunctions(*mTPatch[0]);
	mTPatch[0]->GenerateVertexNormalData();
	mTPatch[0]->SetBasicMaterialSettings(lMaterial);
	mTPatch[0]->SetAlwaysVisible(true);
	mTPatch[0]->SetTransformation(lTransf);

	const int lLevels = 2;
	int lPrevXY = 0;
	int i;
	for (i = 0; i < lLevels; i++)
	{
		int lPatchIndex = i * 8 + 1;
		int lSizeMultiplier = Lepra::Math::Pow(3, i);
		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				int lEdgeFlags = 0;
				if (i > 0)
				{
					if (y == 0)
					{
						if (x == -1)
						{
							lEdgeFlags |= TBC::TerrainPatch::EAST_EDGE;
						}
						else if (x == 1)
						{
							lEdgeFlags |= TBC::TerrainPatch::WEST_EDGE;
						}
					}
					else if (x == 0)
					{
						if (y == -1)
						{
							lEdgeFlags |= TBC::TerrainPatch::NORTH_EDGE;
						}
						else if (y == 1)
						{
							lEdgeFlags |= TBC::TerrainPatch::SOUTH_EDGE;
						}
					}
				}

				if (y != 0 || x != 0)
				{
					mTPatch[lPatchIndex] = new TBC::TerrainPatch(Lepra::Vector2D<int>(x * lSizeMultiplier + lPrevXY, y * lSizeMultiplier + lPrevXY), 0, i, lEdgeFlags);
					lTFGroup.AddFunctions(*mTPatch[lPatchIndex]);
					mTPatch[lPatchIndex]->GenerateVertexNormalData();
					mTPatch[lPatchIndex]->SetBasicMaterialSettings(lMaterial);
					mTPatch[lPatchIndex]->SetAlwaysVisible(true);
					mTPatch[lPatchIndex]->SetTransformation(lTransf);

					if ((lEdgeFlags & TBC::TerrainPatch::SOUTH_EDGE) != 0)
					{
						int lSouthIndex = -1;
						if (y == -1)
						{
							if (i == 0)
								lSouthIndex = 0;
							else
								lSouthIndex = (i - 1) * 8 + 1 + (x + 1);
						}
						else if (y == 0)
						{
							lSouthIndex = i * 8 + 1 + 6 + (x + 1);
						}

						if (lSouthIndex != -1)
						{
							mTPatch[lPatchIndex]->ShareVerticesWithSouthNeighbour(*mTPatch[lSouthIndex]);
							mTPatch[lPatchIndex]->MergeNormalsWithSouthNeighbour(*mTPatch[lSouthIndex]);
						}
					}
					if ((lEdgeFlags & TBC::TerrainPatch::NORTH_EDGE) != 0)
					{
					}
					if ((lEdgeFlags & TBC::TerrainPatch::WEST_EDGE) != 0)
					{
					}
					if ((lEdgeFlags & TBC::TerrainPatch::EAST_EDGE) != 0)
					{
					}


					lPatchIndex++;
				}
			}
		}

		lPrevXY += -lSizeMultiplier;
	}

/*
	mTPatch[0]->ShareVerticesWithNorthNeighbour(*mTPatch[3]);
	mTPatch[1]->ShareVerticesWithNorthNeighbour(*mTPatch[4]);
	mTPatch[2]->ShareVerticesWithNorthNeighbour(*mTPatch[5]);

	mTPatch[3]->ShareVerticesWithNorthNeighbour(*mTPatch[6]);
	mTPatch[4]->ShareVerticesWithNorthNeighbour(*mTPatch[7]);
	mTPatch[5]->ShareVerticesWithNorthNeighbour(*mTPatch[8]);
*/

	UiTbc::Renderer::TextureID lTexture[2];
	lTexture[0] = gTextureId[MULTIMAP];
	lTexture[1] = gTextureId[LIGHTMAP];

	for (i = 0; i < 17; i++)
	{
//		UiTbc::Renderer::MaterialType lMaterial = UiTbc::Renderer::MAT_SINGLE_TEXTURE_SOLID;
		//UiTbc::Renderer::MaterialType lMaterial = UiTbc::Renderer::MAT_TEXTURE_AND_DIFFUSE_BUMPMAP_PXS;
//		gRenderer->AddGeometry(mTPatch[i], lMaterial, lTexture, 2, UiTbc::Renderer::NO_SHADOWS);
	}
}

void BumpMapSceneTest::UpdateScene(double pTotalTime, double pDeltaTime)
{
	Lepra::Vector3DF lTempPos(mLightPos + Lepra::Vector3DF(10.0f * (float)sin(pTotalTime * 2.0), 0, 0));
	gRenderer->SetLightPosition(mLightID, lTempPos);
	Lepra::TransformationF lTransf;
	lTransf.SetPosition(lTempPos);
	mSphere->SetTransformation(lTransf);
	

	lTransf = mModel.GetTransformation();
	const float RPS = 0.125 * 0.25;
	Lepra::QuaternionF lQ;
	lQ.RotateAroundWorldZ(RPS * (float)pTotalTime * 2.0f * Lepra::PIF);
	lTransf.SetOrientation(lQ);
	mModel.SetTransformation(lTransf);
	mModel.Update(pDeltaTime * 0.1);
}

bool TestGUI(const Lepra::LogDecorator& /*pLog*/, double pShowTime)
{
	gPainter->ResetClippingRect();
	gPainter->SetRenderMode(UiTbc::Painter::RM_NORMAL);
	gPainter->PrePaint(true);

	UiTbc::DesktopWindow* lDesktopWindow = new UiTbc::DesktopWindow(gInput, gPainter, Lepra::DARK_GREEN, new UiTbc::FloatingLayout(), 0, 0);
	lDesktopWindow->SetPreferredSize(gScreen->GetWidth(), gScreen->GetHeight());
	lDesktopWindow->AddChild(new GUITestWindow);
	//lDesktopWindow->AddChild(new UiTbc::ASEFileConverter(lDesktopWindow));

	gInput->ActivateAll();

	Lepra::Timer lTotalTimer;

	bool lTestOk = true;

//	((UiTbc::SoftwarePainter*)gPainter)->SetIncrementalAlpha(true);

	do
	{
		UiLepra::Core::ProcessMessages();
		if (Lepra::SystemManager::GetQuitRequest())
		{
			CloseRenderer();
			Lepra::SystemManager::ExitProcess(0);
		}

		gInput->PollEvents();

		gScreen->SetBuffer(0);
		//gPainter->SetColor(Lepra::OFF_BLACK);
		gPainter->FillRect(0, 0, gScreen->GetWidth(), gScreen->GetHeight());
		//gFontManager->SetColor(Lepra::OFF_BLACK);
		lDesktopWindow->Repaint();
		{
			LEPRA_MEASURE_SCOPE(UpdateScreen);
			gDisplay->UpdateScreen();
		}

		lTotalTimer.UpdateTimer();
	}
	while (lTestOk && lTotalTimer.GetTimeDiff() < pShowTime);

	delete lDesktopWindow;

	return lTestOk;
}

bool TestCubicSpline(const Lepra::LogDecorator&, double pShowTime)
{
	Lepra::Vector2DF lCoordinates[4];
	lCoordinates[0].Set(0, 0);
	lCoordinates[1].Set(10, 500);
	lCoordinates[2].Set(400, 550);
	lCoordinates[3].Set(400, 0);

	float lTimeTags[10] = {0, 1, 2, 3, 4};
	Lepra::CubicDeCasteljauSpline<Lepra::Vector2DF, float> lSpline(lCoordinates, lTimeTags, 4, Lepra::CubicDeCasteljauSpline<Lepra::Vector2DF, float>::TYPE_BEZIER);

	gPainter->ResetClippingRect();
	gPainter->SetRenderMode(UiTbc::Painter::RM_NORMAL);

	gPainter->SetColor(Lepra::BLUE);
	gPainter->FillRect(0, 0, gPainter->GetCanvas()->GetWidth(), gPainter->GetCanvas()->GetHeight());

	float lStep = 4.0f / (float)gPainter->GetCanvas()->GetWidth();
	lSpline.StartInterpolation(0);
	for (unsigned int i = 0; i < gPainter->GetCanvas()->GetWidth(); i++)
	{
		Lepra::Vector2DF lValue = lSpline.GetValue();
		float x = lValue.x;
		float y = lValue.y;
		
		gPainter->SetColor(Lepra::WHITE);
		gPainter->DrawPixel((int)x, (int)y);
	
		lSpline.StepInterpolation(lStep);
		gDisplay->UpdateScreen();
	}

	Lepra::Timer lTimer;
	while (lTimer.GetTimeDiff() < pShowTime && gInput->ReadKey(UiLepra::InputManager::IN_KBD_ESC) == false)
	{
		lTimer.UpdateTimer();
		if (Lepra::SystemManager::GetQuitRequest())
		{
			CloseRenderer();
			Lepra::SystemManager::ExitProcess(0);
		}
		Lepra::Thread::Sleep(0.01f);
	}

	return true;
}

void TestPainter()
{
	gPainter->ResetClippingRect();
	UiTbc::Painter::DisplayListID lDispId = gPainter->NewDisplayList();

	gPainter->BeginDisplayList(lDispId);

		gPainter->SetRenderMode(UiTbc::Painter::RM_NORMAL);
		gPainter->SetColor(Lepra::BLUE);
		gPainter->FillRect(0, 0, gScreen->GetWidth(), gScreen->GetHeight());

		gPainter->SetColor(Lepra::WHITE);
		gPainter->DrawLine(0, 0, 0, gScreen->GetHeight());
		gPainter->DrawLine(gScreen->GetWidth() - 1, 0, gScreen->GetWidth() - 1, gScreen->GetHeight());
		gPainter->DrawLine(0, 0, gScreen->GetWidth(), 0);
		gPainter->DrawLine(0, gScreen->GetHeight() - 1, gScreen->GetWidth(), gScreen->GetHeight() - 1);

		gPainter->SetColor(Lepra::GRAY);
		gPainter->FillRect(100, 100, 200, 300);

		gPainter->SetColor(Lepra::PINK, 0);
		gPainter->SetColor(Lepra::RED, 1);
		gPainter->Draw3DRect(100, 100, 200, 300, 1, false);
	
	gPainter->EndDisplayList();

	while(!Lepra::SystemManager::GetQuitRequest())
	{
		UiLepra::Core::ProcessMessages();
		gInput->PollEvents();

		gPainter->RenderDisplayList(lDispId);

		gDisplay->UpdateScreen();
	}

	gPainter->DeleteDisplayList(lDispId);
}

bool TestUiTbc()
{
	//RunRotationalAgreementTest();
	/*OpenRenderer(gUiTbcLog, UiLepra::DisplayManager::OPENGL_CONTEXT);
	TestPainter();
	CloseRenderer();*/
	

	bool lTestOk = true;
#ifdef TEST_D3D_RENDERER
	for (unsigned y = 0; y < 2; ++y)
#else // TEST_D3D_RENDERER
	for (unsigned y = 0; y < 1; ++y)
#endif // TEST_D3D_RENDERER/!TEST_D3D_RENDERER
	{
		LEPRA_MEASURE_SCOPE(Graphics);
		if (lTestOk)
		{
			LEPRA_MEASURE_SCOPE(OpenRenderer);
			if (y == 0)
			{
				lTestOk = OpenRenderer(gUiTbcLog, UiLepra::DisplayManager::OPENGL_CONTEXT);
			}
			else
			{
				assert(false);
			}
			assert(lTestOk);
		}
		/*//TestOpenGLPainter();
		if (lTestOk)
		{
			LEPRA_MEASURE_SCOPE(GenerateTest);
			lTestOk = TestGenerate(gUiTbcLog, 2.0);
		}*/
		if (lTestOk)
		{
			LEPRA_MEASURE_SCOPE(MaterialTest);
			lTestOk = TestMaterials(gUiTbcLog, 20.0);
		}
		/*if (lTestOk)
		{
			lTestOk = TestFps(gUiTbcLog, 150.0);
		}
		if (lTestOk)
		{
			lTestOk = TestSkinningSaveLoad(gUiTbcLog, 2.0);
		}
		if (lTestOk)
		{
			lTestOk = TestMeshImport(gUiTbcLog, 4.0);
		}
		if (lTestOk)
		{
			lTestOk = TestLoadClass(gUiTbcLog);
		}
		if (lTestOk)
		{
			lTestOk = TestCubicSpline(gUiTbcLog, 1.0);
		}
		if (lTestOk)
		{
			GeometryReferenceTest lTest(gUiTbcLog);
			lTestOk = lTest.Run(10.0f);
		}
		if (lTestOk && y == 0)
		{
			TerrainFunctionTest lTest(gUiTbcLog);
			lTestOk = lTest.Run(3.0f);
		}
		if (lTestOk && y == 0)
		{
			LEPRA_MEASURE_SCOPE(CompleteScene);
			BumpMapSceneTest lTest(gUiTbcLog);
			lTestOk = lTest.Run(3.0f);
		}
		if (lTestOk)
		{
			LEPRA_MEASURE_SCOPE(GuiTest);
			lTestOk = TestGUI(gUiTbcLog, 3.0);
		}

		{
			LEPRA_MEASURE_SCOPE(CloseRenderer);
			lTestOk = CloseRenderer();
		}

		if (lTestOk)
		{
			UiTbc::TriangleBasedGeometry lMesh;
			bool lCastShadows = false;
			DiskFile lFile;
			lTestOk = lFile.Open(_T("Data/road_sign_01_sign.mesh"), DiskFile::MODE_READ);
			assert(lTestOk);
			if (lTestOk)
			{
				UiTbc::ChunkyMeshLoader lLoader(&lFile, false);
				lTestOk = lLoader.Load(&lMesh, lCastShadows);
				assert(lTestOk);
				if (lTestOk)
				{
					lTestOk = (lMesh.GetUVSetCount() == 1);
					assert(lTestOk);
				}
			}
		}*/
	}
	return (lTestOk);
}

#endif //!CURE_TEST_WITHOUT_UI
