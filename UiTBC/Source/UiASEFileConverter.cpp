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
	Window(BORDER_LINEARSHADING, 4, Lepra::LIGHT_GRAY, Lepra::String(_T("ASEFileConverter")), new GridLayout(4, 1)),
	mFileNameField(0)
{
	Painter* lPainter = pDesktopWindow->GetInternalPainter();
	UiTbc::Caption* lCaption = new UiTbc::Caption(Lepra::LIGHT_BLUE, Lepra::DARK_BLUE, Lepra::LIGHT_BLUE, Lepra::DARK_BLUE,
		Lepra::LIGHT_GRAY, Lepra::GRAY, Lepra::LIGHT_GRAY, Lepra::GRAY, 20);
	lCaption->SetText(Lepra::String(_T("Select an ASE file to convert")), lPainter->GetStandardFont(0),
		Lepra::WHITE, Lepra::BLACK, Lepra::BLACK, Lepra::BLACK);
	Window::SetCaption(lCaption);

	// The top rect creates some space between the caption and the filename field.
	RectComponent* lTopRect = new RectComponent(Lepra::String(_T("TopRect")));
	Window::AddChild(lTopRect);

	mFileNameField = new FileNameField(pDesktopWindow, BORDER_SUNKEN | BORDER_LINEARSHADING, 3, Lepra::WHITE, Lepra::String(_T("ASEFileNameField")));
	mFileNameField->SetFont(lPainter->GetStandardFont(0), Lepra::BLACK, Component::ALPHATEST, 128);
	mFileNameField->SetPreferredHeight(24);
	mFileNameField->SetMinSize(0, 20);
	mFileNameField->AddFileExtension(_T("ase"));
	Window::AddChild(mFileNameField);

	RectComponent* lMidRect = new RectComponent(Lepra::String(_T("MidRect")));
	Window::AddChild(lMidRect);

	RectComponent* lBottomRect = new RectComponent(Lepra::String(_T("BottomRect")), new GridLayout(1, 5));
	lBottomRect->AddChild(new RectComponent(Lepra::String(_T("LeftBottomRect"))));

	Button* lCancelButton = new Button(BorderComponent::LINEAR, 4, Lepra::LIGHT_GRAY, Lepra::String(_T("CancelButton")));
	lCancelButton->SetText(Lepra::String(_T("Cancel")), lPainter->GetStandardFont(0), Lepra::BLACK, Lepra::BLACK);
	lCancelButton->SetPreferredSize(100, 20);
	lCancelButton->SetMinSize(20, 20);
	lCancelButton->SetOnUnclickedFunc(ASEFileConverter, OnCancel);
	lBottomRect->AddChild(lCancelButton);

	lBottomRect->AddChild(new RectComponent(Lepra::String(_T("MidBottomRect"))));

	Button* lOkButton = new Button(BorderComponent::LINEAR, 4, Lepra::LIGHT_GRAY, Lepra::String(_T("OkButton")));
	lOkButton->SetText(Lepra::String(_T("Ok")), lPainter->GetStandardFont(0), Lepra::BLACK, Lepra::BLACK);
	lOkButton->SetPreferredSize(100, 20);
	lOkButton->SetMinSize(20, 20);
	lOkButton->SetOnUnclickedFunc(ASEFileConverter, OnOk);
	lBottomRect->AddChild(lOkButton);

	lBottomRect->AddChild(new RectComponent(Lepra::String(_T("RightBottomRect"))));

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
	Window(BORDER_LINEARSHADING | BORDER_RESIZABLE, 4, Lepra::LIGHT_GRAY, Lepra::String(_T("ASEViewer")), new GridLayout(2, 1))
{
	pDesktopWindow;
	pASEFile;

	pDesktopWindow->GetMouseTheme()->LoadBusyCursor();
	Painter* lPainter = pDesktopWindow->GetInternalPainter();

	UiTbc::Caption* lCaption = new UiTbc::Caption(Lepra::LIGHT_BLUE, Lepra::DARK_BLUE, Lepra::LIGHT_BLUE, Lepra::DARK_BLUE,
		Lepra::LIGHT_GRAY, Lepra::GRAY, Lepra::LIGHT_GRAY, Lepra::GRAY, 20);
	lCaption->SetText(Lepra::String(_T("ASEViewer - ")) + pASEFile, lPainter->GetStandardFont(0),
		Lepra::WHITE, Lepra::BLACK, Lepra::BLACK, Lepra::BLACK);
	Window::SetCaption(lCaption);

	ListControl* lTreeView = new ListControl(BORDER_SUNKEN, 3, Lepra::WHITE);
	Window::AddChild(lTreeView);

	ASELoader lLoader;
	ASELoader::ASEData lASEData;

	if (lLoader.Load(lASEData, pASEFile) == true)
	{
		TreeNode* l3DSMaxAsciiExportNode = new TreeNode(Lepra::StringUtility::Format(_T("3DSMaxAsciiExport: %i"), lASEData.m3DSMaxAsciiExport), Lepra::String(_T("3DSMaxAsciiExportNode")));
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
	TreeNode* lSceneNode = new TreeNode(_T("Scene"), Lepra::String(_T("SceneNode")));
	if (pScene != 0)
	{
		lSceneNode->AddChildNode(new TreeNode(Lepra::String(_T("Comment:")) + pScene->mComment.ToCurrentCode(), Lepra::String(_T("CommentNode"))));
		lSceneNode->AddChildNode(new TreeNode(Lepra::String(_T("Filename: ")) + pScene->mFilename.ToCurrentCode(), Lepra::String(_T("FilenameNode"))));
		lSceneNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("FirstFrame: %i"), pScene->mFirstFrame), Lepra::String(_T("FirstFrameNode"))));
		lSceneNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("LastFrame: %i"), pScene->mLastFrame), Lepra::String(_T("LastFrameNode"))));
		lSceneNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("FrameSpeed: %i"), pScene->mFrameSpeed), Lepra::String(_T("FrameSpeedNode"))));
		lSceneNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TicksPerFrame: %i"), pScene->mTicksPerFrame), Lepra::String(_T("TicksPerFrameNode"))));
		lSceneNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("BackgroundStatic: [%f, %f, %f]"), pScene->mBackgroundStatic[0], pScene->mBackgroundStatic[1], pScene->mBackgroundStatic[2]), Lepra::String(_T("BackgroundStaticNode"))));
		lSceneNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("AmbientStatic: [%f, %f, %f]"), pScene->mAmbientStatic[0], pScene->mAmbientStatic[1], pScene->mAmbientStatic[2]), Lepra::String(_T("AmbientStaticNode"))));
	}

	return lSceneNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateCamerasNode(const ASELoader::ASEData::CameraObjectList& pCameraList)
{
	TreeNode* lCamerasNode = new TreeNode(_T("Cameras"), Lepra::String(_T("CamerasNode")));

	Lepra::String lFormatString(_T("Camera%."));
	lFormatString += (Lepra::int64)GetNumDigits(pCameraList.GetCount());
	lFormatString += _T("i");

	ASELoader::ASEData::CameraObjectList::ConstIterator lCamIter;
	int lCount = 1;
	for (lCamIter = pCameraList.First(); lCamIter != pCameraList.End(); ++lCamIter, ++lCount)
	{
		TreeNode* lCameraNode = new TreeNode(Lepra::StringUtility::Format(lFormatString.c_str(), lCount), Lepra::String(_T("CameraNode")));
		ASELoader::CameraObject* lCam = *lCamIter;
		
		lCameraNode->AddChildNode(new TreeNode(Lepra::String(_T("CameraType: ")) + lCam->mCameraType.ToCurrentCode(), Lepra::String(_T("CameraTypeNode"))));
		lCameraNode->AddChildNode(new TreeNode(Lepra::String(_T("Comment: ")) + lCam->mCameraType.ToCurrentCode(), Lepra::String(_T("CommentNode"))));
		lCameraNode->AddChildNode(new TreeNode(Lepra::String(_T("NodeName: ")) + lCam->mNodeName.ToCurrentCode(), Lepra::String(_T("NodeNameNode"))));
		
		lCameraNode->AddChildNode(CreateTMListNode(lCam->mNodeTMList));
		lCameraNode->AddChildNode(CreateTMAnimationNode(*lCam->mTMAnimation));

		if (lCam->mSettings != 0)
		{
			TreeNode* lSettingsNode = new TreeNode(Lepra::String(_T("Settings")), Lepra::String(_T("SettingsNode")));

			lSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("FOV: %f"), lCam->mSettings->mFOV), Lepra::String(_T("FOVNode"))));
			lSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("Near: %f"), lCam->mSettings->mNear), Lepra::String(_T("NearNode"))));
			lSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("Far: %f"), lCam->mSettings->mFar), Lepra::String(_T("FarNode"))));
			lSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TDist: %f"), lCam->mSettings->mTDist), Lepra::String(_T("TDistNode"))));
			lSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TimeValue: %i"), lCam->mSettings->mTimeValue), Lepra::String(_T("TimeValueNode"))));

			lCameraNode->AddChildNode(lSettingsNode);
		}

		lCamerasNode->AddChildNode(lCameraNode);
	}

	return lCamerasNode;
}


