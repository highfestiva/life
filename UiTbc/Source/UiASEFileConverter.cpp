/*
	Class:  ASEFileConverter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "pch.h"
#include "../Include/UiASEFileConverter.h"
#include "../Include/UiASELoader.h"
#include "../Include/UiPainter.h"
#include "../Include/UiMouseTheme.h"
#include "../Include/GUI/UiButton.h"
#include "../Include/GUI/UiCaption.h"
#include "../Include/GUI/UiDesktopWindow.h"
#include "../Include/GUI/UiFileNameField.h"
#include "../Include/GUI/UiGridLayout.h"
#include "../Include/GUI/UiRectComponent.h"
#include "../Include/GUI/UiTreeNode.h"
#include "../../Lepra/Include/Graphics2D.h"

namespace UiTbc
{

ASEFileConverter::ASEFileConverter(DesktopWindow* pDesktopWindow) :
	Window(BORDER_LINEARSHADING, 4, Lepra::LIGHT_GRAY, Lepra::String("ASEFileConverter"), new GridLayout(4, 1)),
	mFileNameField(0)
{
	Painter* lPainter = pDesktopWindow->GetInternalPainter();
	UiTbc::Caption* lCaption = new UiTbc::Caption(Lepra::LIGHT_BLUE, Lepra::DARK_BLUE, Lepra::LIGHT_BLUE, Lepra::DARK_BLUE,
		Lepra::LIGHT_GRAY, Lepra::GRAY, Lepra::LIGHT_GRAY, Lepra::GRAY, 20);
	lCaption->SetText(Lepra::String("Select an ASE file to convert"), lPainter->GetStandardFont(0),
		Lepra::WHITE, Lepra::BLACK, Lepra::BLACK, Lepra::BLACK);
	Window::SetCaption(lCaption);

	// The top rect creates some space between the caption and the filename field.
	RectComponent* lTopRect = new RectComponent(Lepra::String("TopRect"));
	Window::AddChild(lTopRect);

	mFileNameField = new FileNameField(pDesktopWindow, BORDER_SUNKEN | BORDER_LINEARSHADING, 3, Lepra::WHITE, Lepra::String("ASEFileNameField"));
	mFileNameField->SetFont(lPainter->GetStandardFont(0), Lepra::BLACK, Component::ALPHATEST, 128);
	mFileNameField->SetPreferredHeight(24);
	mFileNameField->SetMinSize(0, 20);
	mFileNameField->AddFileExtension("ase");
	Window::AddChild(mFileNameField);

	RectComponent* lMidRect = new RectComponent(Lepra::String("MidRect"));
	Window::AddChild(lMidRect);

	RectComponent* lBottomRect = new RectComponent(Lepra::String("BottomRect"), new GridLayout(1, 5));
	lBottomRect->AddChild(new RectComponent(Lepra::String("LeftBottomRect")));

	Button* lCancelButton = new Button(BorderComponent::LINEAR, 4, Lepra::LIGHT_GRAY, Lepra::String("CancelButton"));
	lCancelButton->SetText(Lepra::String("Cancel"), lPainter->GetStandardFont(0), Lepra::BLACK, Lepra::BLACK);
	lCancelButton->SetPreferredSize(100, 20);
	lCancelButton->SetMinSize(20, 20);
	lCancelButton->SetOnUnclickedFunc(ASEFileConverter, OnCancel);
	lBottomRect->AddChild(lCancelButton);

	lBottomRect->AddChild(new RectComponent(Lepra::String("MidBottomRect")));

	Button* lOkButton = new Button(BorderComponent::LINEAR, 4, Lepra::LIGHT_GRAY, Lepra::String("OkButton"));
	lOkButton->SetText(Lepra::String("Ok"), lPainter->GetStandardFont(0), Lepra::BLACK, Lepra::BLACK);
	lOkButton->SetPreferredSize(100, 20);
	lOkButton->SetMinSize(20, 20);
	lOkButton->SetOnUnclickedFunc(ASEFileConverter, OnOk);
	lBottomRect->AddChild(lOkButton);

	lBottomRect->AddChild(new RectComponent(Lepra::String("RightBottomRect")));

	lBottomRect->SetPreferredHeight(30);
	Window::AddChild(lBottomRect);

	SetPreferredSize(500, 100);
}

ASEFileConverter::~ASEFileConverter()
{
}

void ASEFileConverter::OnOk(UiTbc::Button*)
{
	UiTbc::DesktopWindow* lDesktopWindow = (UiTbc::DesktopWindow*)GetParentOfType(UiTbc::Component::DESKTOPWINDOW);

	lDesktopWindow->AddChild(new ASEViewer(lDesktopWindow, mFileNameField->GetText()));
	lDesktopWindow->DeleteComponent(this, 0);
}

void ASEFileConverter::OnCancel(UiTbc::Button*)
{
	UiTbc::DesktopWindow* lDesktopWindow = (UiTbc::DesktopWindow*)GetParentOfType(UiTbc::Component::DESKTOPWINDOW);
	lDesktopWindow->DeleteComponent(this, 0);
}





ASEFileConverter::ASEViewer::ASEViewer(DesktopWindow* pDesktopWindow, const Lepra::String& pASEFile) :
	Window(BORDER_LINEARSHADING | BORDER_RESIZABLE, 4, Lepra::LIGHT_GRAY, Lepra::String("ASEViewer"), new GridLayout(2, 1))
{
	pDesktopWindow;
	pASEFile;

	pDesktopWindow->GetMouseTheme()->LoadBusyCursor();
	Painter* lPainter = pDesktopWindow->GetInternalPainter();

	UiTbc::Caption* lCaption = new UiTbc::Caption(Lepra::LIGHT_BLUE, Lepra::DARK_BLUE, Lepra::LIGHT_BLUE, Lepra::DARK_BLUE,
		Lepra::LIGHT_GRAY, Lepra::GRAY, Lepra::LIGHT_GRAY, Lepra::GRAY, 20);
	lCaption->SetText(Lepra::String("ASEViewer - ") + pASEFile, lPainter->GetStandardFont(0),
		Lepra::WHITE, Lepra::BLACK, Lepra::BLACK, Lepra::BLACK);
	Window::SetCaption(lCaption);

	ListControl* lTreeView = new ListControl(BORDER_SUNKEN, 3, Lepra::WHITE);
	Window::AddChild(lTreeView);

	ASELoader lLoader;
	ASELoader::ASEData lASEData;

	if (lLoader.Load(lASEData, pASEFile) == true)
	{
		TreeNode* l3DSMaxAsciiExportNode = new TreeNode(Lepra::StringUtility::Format("3DSMaxAsciiExport: %i", lASEData.m3DSMaxAsciiExport), Lepra::String("3DSMaxAsciiExportNode"));
		lTreeView->AddChild(l3DSMaxAsciiExportNode);

		lTreeView->AddChild(CreateSceneNode(lASEData.mScene));
		lTreeView->AddChild(CreateCamerasNode(lASEData.mCamList));
		lTreeView->AddChild(CreateLightsNode(lASEData.mLightList));
		lTreeView->AddChild(CreateGeometriesNode(lASEData.mGeomList));
		lTreeView->AddChild(CreateMaterialsNode(lASEData.mMaterialList));
	}
	else
	{
		// TODO: Create a message box.
	}

	SetPreferredSize(300, 400);
	pDesktopWindow->GetMouseTheme()->LoadArrowCursor();
}

ASEFileConverter::ASEViewer::~ASEViewer()
{
}

TreeNode* ASEFileConverter::ASEViewer::CreateSceneNode(const ASELoader::Scene* pScene)
{
	TreeNode* lSceneNode = new TreeNode("Scene", Lepra::String("SceneNode"));
	if (pScene != 0)
	{
		lSceneNode->AddChildNode(new TreeNode(Lepra::String("Comment:") + pScene->mComment.ToCurrentCode(), Lepra::String("CommentNode")));
		lSceneNode->AddChildNode(new TreeNode(Lepra::String("Filename: ") + pScene->mFilename.ToCurrentCode(), Lepra::String("FilenameNode")));
		lSceneNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("FirstFrame: %i", pScene->mFirstFrame), Lepra::String("FirstFrameNode")));
		lSceneNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("LastFrame: %i", pScene->mLastFrame), Lepra::String("LastFrameNode")));
		lSceneNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("FrameSpeed: %i", pScene->mFrameSpeed), Lepra::String("FrameSpeedNode")));
		lSceneNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TicksPerFrame: %i", pScene->mTicksPerFrame), Lepra::String("TicksPerFrameNode")));
		lSceneNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("BackgroundStatic: [%f, %f, %f]", pScene->mBackgroundStatic[0], pScene->mBackgroundStatic[1], pScene->mBackgroundStatic[2]), Lepra::String("BackgroundStaticNode")));
		lSceneNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("AmbientStatic: [%f, %f, %f]", pScene->mAmbientStatic[0], pScene->mAmbientStatic[1], pScene->mAmbientStatic[2]), Lepra::String("AmbientStaticNode")));
	}

	return lSceneNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateCamerasNode(const ASELoader::ASEData::CameraObjectList& pCameraList)
{
	TreeNode* lCamerasNode = new TreeNode("Cameras", Lepra::String("CamerasNode"));

	Lepra::String lFormatString("Camera%.");
	lFormatString += (Lepra::int64)GetNumDigits(pCameraList.GetCount());
	lFormatString += "i";

	ASELoader::ASEData::CameraObjectList::ConstIterator lCamIter;
	int lCount = 1;
	for (lCamIter = pCameraList.First(); lCamIter != pCameraList.End(); ++lCamIter, ++lCount)
	{
		TreeNode* lCameraNode = new TreeNode(Lepra::StringUtility::Format(lFormatString.c_str(), lCount), Lepra::String("CameraNode"));
		ASELoader::CameraObject* lCam = *lCamIter;
		
		lCameraNode->AddChildNode(new TreeNode(Lepra::String("CameraType: ") + lCam->mCameraType.ToCurrentCode(), Lepra::String("CameraTypeNode")));
		lCameraNode->AddChildNode(new TreeNode(Lepra::String("Comment: ") + lCam->mCameraType.ToCurrentCode(), Lepra::String("CommentNode")));
		lCameraNode->AddChildNode(new TreeNode(Lepra::String("NodeName: ") + lCam->mNodeName.ToCurrentCode(), Lepra::String("NodeNameNode")));
		
		lCameraNode->AddChildNode(CreateTMListNode(lCam->mNodeTMList));
		lCameraNode->AddChildNode(CreateTMAnimationNode(*lCam->mTMAnimation));

		if (lCam->mSettings != 0)
		{
			TreeNode* lSettingsNode = new TreeNode(Lepra::String("Settings"), Lepra::String("SettingsNode"));

			lSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("FOV: %f", lCam->mSettings->mFOV), Lepra::String("FOVNode")));
			lSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("Near: %f", lCam->mSettings->mNear), Lepra::String("NearNode")));
			lSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("Far: %f", lCam->mSettings->mFar), Lepra::String("FarNode")));
			lSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TDist: %f", lCam->mSettings->mTDist), Lepra::String("TDistNode")));
			lSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TimeValue: %i", lCam->mSettings->mTimeValue), Lepra::String("TimeValueNode")));

			lCameraNode->AddChildNode(lSettingsNode);
		}

		lCamerasNode->AddChildNode(lCameraNode);
	}

	return lCamerasNode;
}


TreeNode* ASEFileConverter::ASEViewer::CreateLightsNode(const ASELoader::ASEData::LightObjectList& pLightList)
{
	TreeNode* lLightsNode = new TreeNode("Lights", Lepra::String("LightsNode"));

	Lepra::String lFormatString("Light%.");
	lFormatString += (Lepra::int64)GetNumDigits(pLightList.GetCount());
	lFormatString += "i";

	ASELoader::ASEData::LightObjectList::ConstIterator lLightIter;
	int lCount = 1;
	for (lLightIter = pLightList.First(); lLightIter != pLightList.End(); ++lLightIter, ++lCount)
	{
		TreeNode* lLightNode = new TreeNode(Lepra::StringUtility::Format(lFormatString.c_str(), lCount), Lepra::String("LightNode"));
		ASELoader::LightObject* lLight = *lLightIter;
		lLightNode->AddChildNode(new TreeNode(Lepra::String("NodeName: ") + lLight->mNodeName.ToCurrentCode(), Lepra::String("NodeNameNode")));
		lLightNode->AddChildNode(new TreeNode(Lepra::String("Comment: ") + lLight->mComment.ToCurrentCode(), Lepra::String("CommentNode")));
		lLightNode->AddChildNode(new TreeNode(Lepra::String("LightType: ") + lLight->mLightType.ToCurrentCode(), Lepra::String("LightTypeNode")));
		lLightNode->AddChildNode(new TreeNode(Lepra::String("Shadows: ") + lLight->mShadows.ToCurrentCode(), Lepra::String("ShadowsNode")));
		lLightNode->AddChildNode(new TreeNode(Lepra::String("SpotShape: ") + lLight->mSpotShape.ToCurrentCode(), Lepra::String("SpotShapeNode")));

		lLightNode->AddChildNode(new TreeNode(Lepra::String("UseLight: ") + ToString(lLight->mUseLight), Lepra::String("UseLightNode")));
		lLightNode->AddChildNode(new TreeNode(Lepra::String("UseGlobal: ") + ToString(lLight->mUseGlobal), Lepra::String("UseGlobalNode")));
		lLightNode->AddChildNode(new TreeNode(Lepra::String("AbsMapBias: ") + ToString(lLight->mAbsMapBias), Lepra::String("AbsMapBiasNode")));
		lLightNode->AddChildNode(new TreeNode(Lepra::String("OverShoot: ") + ToString(lLight->mOverShoot), Lepra::String("OverShootNode")));

		lLightNode->AddChildNode(CreateTMListNode(lLight->mNodeTMList));
		lLightNode->AddChildNode(CreateTMAnimationNode(*lLight->mTMAnimation));

		lLightNode->AddChildNode(CreateLightSettingsNode(lLight->mSettings));

		lLightsNode->AddChildNode(lLightNode);
	}

	return lLightsNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateGeometriesNode(const ASELoader::ASEData::GeomObjectList& pGeometryList)
{
	TreeNode* lGeometriesNode = new TreeNode("Geometries", Lepra::String("GeometriesNode"));

	Lepra::String lFormatString("Geometry%.");
	lFormatString += (Lepra::int64)GetNumDigits(pGeometryList.GetCount());
	lFormatString += "i";

	ASELoader::ASEData::GeomObjectList::ConstIterator lGeomIter;
	int lCount = 1;
	for (lGeomIter = pGeometryList.First(); lGeomIter != pGeometryList.End(); ++lGeomIter, ++lCount)
	{
		TreeNode* lGeometryNode = new TreeNode(Lepra::StringUtility::Format(lFormatString.c_str(), lCount), Lepra::String("GeometryNode"));
		ASELoader::GeomObject* lGeom = *lGeomIter;

		lGeometryNode->AddChildNode(new TreeNode(Lepra::String("NodeName: ") + lGeom->mNodeName.ToCurrentCode(), Lepra::String("NodeNameNode")));
		lGeometryNode->AddChildNode(new TreeNode(Lepra::String("Comment: ") + lGeom->mComment.ToCurrentCode(), Lepra::String("CommentNode")));
		lGeometryNode->AddChildNode(new TreeNode(Lepra::String("MotionBlur: ") + ToString(lGeom->mMotionBlur), Lepra::String("MotionBlurNode")));
		lGeometryNode->AddChildNode(new TreeNode(Lepra::String("CastShadow: ") + ToString(lGeom->mCastShadow), Lepra::String("CastShadowNode")));
		lGeometryNode->AddChildNode(new TreeNode(Lepra::String("RecvShadow: ") + ToString(lGeom->mRecvShadow), Lepra::String("RecvShadowNode")));

		lGeometryNode->AddChildNode(new TreeNode(Lepra::String("MaterialRef: ") + (Lepra::int64)lGeom->mMaterialRef, Lepra::String("MaterialRefNode")));
		lGeometryNode->AddChildNode(CreateTMListNode(lGeom->mNodeTMList));
		lGeometryNode->AddChildNode(CreateTMAnimationNode(*lGeom->mTMAnimation));
		lGeometryNode->AddChildNode(CreateMeshListNode(lGeom->mMeshList));

		lGeometriesNode->AddChildNode(lGeometryNode);
	}

	return lGeometriesNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateMaterialsNode(const ASELoader::ASEData::MaterialList& pMaterialList)
{
	TreeNode* lMaterialsNode = new TreeNode("Materials", Lepra::String("MaterialsNode"));

	Lepra::String lFormatString("Material%.");
	lFormatString += (Lepra::int64)GetNumDigits(pMaterialList.GetCount());
	lFormatString += "i";

	ASELoader::ASEData::MaterialList::ConstIterator lMatIter;
	int lCount = 1;
	for (lMatIter = pMaterialList.First(); lMatIter != pMaterialList.End(); ++lMatIter, ++lCount)
	{
		TreeNode* lMaterialNode = new TreeNode(Lepra::StringUtility::Format(lFormatString.c_str(), lCount), Lepra::String("MaterialNode"));
		ASELoader::Material* lMat = *lMatIter;

		lMaterialNode->AddChildNode(new TreeNode(Lepra::String("Name: ") + lMat->mName.ToCurrentCode(), Lepra::String("NameNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::String("Class: ") + lMat->mClass.ToCurrentCode(), Lepra::String("ClassNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::String("Comment: ") + lMat->mComment.ToCurrentCode(), Lepra::String("CommentNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::String("Shading: ") + lMat->mShading.ToCurrentCode(), Lepra::String("ShadingNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::String("Falloff: ") + lMat->mFalloff.ToCurrentCode(), Lepra::String("FalloffNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::String("XPType: ") + lMat->mXPType.ToCurrentCode(), Lepra::String("XPTypeNode")));

		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("Ambient: [%f, %f, %f]", lMat->mAmbient[0], lMat->mAmbient[1], lMat->mAmbient[2]), Lepra::String("AmbientNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("Diffuse: [%f, %f, %f]", lMat->mDiffuse[0], lMat->mDiffuse[1], lMat->mDiffuse[2]), Lepra::String("DiffuseNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("Specular: [%f, %f, %f]", lMat->mSpecular[0], lMat->mSpecular[1], lMat->mSpecular[2]), Lepra::String("SpecularNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("Shine: %f", lMat->mShine), Lepra::String("ShineNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("ShineStrength: %f", lMat->mShineStrength), Lepra::String("ShineStrengthNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("Transparency: %f", lMat->mTransparency), Lepra::String("TransparencyNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("WireSize: %f", lMat->mWireSize), Lepra::String("WireSizeNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("XPFalloff: %f", lMat->mXPFalloff), Lepra::String("XPFalloffNode")));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("SelfIllum: %f", lMat->mSelfIllum), Lepra::String("SelfIllumNode")));

		lMaterialNode->AddChildNode(CreateSubMaterialListNode(lMat->mSubMaterialList));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mAmbientMapList, Lepra::String("AmbientMapList")));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mDiffuseMapList, Lepra::String("DiffuseMapList")));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mSpecularMapList, Lepra::String("SpecularMapList")));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mShineMapList, Lepra::String("ShineMapList")));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mShineStrengthMapList, Lepra::String("ShineStrengthMapList")));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mSelfIllumMapList, Lepra::String("SelfIllumMapList")));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mOpacityMapList, Lepra::String("OpacityMapList")));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mFilterColorMapList, Lepra::String("FilterColorMapList")));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mBumpMapList, Lepra::String("BumpMapList")));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mReflectMapList, Lepra::String("ReflectMapList")));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mRefractMapList, Lepra::String("RefractMapList")));

		lMaterialsNode->AddChildNode(lMaterialNode);
	}

	return lMaterialsNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateTMListNode(const std::list<ASELoader::NodeTM*>& pNodeTMList)
{
	TreeNode* lNodeTMListNode = new TreeNode(Lepra::String("NodeTMList"), Lepra::String("NodeTMListNode"));

	Lepra::String lFormatString("NodeTM%.");
	lFormatString += (Lepra::int64)GetNumDigits(pNodeTMList.GetCount());
	lFormatString += "i";

	int lCount;
	std::list<ASELoader::NodeTM*>::ConstIterator lIter;
	for (lIter = pNodeTMList.First(), lCount = 1; lIter != pNodeTMList.End(); ++lIter, ++lCount)
	{
		TreeNode* lNodeTMNode = new TreeNode(Lepra::StringUtility::Format(lFormatString.c_str(), lCount), Lepra::String("NodeTMNode"));

		ASELoader::NodeTM* lNodeTM = *lIter;
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::String("NodeName: ") + lNodeTM->mNodeName.ToCurrentCode(), Lepra::String("NodeNameNode")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::String("Comment: ")  + lNodeTM->mComment.ToCurrentCode(), Lepra::String("CommentNode")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("InheritPos: [%i, %i, %i]", lNodeTM->mInheritPos[0], lNodeTM->mInheritPos[1], lNodeTM->mInheritPos[2]), Lepra::String("InheritPosNode")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("InheritRot: [%i, %i, %i]", lNodeTM->mInheritRot[0], lNodeTM->mInheritRot[1], lNodeTM->mInheritRot[2]), Lepra::String("InheritRotNode")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("InheritScl: [%i, %i, %i]", lNodeTM->mInheritScl[0], lNodeTM->mInheritScl[1], lNodeTM->mInheritScl[2]), Lepra::String("InheritSclNode")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TMPos: [%f, %f, %f]", lNodeTM->mTMPos[0], lNodeTM->mTMPos[1], lNodeTM->mTMPos[2]), Lepra::String("TMPosNode")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TMRotAngle: %f", lNodeTM->mTMRotAngle), Lepra::String("TMRotAngleNode")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TMRotAxis: [%f, %f, %f]", lNodeTM->mTMRotAxis[0], lNodeTM->mTMRotAxis[1], lNodeTM->mTMRotAxis[2]), Lepra::String("TMRotAxisNode")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TMRow0: [%f, %f, %f]", lNodeTM->mTMRow0[0], lNodeTM->mTMRow0[1], lNodeTM->mTMRow0[2]), Lepra::String("TMRow0Node")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TMRow1: [%f, %f, %f]", lNodeTM->mTMRow1[0], lNodeTM->mTMRow1[1], lNodeTM->mTMRow1[2]), Lepra::String("TMRow1Node")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TMRow2: [%f, %f, %f]", lNodeTM->mTMRow2[0], lNodeTM->mTMRow2[1], lNodeTM->mTMRow2[2]), Lepra::String("TMRow2Node")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TMRow3: [%f, %f, %f]", lNodeTM->mTMRow3[0], lNodeTM->mTMRow3[1], lNodeTM->mTMRow3[2]), Lepra::String("TMRow3Node")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TMScale: [%f, %f, %f]", lNodeTM->mTMScale[0], lNodeTM->mTMScale[1], lNodeTM->mTMScale[2]), Lepra::String("TMScaleNode")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TMScaleAxis: [%f, %f, %f]", lNodeTM->mTMScaleAxis[0], lNodeTM->mTMScaleAxis[1], lNodeTM->mTMScaleAxis[2]), Lepra::String("TMScaleAxisNode")));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TMScaleAxisAng: %f", lNodeTM->mTMScaleAxisAng), Lepra::String("TMScaleAxisAngNode")));

		lNodeTMListNode->AddChildNode(lNodeTMNode);
	}

	return lNodeTMListNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateTMAnimationNode(const ASELoader::TMAnimation& pTMAnimation)
{
	TreeNode* lTMAnimationNode = new TreeNode(Lepra::String("TMAnimation"), Lepra::String("TMAnimationNode"));

	lTMAnimationNode->AddChildNode(new TreeNode(Lepra::String("NodeName: ") + pTMAnimation.mNodeName.ToCurrentCode(), Lepra::String("NodeNameNode")));
	lTMAnimationNode->AddChildNode(new TreeNode(Lepra::String("Comment: ") + pTMAnimation.mComment.ToCurrentCode(), Lepra::String("CommentNode")));

	TreeNode* lControlPosTrackNode = new TreeNode(Lepra::String("ControlPosTrack"), Lepra::String("ControlPosTrackNode"));
	TreeNode* lControlRotTrackNode = new TreeNode(Lepra::String("ControlRotTrack"), Lepra::String("ControlRotTrackNode"));

	Lepra::String lFormatString("ControlPosSample%.");
	lFormatString += (Lepra::int64)GetNumDigits(pTMAnimation.mControlPosTrack.GetCount());
	lFormatString += "i";

	int lCount;
	ASELoader::TMAnimation::ControlPosSampleList::ConstIterator lPosIter;
	for(lPosIter = pTMAnimation.mControlPosTrack.First(), lCount = 1; lPosIter != pTMAnimation.mControlPosTrack.End(); ++lPosIter, ++lCount)
	{
		Lepra::String lName(Lepra::StringUtility::Format(lFormatString.c_str(), lCount));
		lControlPosTrackNode->AddChildNode(new TreeNode(lName, Lepra::String(lName + "Node")));
	}

	lFormatString = "ControlRotSample%.";
	lFormatString += (Lepra::int64)GetNumDigits(pTMAnimation.mControlPosTrack.GetCount());
	lFormatString += "i";

	ASELoader::TMAnimation::ControlRotSampleList::ConstIterator lRotIter;
	for(lRotIter = pTMAnimation.mControlRotTrack.First(), lCount = 1; lRotIter != pTMAnimation.mControlRotTrack.End(); ++lRotIter, ++lCount)
	{
		Lepra::String lName(Lepra::StringUtility::Format(lFormatString.c_str(), lCount));
		lControlRotTrackNode->AddChildNode(new TreeNode(lName, Lepra::String(lName + "Node")));
	}

	lTMAnimationNode->AddChildNode(lControlPosTrackNode);
	lTMAnimationNode->AddChildNode(lControlRotTrackNode);

	return lTMAnimationNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateLightSettingsNode(const ASELoader::LightSettings* pLightSettings)
{
	TreeNode* lLightSettingsNode = new TreeNode(Lepra::String("LightSettings"), Lepra::String("LightSettingsNode"));

	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TimeValue: %i", pLightSettings->mTimeValue), Lepra::String("TimeValueNode")));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("Color: [%f, %f, %f]", pLightSettings->mColor[0], pLightSettings->mColor[1], pLightSettings->mColor[2]), Lepra::String("ColorNode")));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("Intens:   %f", pLightSettings->mIntens), Lepra::String("IntensNode")));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("Aspect:   %f", pLightSettings->mAspect), Lepra::String("AspectNode")));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("HotSpot:  %f", pLightSettings->mHotSpot), Lepra::String("HotSpotNode")));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("Falloff:  %f", pLightSettings->mFalloff), Lepra::String("FalloffNode")));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("TDist:    %f", pLightSettings->mTDist), Lepra::String("TDistNode")));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("MapBias:  %f", pLightSettings->mMapBias), Lepra::String("MapBiasNode")));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("MapRange: %f", pLightSettings->mMapRange), Lepra::String("MapRangeNode")));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("MapSize:  %f", pLightSettings->mMapSize), Lepra::String("MapSizeNode")));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format("RayBias:  %f", pLightSettings->mRayBias), Lepra::String("RayBiasNode")));
	
	return lLightSettingsNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateMeshListNode(const ASELoader::GeomObject::MeshList& pMeshList)
{
	TreeNode* lMeshListNode = new TreeNode(Lepra::String("MeshList"), Lepra::String("MeshListNode"));

	Lepra::String lFormatString("Mesh%.");
	lFormatString += (Lepra::int64)GetNumDigits(pMeshList.GetCount());
	lFormatString += "i";

	ASELoader::GeomObject::MeshList::ConstIterator lIter;
	int lCount = 1;
	for (lIter = pMeshList.First(); lIter != pMeshList.End(); ++lIter, ++lCount)
	{
		TreeNode* lMeshNode = new TreeNode(Lepra::StringUtility::Format(lFormatString.c_str(), lCount), Lepra::String("MeshNode"));
		//ASELoader::Mesh* lMesh = *lIter;

		//lMesh->

		lMeshListNode->AddChildNode(lMeshNode);
	}

	return lMeshListNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateSubMaterialListNode(const ASELoader::Material::MaterialList& pSubMatList)
{
	TreeNode* lSubMaterialListNode = new TreeNode(Lepra::String("SubMaterialList"), Lepra::String("SubMaterialListNode"));

	pSubMatList;

	return lSubMaterialListNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateMapListNode(const ASELoader::Material::MapList& pMapList, const Lepra::String& pNodeName)
{
	TreeNode* lMapListNode = new TreeNode(pNodeName, Lepra::String("MapListNode"));

	pMapList;

	return lMapListNode;
}

int ASEFileConverter::ASEViewer::GetNumDigits(int pValue)
{
	int lNumDigits = 0;
	do
	{
		lNumDigits++;
		pValue /= 10;
	} while (pValue != 0);

	return lNumDigits;
}

Lepra::String ASEFileConverter::ASEViewer::ToString(bool pValue)
{
	if (pValue)
	{
		return Lepra::String("true");
	}
	else
	{
		return Lepra::String("false");
	}
}

} // End namespace.
