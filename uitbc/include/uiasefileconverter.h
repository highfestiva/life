/*
	Class:  ASEFileConverter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand

	NOTES:

	Just create an instance of this class and add it to the
	DesktopWindow and you are done.
*/

#pragma once

#include "./gui/uiwindow.h"
#include "uiaseloader.h"

namespace uitbc {

class DesktopWindow;
class FileNameField;
class Button;
class TreeNode;

class ASEFileConverter : public Window {
public:
	ASEFileConverter(DesktopWindow* desktop_window);
	virtual ~ASEFileConverter();
private:
	class ASEViewer : public Window {
	public:
		ASEViewer(DesktopWindow* desktop_window, const lepra::String& ase_file);
		virtual ~ASEViewer();
	private:
		TreeNode* CreateSceneNode(const ASELoader::Scene* scene);
		TreeNode* CreateCamerasNode(const ASELoader::ASEData::CameraObjectList& camera_list);
		TreeNode* CreateLightsNode(const ASELoader::ASEData::LightObjectList& light_list);
		TreeNode* CreateGeometriesNode(const ASELoader::ASEData::GeomObjectList& geometry_list);
		TreeNode* CreateMaterialsNode(const ASELoader::ASEData::MaterialList& material_list);
		TreeNode* CreateTMListNode(const std::list<ASELoader::NodeTM*>& node_tm_list);
		TreeNode* CreateTMAnimationNode(const ASELoader::TMAnimation& tm_animation);

		TreeNode* CreateLightSettingsNode(const ASELoader::LightSettings* light_settings);
		TreeNode* CreateMeshListNode(const ASELoader::GeomObject::MeshList& mesh_list);
		TreeNode* CreateSubMaterialListNode(const ASELoader::Material::MaterialList& sub_mat_list);
		TreeNode* CreateMapListNode(const ASELoader::Material::MapList& map_list, const lepra::String& node_name);

		static int GetNumDigits(int value);
		static lepra::String ToString(bool value);
	};

	void OnOk(uitbc::Button*);
	void OnCancel(uitbc::Button*);

	FileNameField* file_name_field_;
};

}
