/*
	Class:  ASEFileConverter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand

	NOTES:

	Just create an instance of this class and add it to the
	DesktopWindow and you are done.
*/

#pragma once

#include "./GUI/UiWindow.h"
#include "UiASELoader.h"

namespace UiTbc
{

class DesktopWindow;
class FileNameField;
class Button;
class TreeNode;

class ASEFileConverter : public Window
{
public:
	ASEFileConverter(DesktopWindow* pDesktopWindow);
	virtual ~ASEFileConverter();
private:
	class ASEViewer : public Window
	{
	public:
		ASEViewer(DesktopWindow* pDesktopWindow, const Lepra::String& pASEFile);
		virtual ~ASEViewer();
	private:
		TreeNode* CreateSceneNode(const ASELoader::Scene* pScene);
		TreeNode* CreateCamerasNode(const ASELoader::ASEData::CameraObjectList& pCameraList);
		TreeNode* CreateLightsNode(const ASELoader::ASEData::LightObjectList& pLightList);
		TreeNode* CreateGeometriesNode(const ASELoader::ASEData::GeomObjectList& pGeometryList);
		TreeNode* CreateMaterialsNode(const ASELoader::ASEData::MaterialList& pMaterialList);
		TreeNode* CreateTMListNode(const std::list<ASELoader::NodeTM*>& pNodeTMList);
		TreeNode* CreateTMAnimationNode(const ASELoader::TMAnimation& pTMAnimation);

		TreeNode* CreateLightSettingsNode(const ASELoader::LightSettings* pLightSettings);
		TreeNode* CreateMeshListNode(const ASELoader::GeomObject::MeshList& pMeshList);
		TreeNode* CreateSubMaterialListNode(const ASELoader::Material::MaterialList& pSubMatList);
		TreeNode* CreateMapListNode(const ASELoader::Material::MapList& pMapList, const Lepra::String& pNodeName);

		static int GetNumDigits(int pValue);
		static Lepra::String ToString(bool pValue);
	};

	void OnOk(UiTbc::Button*);
	void OnCancel(UiTbc::Button*);

	FileNameField* mFileNameField;
};

}