TreeNode* ASEFileConverter::ASEViewer::CreateLightsNode(const ASELoader::ASEData::LightObjectList& pLightList)
{
	TreeNode* lLightsNode = new TreeNode(_T("Lights"), Lepra::String(_T("LightsNode")));

	Lepra::String lFormatString(_T("Light%."));
	lFormatString += (Lepra::int64)GetNumDigits(pLightList.GetCount());
	lFormatString += _T("i");

	ASELoader::ASEData::LightObjectList::ConstIterator lLightIter;
	int lCount = 1;
	for (lLightIter = pLightList.First(); lLightIter != pLightList.End(); ++lLightIter, ++lCount)
	{
		TreeNode* lLightNode = new TreeNode(Lepra::StringUtility::Format(lFormatString.c_str(), lCount), Lepra::String(_T("LightNode")));
		ASELoader::LightObject* lLight = *lLightIter;
		lLightNode->AddChildNode(new TreeNode(Lepra::String(_T("NodeName: ")) + lLight->mNodeName.ToCurrentCode(), Lepra::String(_T("NodeNameNode"))));
		lLightNode->AddChildNode(new TreeNode(Lepra::String(_T("Comment: ")) + lLight->mComment.ToCurrentCode(), Lepra::String(_T("CommentNode"))));
		lLightNode->AddChildNode(new TreeNode(Lepra::String(_T("LightType: ")) + lLight->mLightType.ToCurrentCode(), Lepra::String(_T("LightTypeNode"))));
		lLightNode->AddChildNode(new TreeNode(Lepra::String(_T("Shadows: ")) + lLight->mShadows.ToCurrentCode(), Lepra::String(_T("ShadowsNode"))));
		lLightNode->AddChildNode(new TreeNode(Lepra::String(_T("SpotShape: ")) + lLight->mSpotShape.ToCurrentCode(), Lepra::String(_T("SpotShapeNode"))));

		lLightNode->AddChildNode(new TreeNode(Lepra::String(_T("UseLight: ")) + ToString(lLight->mUseLight), Lepra::String(_T("UseLightNode"))));
		lLightNode->AddChildNode(new TreeNode(Lepra::String(_T("UseGlobal: ")) + ToString(lLight->mUseGlobal), Lepra::String(_T("UseGlobalNode"))));
		lLightNode->AddChildNode(new TreeNode(Lepra::String(_T("AbsMapBias: ")) + ToString(lLight->mAbsMapBias), Lepra::String(_T("AbsMapBiasNode"))));
		lLightNode->AddChildNode(new TreeNode(Lepra::String(_T("OverShoot: ")) + ToString(lLight->mOverShoot), Lepra::String(_T("OverShootNode"))));

		lLightNode->AddChildNode(CreateTMListNode(lLight->mNodeTMList));
		lLightNode->AddChildNode(CreateTMAnimationNode(*lLight->mTMAnimation));

		lLightNode->AddChildNode(CreateLightSettingsNode(lLight->mSettings));

		lLightsNode->AddChildNode(lLightNode);
	}

	return lLightsNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateGeometriesNode(const ASELoader::ASEData::GeomObjectList& pGeometryList)
{
	TreeNode* lGeometriesNode = new TreeNode(_T("Geometries"), Lepra::String(_T("GeometriesNode")));

	Lepra::String lFormatString(_T("Geometry%."));
	lFormatString += (Lepra::int64)GetNumDigits(pGeometryList.GetCount());
	lFormatString += _T("i");

	ASELoader::ASEData::GeomObjectList::ConstIterator lGeomIter;
	int lCount = 1;
	for (lGeomIter = pGeometryList.First(); lGeomIter != pGeometryList.End(); ++lGeomIter, ++lCount)
	{
		TreeNode* lGeometryNode = new TreeNode(Lepra::StringUtility::Format(lFormatString.c_str(), lCount), Lepra::String(_T("GeometryNode")));
		ASELoader::GeomObject* lGeom = *lGeomIter;

		lGeometryNode->AddChildNode(new TreeNode(Lepra::String(_T("NodeName: ")) + lGeom->mNodeName.ToCurrentCode(), Lepra::String(_T("NodeNameNode"))));
		lGeometryNode->AddChildNode(new TreeNode(Lepra::String(_T("Comment: ")) + lGeom->mComment.ToCurrentCode(), Lepra::String(_T("CommentNode"))));
		lGeometryNode->AddChildNode(new TreeNode(Lepra::String(_T("MotionBlur: ")) + ToString(lGeom->mMotionBlur), Lepra::String(_T("MotionBlurNode"))));
		lGeometryNode->AddChildNode(new TreeNode(Lepra::String(_T("CastShadow: ")) + ToString(lGeom->mCastShadow), Lepra::String(_T("CastShadowNode"))));
		lGeometryNode->AddChildNode(new TreeNode(Lepra::String(_T("RecvShadow: ")) + ToString(lGeom->mRecvShadow), Lepra::String(_T("RecvShadowNode"))));

		lGeometryNode->AddChildNode(new TreeNode(Lepra::String(_T("MaterialRef: ")) + (Lepra::int64)lGeom->mMaterialRef, Lepra::String(_T("MaterialRefNode"))));
		lGeometryNode->AddChildNode(CreateTMListNode(lGeom->mNodeTMList));
		lGeometryNode->AddChildNode(CreateTMAnimationNode(*lGeom->mTMAnimation));
		lGeometryNode->AddChildNode(CreateMeshListNode(lGeom->mMeshList));

		lGeometriesNode->AddChildNode(lGeometryNode);
	}

	return lGeometriesNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateMaterialsNode(const ASELoader::ASEData::MaterialList& pMaterialList)
{
	TreeNode* lMaterialsNode = new TreeNode(_T("Materials"), Lepra::String(_T("MaterialsNode")));

	Lepra::String lFormatString(_T("Material%."));
	lFormatString += (Lepra::int64)GetNumDigits(pMaterialList.GetCount());
	lFormatString += _T("i");

	ASELoader::ASEData::MaterialList::ConstIterator lMatIter;
	int lCount = 1;
	for (lMatIter = pMaterialList.First(); lMatIter != pMaterialList.End(); ++lMatIter, ++lCount)
	{
		TreeNode* lMaterialNode = new TreeNode(Lepra::StringUtility::Format(lFormatString.c_str(), lCount), Lepra::String(_T("MaterialNode")));
		ASELoader::Material* lMat = *lMatIter;

		lMaterialNode->AddChildNode(new TreeNode(Lepra::String(_T("Name: ")) + lMat->mName.ToCurrentCode(), Lepra::String(_T("NameNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::String(_T("Class: ")) + lMat->mClass.ToCurrentCode(), Lepra::String(_T("ClassNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::String(_T("Comment: ")) + lMat->mComment.ToCurrentCode(), Lepra::String(_T("CommentNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::String(_T("Shading: ")) + lMat->mShading.ToCurrentCode(), Lepra::String(_T("ShadingNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::String(_T("Falloff: ")) + lMat->mFalloff.ToCurrentCode(), Lepra::String(_T("FalloffNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::String(_T("XPType: ")) + lMat->mXPType.ToCurrentCode(), Lepra::String(_T("XPTypeNode"))));

		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("Ambient: [%f, %f, %f]"), lMat->mAmbient[0], lMat->mAmbient[1], lMat->mAmbient[2]), Lepra::String(_T("AmbientNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("Diffuse: [%f, %f, %f]"), lMat->mDiffuse[0], lMat->mDiffuse[1], lMat->mDiffuse[2]), Lepra::String(_T("DiffuseNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("Specular: [%f, %f, %f]"), lMat->mSpecular[0], lMat->mSpecular[1], lMat->mSpecular[2]), Lepra::String(_T("SpecularNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("Shine: %f"), lMat->mShine), Lepra::String(_T("ShineNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("ShineStrength: %f"), lMat->mShineStrength), Lepra::String(_T("ShineStrengthNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("Transparency: %f"), lMat->mTransparency), Lepra::String(_T("TransparencyNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("WireSize: %f"), lMat->mWireSize), Lepra::String(_T("WireSizeNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("XPFalloff: %f"), lMat->mXPFalloff), Lepra::String(_T("XPFalloffNode"))));
		lMaterialNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("SelfIllum: %f"), lMat->mSelfIllum), Lepra::String(_T("SelfIllumNode"))));

		lMaterialNode->AddChildNode(CreateSubMaterialListNode(lMat->mSubMaterialList));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mAmbientMapList, Lepra::String(_T("AmbientMapList"))));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mDiffuseMapList, Lepra::String(_T("DiffuseMapList"))));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mSpecularMapList, Lepra::String(_T("SpecularMapList"))));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mShineMapList, Lepra::String(_T("ShineMapList"))));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mShineStrengthMapList, Lepra::String(_T("ShineStrengthMapList"))));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mSelfIllumMapList, Lepra::String(_T("SelfIllumMapList"))));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mOpacityMapList, Lepra::String(_T("OpacityMapList"))));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mFilterColorMapList, Lepra::String(_T("FilterColorMapList"))));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mBumpMapList, Lepra::String(_T("BumpMapList"))));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mReflectMapList, Lepra::String(_T("ReflectMapList"))));
		lMaterialNode->AddChildNode(CreateMapListNode(lMat->mRefractMapList, Lepra::String(_T("RefractMapList"))));

		lMaterialsNode->AddChildNode(lMaterialNode);
	}

	return lMaterialsNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateTMListNode(const std::list<ASELoader::NodeTM*>& pNodeTMList)
{
	TreeNode* lNodeTMListNode = new TreeNode(Lepra::String(_T("NodeTMList")), Lepra::String(_T("NodeTMListNode")));

	Lepra::String lFormatString(_T("NodeTM%."));
	lFormatString += (Lepra::int64)GetNumDigits(pNodeTMList.GetCount());
	lFormatString += _T("i");

	int lCount;
	std::list<ASELoader::NodeTM*>::ConstIterator lIter;
	for (lIter = pNodeTMList.First(), lCount = 1; lIter != pNodeTMList.End(); ++lIter, ++lCount)
	{
		TreeNode* lNodeTMNode = new TreeNode(Lepra::StringUtility::Format(lFormatString.c_str(), lCount), Lepra::String(_T("NodeTMNode")));

		ASELoader::NodeTM* lNodeTM = *lIter;
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::String(_T("NodeName: ")) + lNodeTM->mNodeName.ToCurrentCode(), Lepra::String(_T("NodeNameNode"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::String(_T("Comment: "))  + lNodeTM->mComment.ToCurrentCode(), Lepra::String(_T("CommentNode"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("InheritPos: [%i, %i, %i]"), lNodeTM->mInheritPos[0], lNodeTM->mInheritPos[1], lNodeTM->mInheritPos[2]), Lepra::String(_T("InheritPosNode"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("InheritRot: [%i, %i, %i]"), lNodeTM->mInheritRot[0], lNodeTM->mInheritRot[1], lNodeTM->mInheritRot[2]), Lepra::String(_T("InheritRotNode"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("InheritScl: [%i, %i, %i]"), lNodeTM->mInheritScl[0], lNodeTM->mInheritScl[1], lNodeTM->mInheritScl[2]), Lepra::String(_T("InheritSclNode"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TMPos: [%f, %f, %f]"), lNodeTM->mTMPos[0], lNodeTM->mTMPos[1], lNodeTM->mTMPos[2]), Lepra::String(_T("TMPosNode"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TMRotAngle: %f"), lNodeTM->mTMRotAngle), Lepra::String(_T("TMRotAngleNode"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TMRotAxis: [%f, %f, %f]"), lNodeTM->mTMRotAxis[0], lNodeTM->mTMRotAxis[1], lNodeTM->mTMRotAxis[2]), Lepra::String(_T("TMRotAxisNode"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TMRow0: [%f, %f, %f]"), lNodeTM->mTMRow0[0], lNodeTM->mTMRow0[1], lNodeTM->mTMRow0[2]), Lepra::String(_T("TMRow0Node"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TMRow1: [%f, %f, %f]"), lNodeTM->mTMRow1[0], lNodeTM->mTMRow1[1], lNodeTM->mTMRow1[2]), Lepra::String(_T("TMRow1Node"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TMRow2: [%f, %f, %f]"), lNodeTM->mTMRow2[0], lNodeTM->mTMRow2[1], lNodeTM->mTMRow2[2]), Lepra::String(_T("TMRow2Node"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TMRow3: [%f, %f, %f]"), lNodeTM->mTMRow3[0], lNodeTM->mTMRow3[1], lNodeTM->mTMRow3[2]), Lepra::String(_T("TMRow3Node"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TMScale: [%f, %f, %f]"), lNodeTM->mTMScale[0], lNodeTM->mTMScale[1], lNodeTM->mTMScale[2]), Lepra::String(_T("TMScaleNode"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TMScaleAxis: [%f, %f, %f]"), lNodeTM->mTMScaleAxis[0], lNodeTM->mTMScaleAxis[1], lNodeTM->mTMScaleAxis[2]), Lepra::String(_T("TMScaleAxisNode"))));
		lNodeTMNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TMScaleAxisAng: %f"), lNodeTM->mTMScaleAxisAng), Lepra::String(_T("TMScaleAxisAngNode"))));

		lNodeTMListNode->AddChildNode(lNodeTMNode);
	}

	return lNodeTMListNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateTMAnimationNode(const ASELoader::TMAnimation& pTMAnimation)
{
	TreeNode* lTMAnimationNode = new TreeNode(Lepra::String(_T("TMAnimation")), Lepra::String(_T("TMAnimationNode")));

	lTMAnimationNode->AddChildNode(new TreeNode(Lepra::String(_T("NodeName: ")) + pTMAnimation.mNodeName.ToCurrentCode(), Lepra::String(_T("NodeNameNode"))));
	lTMAnimationNode->AddChildNode(new TreeNode(Lepra::String(_T("Comment: ")) + pTMAnimation.mComment.ToCurrentCode(), Lepra::String(_T("CommentNode"))));

	TreeNode* lControlPosTrackNode = new TreeNode(Lepra::String(_T("ControlPosTrack")), Lepra::String(_T("ControlPosTrackNode")));
	TreeNode* lControlRotTrackNode = new TreeNode(Lepra::String(_T("ControlRotTrack")), Lepra::String(_T("ControlRotTrackNode")));

	Lepra::String lFormatString(_T("ControlPosSample%."));
	lFormatString += (Lepra::int64)GetNumDigits(pTMAnimation.mControlPosTrack.GetCount());
	lFormatString += _T("i");

	int lCount;
	ASELoader::TMAnimation::ControlPosSampleList::ConstIterator lPosIter;
	for(lPosIter = pTMAnimation.mControlPosTrack.First(), lCount = 1; lPosIter != pTMAnimation.mControlPosTrack.End(); ++lPosIter, ++lCount)
	{
		Lepra::String lName(Lepra::StringUtility::Format(lFormatString.c_str(), lCount));
		lControlPosTrackNode->AddChildNode(new TreeNode(lName, Lepra::String(lName + _T("Node"))));
	}

	lFormatString = _T("ControlRotSample%.");
	lFormatString += (Lepra::int64)GetNumDigits(pTMAnimation.mControlPosTrack.GetCount());
	lFormatString += _T("i");

	ASELoader::TMAnimation::ControlRotSampleList::ConstIterator lRotIter;
	for(lRotIter = pTMAnimation.mControlRotTrack.First(), lCount = 1; lRotIter != pTMAnimation.mControlRotTrack.End(); ++lRotIter, ++lCount)
	{
		Lepra::String lName(Lepra::StringUtility::Format(lFormatString.c_str(), lCount));
		lControlRotTrackNode->AddChildNode(new TreeNode(lName, Lepra::String(lName + _T("Node"))));
	}

	lTMAnimationNode->AddChildNode(lControlPosTrackNode);
	lTMAnimationNode->AddChildNode(lControlRotTrackNode);

	return lTMAnimationNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateLightSettingsNode(const ASELoader::LightSettings* pLightSettings)
{
	TreeNode* lLightSettingsNode = new TreeNode(Lepra::String(_T("LightSettings")), Lepra::String(_T("LightSettingsNode")));

	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TimeValue: %i"), pLightSettings->mTimeValue), Lepra::String(_T("TimeValueNode"))));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("Color: [%f, %f, %f]"), pLightSettings->mColor[0], pLightSettings->mColor[1], pLightSettings->mColor[2]), Lepra::String(_T("ColorNode"))));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("Intens:   %f"), pLightSettings->mIntens), Lepra::String(_T("IntensNode"))));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("Aspect:   %f"), pLightSettings->mAspect), Lepra::String(_T("AspectNode"))));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("HotSpot:  %f"), pLightSettings->mHotSpot), Lepra::String(_T("HotSpotNode"))));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("Falloff:  %f"), pLightSettings->mFalloff), Lepra::String(_T("FalloffNode"))));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("TDist:    %f"), pLightSettings->mTDist), Lepra::String(_T("TDistNode"))));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("MapBias:  %f"), pLightSettings->mMapBias), Lepra::String(_T("MapBiasNode"))));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("MapRange: %f"), pLightSettings->mMapRange), Lepra::String(_T("MapRangeNode"))));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("MapSize:  %f"), pLightSettings->mMapSize), Lepra::String(_T("MapSizeNode"))));
	lLightSettingsNode->AddChildNode(new TreeNode(Lepra::StringUtility::Format(_T("RayBias:  %f"), pLightSettings->mRayBias), Lepra::String(_T("RayBiasNode"))));
	
	return lLightSettingsNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateMeshListNode(const ASELoader::GeomObject::MeshList& pMeshList)
{
	TreeNode* lMeshListNode = new TreeNode(Lepra::String(_T("MeshList")), Lepra::String(_T("MeshListNode")));

	Lepra::String lFormatString(_T("Mesh%."));
	lFormatString += (Lepra::int64)GetNumDigits(pMeshList.GetCount());
	lFormatString += _T("i");

	ASELoader::GeomObject::MeshList::ConstIterator lIter;
	int lCount = 1;
	for (lIter = pMeshList.First(); lIter != pMeshList.End(); ++lIter, ++lCount)
	{
		TreeNode* lMeshNode = new TreeNode(Lepra::StringUtility::Format(lFormatString.c_str(), lCount), Lepra::String(_T("MeshNode")));
		//ASELoader::Mesh* lMesh = *lIter;

		//lMesh->

		lMeshListNode->AddChildNode(lMeshNode);
	}

	return lMeshListNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateSubMaterialListNode(const ASELoader::Material::MaterialList& pSubMatList)
{
	TreeNode* lSubMaterialListNode = new TreeNode(Lepra::String(_T("SubMaterialList")), Lepra::String(_T("SubMaterialListNode")));

	pSubMatList;

	return lSubMaterialListNode;
}

TreeNode* ASEFileConverter::ASEViewer::CreateMapListNode(const ASELoader::Material::MapList& pMapList, const Lepra::String& pNodeName)
{
	TreeNode* lMapListNode = new TreeNode(pNodeName, Lepra::String(_T("MapListNode")));

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
		return Lepra::String(_T("true"));
	}
	else
	{
		return Lepra::String(_T("false"));
	}
}

} // End namespace.
