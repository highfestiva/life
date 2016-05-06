/*
	Class:  ASEFileConverter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "pch.h"
#include "../include/uiasefileconverter.h"
#include "../include/uiaseloader.h"
#include "../include/uipainter.h"
#include "../include/UiMouseTheme.h"
#include "../include/gui/uibutton.h"
#include "../include/gui/uicaption.h"
#include "../include/gui/uidesktopwindow.h"
#include "../include/gui/uifilenamefield.h"
#include "../include/gui/uigridlayout.h"
#include "../include/gui/uirectcomponent.h"
#include "../include/gui/uitreenode.h"
#include "../../lepra/include/graphics2d.h"

namespace uitbc {

ASEFileConverter::ASEFileConverter(DesktopWindow* desktop_window) :
	Window(kBorderLinearshading, 4, lepra::LIGHT_GRAY, lepra::String("ASEFileConverter"), new GridLayout(4, 1)),
	file_name_field_(0) {
	Painter* painter = desktop_window->GetInternalPainter();
	uitbc::Caption* caption = new uitbc::Caption(lepra::LIGHT_BLUE, lepra::DARK_BLUE, lepra::LIGHT_BLUE, lepra::DARK_BLUE,
		lepra::LIGHT_GRAY, lepra::GRAY, lepra::LIGHT_GRAY, lepra::GRAY, 20);
	caption->SetText(lepra::String("Select an ASE file to convert"), painter->GetStandardFont(0),
		lepra::WHITE, lepra::BLACK, lepra::BLACK, lepra::BLACK);
	Window::SetCaption(caption);

	// The top rect creates some space between the caption and the filename field.
	RectComponent* top_rect = new RectComponent(lepra::String("TopRect"));
	Window::AddChild(top_rect);

	file_name_field_ = new FileNameField(desktop_window, kBorderSunken | kBorderLinearshading, 3, lepra::WHITE, lepra::String("ASEFileNameField"));
	file_name_field_->SetFont(painter->GetStandardFont(0), lepra::BLACK, Component::kAlphatest, 128);
	file_name_field_->SetPreferredHeight(24);
	file_name_field_->SetMinSize(0, 20);
	file_name_field_->AddFileExtension("ase");
	Window::AddChild(file_name_field_);

	RectComponent* mid_rect = new RectComponent(lepra::String("MidRect"));
	Window::AddChild(mid_rect);

	RectComponent* bottom_rect = new RectComponent(lepra::String("BottomRect"), new GridLayout(1, 5));
	bottom_rect->AddChild(new RectComponent(lepra::String("LeftBottomRect")));

	Button* cancel_button = new Button(BorderComponent::kLinear, 4, lepra::LIGHT_GRAY, lepra::String("CancelButton"));
	cancel_button->SetText(lepra::String("Cancel"), painter->GetStandardFont(0), lepra::BLACK, lepra::BLACK);
	cancel_button->SetPreferredSize(100, 20);
	cancel_button->SetMinSize(20, 20);
	cancel_button->SetOnUnclickedFunc(ASEFileConverter, OnCancel);
	bottom_rect->AddChild(cancel_button);

	bottom_rect->AddChild(new RectComponent(lepra::String("MidBottomRect")));

	Button* ok_button = new Button(BorderComponent::kLinear, 4, lepra::LIGHT_GRAY, lepra::String("OkButton"));
	ok_button->SetText(lepra::String("Ok"), painter->GetStandardFont(0), lepra::BLACK, lepra::BLACK);
	ok_button->SetPreferredSize(100, 20);
	ok_button->SetMinSize(20, 20);
	ok_button->SetOnUnclickedFunc(ASEFileConverter, OnOk);
	bottom_rect->AddChild(ok_button);

	bottom_rect->AddChild(new RectComponent(lepra::String("RightBottomRect")));

	bottom_rect->SetPreferredHeight(30);
	Window::AddChild(bottom_rect);

	SetPreferredSize(500, 100);
}

ASEFileConverter::~ASEFileConverter() {
}

void ASEFileConverter::OnOk(uitbc::Button*) {
	uitbc::DesktopWindow* _desktop_window = (uitbc::DesktopWindow*)GetParentOfType(uitbc::Component::kDesktopwindow);

	_desktop_window->AddChild(new ASEViewer(_desktop_window, file_name_field_->GetText()));
	_desktop_window->DeleteComponent(this, 0);
}

void ASEFileConverter::OnCancel(uitbc::Button*) {
	uitbc::DesktopWindow* _desktop_window = (uitbc::DesktopWindow*)GetParentOfType(uitbc::Component::kDesktopwindow);
	_desktop_window->DeleteComponent(this, 0);
}





ASEFileConverter::ASEViewer::ASEViewer(DesktopWindow* desktop_window, const lepra::String& ase_file) :
	Window(kBorderLinearshading | kBorderResizable, 4, lepra::LIGHT_GRAY, lepra::String("ASEViewer"), new GridLayout(2, 1)) {
	desktop_window;
	ase_file;

	desktop_window->GetMouseTheme()->LoadBusyCursor();
	Painter* painter = desktop_window->GetInternalPainter();

	uitbc::Caption* caption = new uitbc::Caption(lepra::LIGHT_BLUE, lepra::DARK_BLUE, lepra::LIGHT_BLUE, lepra::DARK_BLUE,
		lepra::LIGHT_GRAY, lepra::GRAY, lepra::LIGHT_GRAY, lepra::GRAY, 20);
	caption->SetText(lepra::String("ASEViewer - ") + ase_file, painter->GetStandardFont(0),
		lepra::WHITE, lepra::BLACK, lepra::BLACK, lepra::BLACK);
	Window::SetCaption(caption);

	ListControl* tree_view = new ListControl(kBorderSunken, 3, lepra::WHITE);
	Window::AddChild(tree_view);

	ASELoader loader;
	ASELoader::ASEData ase_data;

	if (loader.Load(ase_data, ase_file) == true) {
		TreeNode* l3DSMaxAsciiExportNode = new TreeNode(lepra::StringUtility::Format("3DSMaxAsciiExport: %i", ase_data.m3DSMaxAsciiExport_), lepra::String("3DSMaxAsciiExportNode"));
		tree_view->AddChild(l3DSMaxAsciiExportNode);

		tree_view->AddChild(CreateSceneNode(ase_data.scene_));
		tree_view->AddChild(CreateCamerasNode(ase_data.cam_list_));
		tree_view->AddChild(CreateLightsNode(ase_data.light_list_));
		tree_view->AddChild(CreateGeometriesNode(ase_data.geom_list_));
		tree_view->AddChild(CreateMaterialsNode(ase_data.material_list_));
	} else {
		// TODO: Create a message box.
	}

	SetPreferredSize(300, 400);
	desktop_window->GetMouseTheme()->LoadArrowCursor();
}

ASEFileConverter::ASEViewer::~ASEViewer() {
}

TreeNode* ASEFileConverter::ASEViewer::CreateSceneNode(const ASELoader::Scene* scene) {
	TreeNode* scene_node = new TreeNode("Scene", lepra::String("SceneNode"));
	if (scene != 0) {
		scene_node->AddChildNode(new TreeNode(lepra::String("Comment:") + scene->comment_.ToCurrentCode(), lepra::String("CommentNode")));
		scene_node->AddChildNode(new TreeNode(lepra::String("Filename: ") + scene->filename_.ToCurrentCode(), lepra::String("FilenameNode")));
		scene_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("FirstFrame: %i", scene->first_frame_), lepra::String("FirstFrameNode")));
		scene_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("LastFrame: %i", scene->last_frame_), lepra::String("LastFrameNode")));
		scene_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("FrameSpeed: %i", scene->frame_speed_), lepra::String("FrameSpeedNode")));
		scene_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TicksPerFrame: %i", scene->ticks_per_frame_), lepra::String("TicksPerFrameNode")));
		scene_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("BackgroundStatic: [%f, %f, %f]", scene->background_static_[0], scene->background_static_[1], scene->background_static_[2]), lepra::String("BackgroundStaticNode")));
		scene_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("AmbientStatic: [%f, %f, %f]", scene->ambient_static_[0], scene->ambient_static_[1], scene->ambient_static_[2]), lepra::String("AmbientStaticNode")));
	}

	return scene_node;
}

TreeNode* ASEFileConverter::ASEViewer::CreateCamerasNode(const ASELoader::ASEData::CameraObjectList& camera_list) {
	TreeNode* cameras_node = new TreeNode("Cameras", lepra::String("CamerasNode"));

	lepra::String format_string("Camera%.");
	format_string += (lepra::int64)GetNumDigits(camera_list.GetCount());
	format_string += "i";

	ASELoader::ASEData::CameraObjectList::ConstIterator cam_iter;
	int count = 1;
	for (cam_iter = camera_list.First(); cam_iter != camera_list.End(); ++cam_iter, ++count) {
		TreeNode* camera_node = new TreeNode(lepra::StringUtility::Format(format_string.c_str(), count), lepra::String("CameraNode"));
		ASELoader::CameraObject* cam = *cam_iter;

		camera_node->AddChildNode(new TreeNode(lepra::String("CameraType: ") + cam->camera_type_.ToCurrentCode(), lepra::String("CameraTypeNode")));
		camera_node->AddChildNode(new TreeNode(lepra::String("Comment: ") + cam->camera_type_.ToCurrentCode(), lepra::String("CommentNode")));
		camera_node->AddChildNode(new TreeNode(lepra::String("NodeName: ") + cam->node_name_.ToCurrentCode(), lepra::String("NodeNameNode")));

		camera_node->AddChildNode(CreateTMListNode(cam->node_tm_list_));
		camera_node->AddChildNode(CreateTMAnimationNode(*cam->tm_animation_));

		if (cam->settings_ != 0) {
			TreeNode* settings_node = new TreeNode(lepra::String("Settings"), lepra::String("SettingsNode"));

			settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("FOV: %f", cam->settings_->fov_), lepra::String("FOVNode")));
			settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("Near: %f", cam->settings_->near_), lepra::String("NearNode")));
			settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("Far: %f", cam->settings_->far_), lepra::String("FarNode")));
			settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TDist: %f", cam->settings_->t_dist_), lepra::String("TDistNode")));
			settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TimeValue: %i", cam->settings_->time_value_), lepra::String("TimeValueNode")));

			camera_node->AddChildNode(settings_node);
		}

		cameras_node->AddChildNode(camera_node);
	}

	return cameras_node;
}


TreeNode* ASEFileConverter::ASEViewer::CreateLightsNode(const ASELoader::ASEData::LightObjectList& light_list) {
	TreeNode* lights_node = new TreeNode("Lights", lepra::String("LightsNode"));

	lepra::String format_string("Light%.");
	format_string += (lepra::int64)GetNumDigits(light_list.GetCount());
	format_string += "i";

	ASELoader::ASEData::LightObjectList::ConstIterator light_iter;
	int count = 1;
	for (light_iter = light_list.First(); light_iter != light_list.End(); ++light_iter, ++count) {
		TreeNode* light_node = new TreeNode(lepra::StringUtility::Format(format_string.c_str(), count), lepra::String("LightNode"));
		ASELoader::LightObject* light = *light_iter;
		light_node->AddChildNode(new TreeNode(lepra::String("NodeName: ") + light->node_name_.ToCurrentCode(), lepra::String("NodeNameNode")));
		light_node->AddChildNode(new TreeNode(lepra::String("Comment: ") + light->comment_.ToCurrentCode(), lepra::String("CommentNode")));
		light_node->AddChildNode(new TreeNode(lepra::String("LightType: ") + light->light_type_.ToCurrentCode(), lepra::String("LightTypeNode")));
		light_node->AddChildNode(new TreeNode(lepra::String("Shadows: ") + light->shadows_.ToCurrentCode(), lepra::String("ShadowsNode")));
		light_node->AddChildNode(new TreeNode(lepra::String("SpotShape: ") + light->spot_shape_.ToCurrentCode(), lepra::String("SpotShapeNode")));

		light_node->AddChildNode(new TreeNode(lepra::String("UseLight: ") + ToString(light->use_light_), lepra::String("UseLightNode")));
		light_node->AddChildNode(new TreeNode(lepra::String("UseGlobal: ") + ToString(light->use_global_), lepra::String("UseGlobalNode")));
		light_node->AddChildNode(new TreeNode(lepra::String("AbsMapBias: ") + ToString(light->abs_map_bias_), lepra::String("AbsMapBiasNode")));
		light_node->AddChildNode(new TreeNode(lepra::String("OverShoot: ") + ToString(light->over_shoot_), lepra::String("OverShootNode")));

		light_node->AddChildNode(CreateTMListNode(light->node_tm_list_));
		light_node->AddChildNode(CreateTMAnimationNode(*light->tm_animation_));

		light_node->AddChildNode(CreateLightSettingsNode(light->settings_));

		lights_node->AddChildNode(light_node);
	}

	return lights_node;
}

TreeNode* ASEFileConverter::ASEViewer::CreateGeometriesNode(const ASELoader::ASEData::GeomObjectList& geometry_list) {
	TreeNode* geometries_node = new TreeNode("Geometries", lepra::String("GeometriesNode"));

	lepra::String format_string("Geometry%.");
	format_string += (lepra::int64)GetNumDigits(geometry_list.GetCount());
	format_string += "i";

	ASELoader::ASEData::GeomObjectList::ConstIterator geom_iter;
	int count = 1;
	for (geom_iter = geometry_list.First(); geom_iter != geometry_list.End(); ++geom_iter, ++count) {
		TreeNode* geometry_node = new TreeNode(lepra::StringUtility::Format(format_string.c_str(), count), lepra::String("GeometryNode"));
		ASELoader::GeomObject* geom = *geom_iter;

		geometry_node->AddChildNode(new TreeNode(lepra::String("NodeName: ") + geom->node_name_.ToCurrentCode(), lepra::String("NodeNameNode")));
		geometry_node->AddChildNode(new TreeNode(lepra::String("Comment: ") + geom->comment_.ToCurrentCode(), lepra::String("CommentNode")));
		geometry_node->AddChildNode(new TreeNode(lepra::String("MotionBlur: ") + ToString(geom->motion_blur_), lepra::String("MotionBlurNode")));
		geometry_node->AddChildNode(new TreeNode(lepra::String("CastShadow: ") + ToString(geom->cast_shadow_), lepra::String("CastShadowNode")));
		geometry_node->AddChildNode(new TreeNode(lepra::String("RecvShadow: ") + ToString(geom->recv_shadow_), lepra::String("RecvShadowNode")));

		geometry_node->AddChildNode(new TreeNode(lepra::String("MaterialRef: ") + (lepra::int64)geom->material_ref_, lepra::String("MaterialRefNode")));
		geometry_node->AddChildNode(CreateTMListNode(geom->node_tm_list_));
		geometry_node->AddChildNode(CreateTMAnimationNode(*geom->tm_animation_));
		geometry_node->AddChildNode(CreateMeshListNode(geom->mesh_list_));

		geometries_node->AddChildNode(geometry_node);
	}

	return geometries_node;
}

TreeNode* ASEFileConverter::ASEViewer::CreateMaterialsNode(const ASELoader::ASEData::MaterialList& material_list) {
	TreeNode* materials_node = new TreeNode("Materials", lepra::String("MaterialsNode"));

	lepra::String format_string("Material%.");
	format_string += (lepra::int64)GetNumDigits(material_list.GetCount());
	format_string += "i";

	ASELoader::ASEData::MaterialList::ConstIterator mat_iter;
	int count = 1;
	for (mat_iter = material_list.First(); mat_iter != material_list.End(); ++mat_iter, ++count) {
		TreeNode* material_node = new TreeNode(lepra::StringUtility::Format(format_string.c_str(), count), lepra::String("MaterialNode"));
		ASELoader::Material* mat = *mat_iter;

		material_node->AddChildNode(new TreeNode(lepra::String("Name: ") + mat->name_.ToCurrentCode(), lepra::String("NameNode")));
		material_node->AddChildNode(new TreeNode(lepra::String("Class: ") + mat->clazz_.ToCurrentCode(), lepra::String("ClassNode")));
		material_node->AddChildNode(new TreeNode(lepra::String("Comment: ") + mat->comment_.ToCurrentCode(), lepra::String("CommentNode")));
		material_node->AddChildNode(new TreeNode(lepra::String("Shading: ") + mat->shading_.ToCurrentCode(), lepra::String("ShadingNode")));
		material_node->AddChildNode(new TreeNode(lepra::String("Falloff: ") + mat->falloff_.ToCurrentCode(), lepra::String("FalloffNode")));
		material_node->AddChildNode(new TreeNode(lepra::String("XPType: ") + mat->xp_type_.ToCurrentCode(), lepra::String("XPTypeNode")));

		material_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("Ambient: [%f, %f, %f]", mat->ambient_[0], mat->ambient_[1], mat->ambient_[2]), lepra::String("AmbientNode")));
		material_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("Diffuse: [%f, %f, %f]", mat->diffuse_[0], mat->diffuse_[1], mat->diffuse_[2]), lepra::String("DiffuseNode")));
		material_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("Specular: [%f, %f, %f]", mat->specular_[0], mat->specular_[1], mat->specular_[2]), lepra::String("SpecularNode")));
		material_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("Shine: %f", mat->shine_), lepra::String("ShineNode")));
		material_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("ShineStrength: %f", mat->shine_strength_), lepra::String("ShineStrengthNode")));
		material_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("Transparency: %f", mat->transparency_), lepra::String("TransparencyNode")));
		material_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("WireSize: %f", mat->wire_size_), lepra::String("WireSizeNode")));
		material_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("XPFalloff: %f", mat->xp_falloff_), lepra::String("XPFalloffNode")));
		material_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("SelfIllum: %f", mat->self_illum_), lepra::String("SelfIllumNode")));

		material_node->AddChildNode(CreateSubMaterialListNode(mat->sub_material_list_));
		material_node->AddChildNode(CreateMapListNode(mat->ambient_map_list_, lepra::String("AmbientMapList")));
		material_node->AddChildNode(CreateMapListNode(mat->diffuse_map_list_, lepra::String("DiffuseMapList")));
		material_node->AddChildNode(CreateMapListNode(mat->specular_map_list_, lepra::String("SpecularMapList")));
		material_node->AddChildNode(CreateMapListNode(mat->shine_map_list_, lepra::String("ShineMapList")));
		material_node->AddChildNode(CreateMapListNode(mat->shine_strength_map_list_, lepra::String("ShineStrengthMapList")));
		material_node->AddChildNode(CreateMapListNode(mat->self_illum_map_list_, lepra::String("SelfIllumMapList")));
		material_node->AddChildNode(CreateMapListNode(mat->opacity_map_list_, lepra::String("OpacityMapList")));
		material_node->AddChildNode(CreateMapListNode(mat->filter_color_map_list_, lepra::String("FilterColorMapList")));
		material_node->AddChildNode(CreateMapListNode(mat->bump_map_list_, lepra::String("BumpMapList")));
		material_node->AddChildNode(CreateMapListNode(mat->reflect_map_list_, lepra::String("ReflectMapList")));
		material_node->AddChildNode(CreateMapListNode(mat->refract_map_list_, lepra::String("RefractMapList")));

		materials_node->AddChildNode(material_node);
	}

	return materials_node;
}

TreeNode* ASEFileConverter::ASEViewer::CreateTMListNode(const std::list<ASELoader::NodeTM*>& node_tm_list) {
	TreeNode* node_tm_list_node = new TreeNode(lepra::String("NodeTMList"), lepra::String("NodeTMListNode"));

	lepra::String format_string("NodeTM%.");
	format_string += (lepra::int64)GetNumDigits(node_tm_list.GetCount());
	format_string += "i";

	int count;
	std::list<ASELoader::NodeTM*>::ConstIterator iter;
	for (iter = node_tm_list.First(), count = 1; iter != node_tm_list.End(); ++iter, ++count) {
		TreeNode* node_tm_node = new TreeNode(lepra::StringUtility::Format(format_string.c_str(), count), lepra::String("NodeTMNode"));

		ASELoader::NodeTM* node_tm = *iter;
		node_tm_node->AddChildNode(new TreeNode(lepra::String("NodeName: ") + node_tm->node_name_.ToCurrentCode(), lepra::String("NodeNameNode")));
		node_tm_node->AddChildNode(new TreeNode(lepra::String("Comment: ")  + node_tm->comment_.ToCurrentCode(), lepra::String("CommentNode")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("InheritPos: [%i, %i, %i]", node_tm->inherit_pos_[0], node_tm->inherit_pos_[1], node_tm->inherit_pos_[2]), lepra::String("InheritPosNode")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("InheritRot: [%i, %i, %i]", node_tm->inherit_rot_[0], node_tm->inherit_rot_[1], node_tm->inherit_rot_[2]), lepra::String("InheritRotNode")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("InheritScl: [%i, %i, %i]", node_tm->inherit_scl_[0], node_tm->inherit_scl_[1], node_tm->inherit_scl_[2]), lepra::String("InheritSclNode")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TMPos: [%f, %f, %f]", node_tm->tm_pos_[0], node_tm->tm_pos_[1], node_tm->tm_pos_[2]), lepra::String("TMPosNode")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TMRotAngle: %f", node_tm->tm_rot_angle_), lepra::String("TMRotAngleNode")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TMRotAxis: [%f, %f, %f]", node_tm->tm_rot_axis_[0], node_tm->tm_rot_axis_[1], node_tm->tm_rot_axis_[2]), lepra::String("TMRotAxisNode")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TMRow0: [%f, %f, %f]", node_tm->tm_row0_[0], node_tm->tm_row0_[1], node_tm->tm_row0_[2]), lepra::String("TMRow0Node")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TMRow1: [%f, %f, %f]", node_tm->tm_row1_[0], node_tm->tm_row1_[1], node_tm->tm_row1_[2]), lepra::String("TMRow1Node")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TMRow2: [%f, %f, %f]", node_tm->tm_row2_[0], node_tm->tm_row2_[1], node_tm->tm_row2_[2]), lepra::String("TMRow2Node")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TMRow3: [%f, %f, %f]", node_tm->tm_row3_[0], node_tm->tm_row3_[1], node_tm->tm_row3_[2]), lepra::String("TMRow3Node")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TMScale: [%f, %f, %f]", node_tm->tm_scale_[0], node_tm->tm_scale_[1], node_tm->tm_scale_[2]), lepra::String("TMScaleNode")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TMScaleAxis: [%f, %f, %f]", node_tm->tm_scale_axis_[0], node_tm->tm_scale_axis_[1], node_tm->tm_scale_axis_[2]), lepra::String("TMScaleAxisNode")));
		node_tm_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TMScaleAxisAng: %f", node_tm->tm_scale_axis_ang_), lepra::String("TMScaleAxisAngNode")));

		node_tm_list_node->AddChildNode(node_tm_node);
	}

	return node_tm_list_node;
}

TreeNode* ASEFileConverter::ASEViewer::CreateTMAnimationNode(const ASELoader::TMAnimation& tm_animation) {
	TreeNode* tm_animation_node = new TreeNode(lepra::String("TMAnimation"), lepra::String("TMAnimationNode"));

	tm_animation_node->AddChildNode(new TreeNode(lepra::String("NodeName: ") + tm_animation.node_name_.ToCurrentCode(), lepra::String("NodeNameNode")));
	tm_animation_node->AddChildNode(new TreeNode(lepra::String("Comment: ") + tm_animation.comment_.ToCurrentCode(), lepra::String("CommentNode")));

	TreeNode* control_pos_track_node = new TreeNode(lepra::String("ControlPosTrack"), lepra::String("ControlPosTrackNode"));
	TreeNode* control_rot_track_node = new TreeNode(lepra::String("ControlRotTrack"), lepra::String("ControlRotTrackNode"));

	lepra::String format_string("ControlPosSample%.");
	format_string += (lepra::int64)GetNumDigits(tm_animation.control_pos_track_.GetCount());
	format_string += "i";

	int count;
	ASELoader::TMAnimation::ControlPosSampleList::ConstIterator pos_iter;
	for(pos_iter = tm_animation.control_pos_track_.First(), count = 1; pos_iter != tm_animation.control_pos_track_.End(); ++pos_iter, ++count) {
		lepra::String name(lepra::StringUtility::Format(format_string.c_str(), count));
		control_pos_track_node->AddChildNode(new TreeNode(name, lepra::String(name + "Node")));
	}

	format_string = "ControlRotSample%.";
	format_string += (lepra::int64)GetNumDigits(tm_animation.control_pos_track_.GetCount());
	format_string += "i";

	ASELoader::TMAnimation::ControlRotSampleList::ConstIterator rot_iter;
	for(rot_iter = tm_animation.control_rot_track_.First(), count = 1; rot_iter != tm_animation.control_rot_track_.End(); ++rot_iter, ++count) {
		lepra::String name(lepra::StringUtility::Format(format_string.c_str(), count));
		control_rot_track_node->AddChildNode(new TreeNode(name, lepra::String(name + "Node")));
	}

	tm_animation_node->AddChildNode(control_pos_track_node);
	tm_animation_node->AddChildNode(control_rot_track_node);

	return tm_animation_node;
}

TreeNode* ASEFileConverter::ASEViewer::CreateLightSettingsNode(const ASELoader::LightSettings* light_settings) {
	TreeNode* light_settings_node = new TreeNode(lepra::String("LightSettings"), lepra::String("LightSettingsNode"));

	light_settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TimeValue: %i", light_settings->time_value_), lepra::String("TimeValueNode")));
	light_settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("Color: [%f, %f, %f]", light_settings->color_[0], light_settings->color_[1], light_settings->color_[2]), lepra::String("ColorNode")));
	light_settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("Intens:   %f", light_settings->intens_), lepra::String("IntensNode")));
	light_settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("Aspect:   %f", light_settings->aspect_), lepra::String("AspectNode")));
	light_settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("HotSpot:  %f", light_settings->hot_spot_), lepra::String("HotSpotNode")));
	light_settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("Falloff:  %f", light_settings->falloff_), lepra::String("FalloffNode")));
	light_settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("TDist:    %f", light_settings->t_dist_), lepra::String("TDistNode")));
	light_settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("MapBias:  %f", light_settings->map_bias_), lepra::String("MapBiasNode")));
	light_settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("MapRange: %f", light_settings->map_range_), lepra::String("MapRangeNode")));
	light_settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("MapSize:  %f", light_settings->map_size_), lepra::String("MapSizeNode")));
	light_settings_node->AddChildNode(new TreeNode(lepra::StringUtility::Format("RayBias:  %f", light_settings->ray_bias_), lepra::String("RayBiasNode")));

	return light_settings_node;
}

TreeNode* ASEFileConverter::ASEViewer::CreateMeshListNode(const ASELoader::GeomObject::MeshList& mesh_list) {
	TreeNode* mesh_list_node = new TreeNode(lepra::String("MeshList"), lepra::String("MeshListNode"));

	lepra::String format_string("Mesh%.");
	format_string += (lepra::int64)GetNumDigits(mesh_list.GetCount());
	format_string += "i";

	ASELoader::GeomObject::MeshList::ConstIterator iter;
	int count = 1;
	for (iter = mesh_list.First(); iter != mesh_list.End(); ++iter, ++count) {
		TreeNode* mesh_node = new TreeNode(lepra::StringUtility::Format(format_string.c_str(), count), lepra::String("MeshNode"));
		//ASELoader::Mesh* mesh = *iter;

		//mesh->

		mesh_list_node->AddChildNode(mesh_node);
	}

	return mesh_list_node;
}

TreeNode* ASEFileConverter::ASEViewer::CreateSubMaterialListNode(const ASELoader::Material::MaterialList& sub_mat_list) {
	TreeNode* sub_material_list_node = new TreeNode(lepra::String("SubMaterialList"), lepra::String("SubMaterialListNode"));

	sub_mat_list;

	return sub_material_list_node;
}

TreeNode* ASEFileConverter::ASEViewer::CreateMapListNode(const ASELoader::Material::MapList& map_list, const lepra::String& node_name) {
	TreeNode* map_list_node = new TreeNode(node_name, lepra::String("MapListNode"));

	map_list;

	return map_list_node;
}

int ASEFileConverter::ASEViewer::GetNumDigits(int value) {
	int num_digits = 0;
	do {
		num_digits++;
		value /= 10;
	} while (value != 0);

	return num_digits;
}

lepra::String ASEFileConverter::ASEViewer::ToString(bool value) {
	if (value) {
		return lepra::String("true");
	} else {
		return lepra::String("false");
	}
}

}
