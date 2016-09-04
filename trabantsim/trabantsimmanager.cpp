
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "trabantsimmanager.h"
#include <algorithm>
#include <iterator>
#include "../cure/include/contextmanager.h"
#include "../cure/include/timemanager.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/random.h"
#include "../lepra/include/socket.h"
#include "../lepra/include/systemmanager.h"
#include "../lepra/include/timelogger.h"
#include "../lepra/include/unordered.h"
#include "../life/lifeclient/gameclientmasterticker.h"
#include "../life/lifeclient/uiconsole.h"
#include "../tbc/include/physicsengine.h"
#include "../uicure/include/uicollisionsoundmanager.h"
#include "../uicure/include/uigameuimanager.h"
#include "../uicure/include/uiiconbutton.h"
#include "../uicure/include/uimachine.h"
#include "../uicure/include/uisoundreleaser.h"
#include "../uilepra/include/uitouchdrag.h"
#include "../uilepra/include/uitouchstick.h"
#include "../uitbc/include/gui/uidesktopwindow.h"
#include "../uitbc/include/gui/uifixedlayouter.h"
#include "../uitbc/include/uimaterial.h"
#include "../uitbc/include/uiparticlerenderer.h"
#include "../uitbc/include/uitrianglebasedgeometry.h"
#include "fileserver.h"
#include "light.h"
#include "object.h"
#include "rtvar.h"
#include "trabantsim.h"
#include "trabantsimconsolemanager.h"
#include "version.h"

#define BG_COLOR		Color(25, 35, 45, 190)
#define BRIGHT_TEXT		Color(220, 215, 205)
#define GREEN_BUTTON		Color(20, 190, 15)


namespace TrabantSim {



void FoldSuspendSimulator();
void UnfoldSimulator();
void DidSyncFiles();
bool ConnectQuery(const str& hostname);



struct AppSyncDelegate: SyncDelegate {
	virtual bool WillSync(const str& hostname) {
		return ConnectQuery(hostname);
	};
	virtual void DidSync() {
		DidSyncFiles();
	};
};



TrabantSimManager::JoystickData::JoystickData(int joystick_id, float px, float py):
	joystick_id_(joystick_id),
	x(px),
	y(py) {
}

TrabantSimManager::TouchstickInfo::TouchstickInfo(Touchstick* stick, float px, float py, int orientation, bool is_sloppy):
	stick_(stick),
	x(px),
	y(py),
	orientation_(orientation),
	is_sloppy_(is_sloppy) {
}

TrabantSimManager::EngineTarget::EngineTarget(int instance_id, float strength):
	instance_id_(instance_id),
	strength_(strength) {
}



TrabantSimManager::TrabantSimManager(life::GameClientMasterTicker* pMaster, const cure::TimeManager* time,
	cure::RuntimeVariableScope* variable_scope, cure::ResourceManager* resource_manager,
	UiCure::GameUiManager* ui_manager, int slave_index, const PixelRect& render_area):
	Parent(pMaster, time, variable_scope, resource_manager, ui_manager, slave_index, render_area),
	collision_sound_manager_(0),
	is_mouse_controlled_(false),
	set_focus_(false),
	set_cursor_visible_(true),
	set_cursor_invisible_(false),
	menu_(0),
	light_(0),
	camera_transform_(quat(), vec3(0, -3, 0)),
	pause_button_(0),
	back_button_(0),
	last_sound_volume_(1.0),
	master_volume_(1.0),
	is_paused_(false),
	is_controlled_(false),
	was_controlled_(false),
	hide_counter_(0),
	is_control_timeout_(false),
	command_socket_(0),
	command_thread_(0),
	file_server_(0),
	user_info_dialog_(0),
	user_info_label_(0) {
	ui_manager_->SetVariableScope(variable_scope);

	collision_sound_manager_ = new UiCure::CollisionSoundManager(this, ui_manager);
	collision_sound_manager_->SetScale(1, 0.5f, 0.08f, 0.2f);
	collision_sound_manager_->AddSound("explosion",	UiCure::CollisionSoundManager::SoundResourceInfo(0.8f, 0.1f, 0));
	collision_sound_manager_->AddSound("rubber",		UiCure::CollisionSoundManager::SoundResourceInfo(1.0f, 0.1f, 0));

	SetConsoleManager(new TrabantSimConsoleManager(GetResourceManager(), this, ui_manager_, GetVariableScope(), render_area_));

	GetPhysicsManager()->SetSimulationParameters(0.0f, 0.03f, 0.2f);

	SocketAddress address;
	const strutil::strvec& args = SystemManager::GetArgumentVector();
	if (args.size() <= 1 || !address.Resolve(args[args.size()-1])) {
		address.Resolve("0.0.0.0:2541");
	}
	open_local_address_ = address;
	SocketAddress internal_address;
	internal_address.Resolve("127.0.0.1:2541");
	internal_address.SetPort(address.GetPort());
	internal_local_address_ = internal_address;
	bool allow_remote_sync;
	v_get(allow_remote_sync, =, UiCure::GetSettings(), "Simulator.AllowRemoteSync", false);
	if (!allow_remote_sync) {
		address = internal_address;
	}
	local_address_ = address;
	last_remote_address_ = address;
	last_accepted_address_ = address;

	file_server_ = new FileServer(new AppSyncDelegate);

	Resume(true);
}

TrabantSimManager::~TrabantSimManager() {
	CloseConnection();
	delete file_server_;
	file_server_ = 0;

	Close();

	delete collision_sound_manager_;
	collision_sound_manager_ = 0;
}



void TrabantSimManager::Resume(bool hard) {
	(void)hard;
	is_control_timeout_ = false;
	is_controlled_ = false;
	was_controlled_ = false;
	startup_timer_.PopTimeDiff();
	v_set(GetVariableScope(), kRtvarGameUsermessage, " ");

	bool allow_remote_sync;
	v_get(allow_remote_sync, =, UiCure::GetSettings(), "Simulator.AllowRemoteSync", false);
	if (allow_remote_sync) {
		file_server_->Start();
	}

	OpenConnection();
}

void TrabantSimManager::Suspend(bool hard) {
	is_control_timeout_ = false;
	v_set(GetVariableScope(), kRtvarGameUsermessage, " ");

	if (command_socket_ && command_socket_->IsOpen()) {
		if (last_remote_address_ != internal_local_address_) {
			command_socket_->SendTo((const unsigned char*)"disconnect\n", 11, last_remote_address_);
		}
	}
	if (hard) {
		CloseConnection();
	}
}

void TrabantSimManager::LoadSettings() {
	Parent::LoadSettings();
	GetConsoleManager()->ExecuteCommand("bind-key F5 \"\"");
}

void TrabantSimManager::RefreshOptions() {
	Parent::RefreshOptions();

	double font_height;
	v_get(font_height, =, cure::GetSettings(), kRtvarUi2DFontheight, 30.0);
	font_height *= ui_manager_->GetCanvas()->GetHeight()/500.0;
	v_override(GetVariableScope(), kRtvarUi2DFontheight, font_height);
	ui_manager_->UpdateSettings();
}


void TrabantSimManager::UserReset() {
	set_focus_ = true;
	set_cursor_visible_ = true;
	is_mouse_controlled_ = false;

	GetConsoleManager()->ExecuteCommand("bind-key F5 \"\"");

	cure::RuntimeVariableScope* scope = GetVariableScope();
	const std::list<str> variable_list = scope->GetVariableNameList(cure::RuntimeVariableScope::kSearchExportable);
	std::list<str>::const_iterator x = variable_list.begin();
	for (; x != variable_list.end(); ++x) {
		const str name = *x;
		if (strutil::StartsWith(name, "Ui.3D.Clear") || strutil::StartsWith(name, "Ui.3D.Clip") || strutil::StartsWith(name, "Ui.Pen")) {
			continue;
		}
		scope->ResetDefaultValue(name);
	}
	camera_angle_.Set(0,0,0);
	camera_velocity_.Set(0,0,0);

	GetResourceManager()->ForceFreeCache();
	GetResourceManager()->ForceFreeCache();
	GetResourceManager()->ForceFreeCache();

	ScopeLock game_lock(GetTickLock());

	collision_list_.clear();
	drag_list_.clear();
	ui_manager_->GetDragManager()->ClearDrags(ui_manager_->GetInputManager());

	TouchstickList::iterator y = touchstick_list_.begin();
	for (; y != touchstick_list_.end(); ++y) {
		delete y->stick_;
	}
	touchstick_list_.clear();
}

int TrabantSimManager::CreateObject(const quat& orientation, const vec3& position, const MeshObject& gfx_object, const PhysObjectArray& phys_objects,
					ObjectMaterial material, bool is_static, bool is_trigger) {
	quat pq, rootq;
	Object* _object = (Object*)CreateContextObject("object");
	tbc::ChunkyPhysics* physics = new tbc::ChunkyPhysics(tbc::BoneHierarchy::kTransformLocal2World, is_static? tbc::ChunkyPhysics::kStatic : tbc::ChunkyPhysics::kDynamic);
	physics->SetGuideMode(tbc::ChunkyPhysics::kGuideAlways);
	if (phys_objects.empty()) {
		_object->SetPhysicsTypeOverride(cure::kPhysicsOverrideBones);
		physics->SetBoneCount(1);
		tbc::ChunkyBoneGeometry::BodyData bone_data(0,0,0);
		tbc::ChunkyBoneGeometry* bone = new tbc::ChunkyBoneSphere(bone_data);
		physics->AddBoneGeometry(xform(orientation,position), bone);
	} else {
		float _friction, bounce;
		v_get(_friction, =(float), GetVariableScope(), kRtvarPhysicsFriction, 0.5);
		v_get(bounce, =(float), GetVariableScope(), kRtvarPhysicsBounce, 0.2);

		physics->SetBoneCount((int)phys_objects.size());
		PhysObjectArray::const_iterator x = phys_objects.begin();
		int y = 0;
		tbc::ChunkyBoneGeometry* parent = 0;
		PlacedObject* parent_phys_object = 0;
		for (; x != phys_objects.end(); ++x, ++y) {
			tbc::ChunkyBoneGeometry::BodyData bone_data(y==0? 15.0f:1.0f, _friction, bounce, parent, tbc::ChunkyBoneGeometry::kJointExclude, tbc::ChunkyBoneGeometry::kConnectee3Dof);
			bone_data.bone_type_ = is_trigger? tbc::ChunkyBoneGeometry::kBoneTrigger : tbc::ChunkyBoneGeometry::kBoneBody;
			PlacedObject* phys_object = *x;
			xform t = xform(phys_object->orientation_, phys_object->pos_);
			if (parent_phys_object) {
				t.position_ = rootq.GetInverse() * t.position_;
				t.orientation_ = rootq.GetInverse() * t.orientation_;
			} else {
				rootq = t.orientation_;
				t.orientation_ = orientation * t.orientation_;
				t.position_ += position;
				pq = t.orientation_;
			}
			BoxObject* box = dynamic_cast<BoxObject*>(phys_object);
			SphereObject* sphere = dynamic_cast<SphereObject*>(phys_object);
			CapsuleObject* capsule = dynamic_cast<CapsuleObject*>(phys_object);
			MeshObject* _mesh = dynamic_cast<MeshObject*>(phys_object);
			if (box) {
				tbc::ChunkyBoneBox* bone = new tbc::ChunkyBoneBox(bone_data);
				bone->SetMaterial("rubber");
				bone->size_ = box->size_;
				physics->AddBoneGeometry(t, bone, parent);
			} else if (sphere) {
				tbc::ChunkyBoneSphere* bone = new tbc::ChunkyBoneSphere(bone_data);
				bone->SetMaterial("rubber");
				bone->radius_ = sphere->radius_;
				physics->AddBoneGeometry(t, bone, parent);
			} else if (capsule) {
				tbc::ChunkyBoneCapsule* bone = new tbc::ChunkyBoneCapsule(bone_data);
				bone->SetMaterial("rubber");
				bone->radius_ = capsule->radius_;
				bone->length_ = capsule->length_;
				physics->AddBoneGeometry(t, bone, parent);
			} else if (_mesh) {
				if (_mesh->indices_.size() <= 3) {
					log_.Warningf("Need two triangles or more (%i indices is too few) to create physics mesh.", _mesh->indices_.size());
					delete physics;
					return -1;
				}
				tbc::ChunkyBoneMesh* bone = new tbc::ChunkyBoneMesh(bone_data);
				bone->SetMaterial("rubber");
				bone->vertex_count_ = (int)_mesh->vertices_.size()/3;
				bone->triangle_count_ = (int)_mesh->indices_.size()/3;
				bone->vertices_ = new float[bone->vertex_count_*3];
				bone->indices_ = new uint32[bone->triangle_count_*3];
				for (unsigned z = 0; z < bone->vertex_count_; ++z) {
					bone->vertices_[z*3+0] = _mesh->vertices_[z*3+0];
					bone->vertices_[z*3+1] = _mesh->vertices_[z*3+1];
					bone->vertices_[z*3+2] = _mesh->vertices_[z*3+2];
				}
				for (unsigned z = 0; z < bone->triangle_count_; ++z) {
					bone->indices_[z*3+0] = _mesh->indices_[z*3+0];
					bone->indices_[z*3+1] = _mesh->indices_[z*3+1];
					bone->indices_[z*3+2] = _mesh->indices_[z*3+2];
				}
				physics->AddBoneGeometry(t, bone, parent);
			}
			if (y == 0) {
				parent = physics->GetBoneGeometry(0);
				parent_phys_object = phys_object;
			}
		}
	}
	_object->SetRootOrientation(pq);
	if (!gfx_object.vertices_.empty() && !gfx_object.indices_.empty()) {
		float r,g,b,a;
		if (material == MaterialChecker) {
			r = g = b = a = 1;
		} else {
			v_get(r, =(float), GetVariableScope(), kRtvarUiPenred, 0.5);
			v_get(g, =(float), GetVariableScope(), kRtvarUiPengreen, 0.5);
			v_get(b, =(float), GetVariableScope(), kRtvarUiPenblue, 0.5);
			v_get(a, =(float), GetVariableScope(), kRtvarUiPenalpha, 1.0);
		}
		vec3 _color(r,g,b);
		const bool is_smooth = (material == MaterialSmooth);
		uitbc::TriangleBasedGeometry* _mesh = _object->CreateGfxMesh(gfx_object.vertices_, gfx_object.indices_, _color, a, is_smooth);
		if (!_mesh) {
			delete _object;
			return -1;
		}
		const str texture = (material==MaterialChecker)? "checker.png" : "noise.png";
		if (material == MaterialChecker || material == MaterialNoise) {
			const float _scale = (fabs(gfx_object.vertices_[gfx_object.vertices_.size()-3]-gfx_object.vertices_[0]) >= 60)? 20.0f : 2.0f;
			AddCheckerTexturing(_mesh, _scale);
			_object->LoadTexture(texture);
		}
		_object->AddMeshResource(_mesh, is_static? -1 : 1);
		_object->AddMeshInfo(_object->GetMeshResource(0)->GetName(), "texture", texture, _color, a);
		_object->GetMeshResource(0)->offset_.offset_.orientation_ = gfx_object.orientation_;
		_object->GetMeshResource(0)->offset_.offset_.position_ = gfx_object.pos_;
	}
	_object->initial_orientation_ = pq;
	_object->initial_inverse_orientation_ = pq.GetInverse();
	_object->generated_physics_ = physics;

	cure::GameObjectId object_id = GetContext()->AllocateGameObjectId(cure::kNetworkObjectLocalOnly);
	ScopeLock object_lock(&objects_lock_);
	created_objects_.insert(ContextObjectTable::value_type(object_id, _object));
	return object_id;
}

void TrabantSimManager::CreateClones(IntList& created_object_ids, int original_id, const XformList& placements, ObjectMaterial material, bool is_static) {
	str mesh_name;
	str phys_name;
	float r,g,b,a;
	if (material == MaterialChecker) {
		r = g = b = a = 1;
	} else {
		v_get(r, =(float), GetVariableScope(), kRtvarUiPenred, 0.5);
		v_get(g, =(float), GetVariableScope(), kRtvarUiPengreen, 0.5);
		v_get(b, =(float), GetVariableScope(), kRtvarUiPenblue, 0.5);
		v_get(a, =(float), GetVariableScope(), kRtvarUiPenalpha, 1.0);
	}
	const vec3 _color(r,g,b);
	const bool is_smooth = (material == MaterialSmooth);

	LEPRA_MEASURE_SCOPE(CreateClonesLock);
	ScopeLock game_lock(GetTickLock());
	Object* original = (Object*)GetContext()->GetObject(original_id);
	if (!original) {
		return;
	}
	mesh_name = original->GetMeshResource(0)->GetName();
	mesh_name.resize(mesh_name.find(".mesh"));
	phys_name = original->GetPhysicsResource()->GetName();
	phys_name.resize(phys_name.find(".phys"));
	xform original_xform = original->GetPhysics()->GetOriginalBoneTransformation(0);
	quat original_offset_orientation = original->GetMeshResource(0)->offset_.offset_.orientation_;

	// Tricky loop to ensure we don't hold any of the mutexes very long.
	while (!GetMaster()->GetPhysicsLock()->TryAcquire()) {
		game_lock.Release();
		Thread::YieldCpu();
		game_lock.Acquire();
	}
	{
		for (XformList::const_iterator x = placements.begin(); x != placements.end(); ++x) {
			Object* _object = (Object*)Parent::CreateContextObject("object", cure::kNetworkObjectLocallyControlled, 0);
			_object->SetPhysicsTypeOverride(is_static? cure::kPhysicsOverrideStatic : cure::kPhysicsOverrideDynamic);
			{
				const quat pq = x->orientation_;
				_object->SetRootOrientation(pq);
				_object->SetRootPosition(x->position_ - original_xform.GetPosition());
				{
					LEPRA_MEASURE_SCOPE(CreateClonesPhys);
					_object->CreatePhysicsRef(phys_name);
				}
				const str texture = (material==MaterialChecker)? "checker.png" : "noise.png";
				if (material == MaterialChecker || material == MaterialNoise) {
					LEPRA_MEASURE_SCOPE(CreateClonesTexture);
					_object->LoadTexture(texture);
				}
				_object->AddMeshInfo(mesh_name, "texture", texture, _color, a, is_smooth);
				{
					LEPRA_MEASURE_SCOPE(CreateClonesMesh);
					_object->AddMeshResourceRef(mesh_name, is_static? -1 : 1);
				}
				_object->GetMeshResource(0)->offset_.offset_.orientation_ = original_offset_orientation;
				_object->initial_orientation_ = pq;
				_object->initial_inverse_orientation_ = pq.GetInverse();
				objects_.insert(_object->GetInstanceId());
				created_object_ids.push_back(_object->GetInstanceId());
			}
		}
	}
	GetMaster()->GetPhysicsLock()->Release();
}

void TrabantSimManager::DeleteObject(int object_id) {
	ScopeLock object_lock(&objects_lock_);
	deleted_objects_.insert(object_id);
}

void TrabantSimManager::DeleteAllObjects() {
	GetContext()->SetPostKillTimeout(1);
	{
		ScopeLock object_lock(&objects_lock_);
		deleted_objects_.clear();
		ContextObjectTable::iterator x = created_objects_.begin();
		for (; x != created_objects_.end(); ++x) {
			Object* object = (Object*)x->second;
			delete object;
		}
	}
	{
		ScopeLock game_lock(GetTickLock());
		std::set<cure::GameObjectId>::iterator x;
		for (x = objects_.begin(); x != objects_.end(); ++x) {
			GetContext()->PostKillObject(*x);
		}
		objects_.clear();
		GetResourceManager()->ForceFreeCache();
	}
	int stable_count = 0;
	size_t last_count = 0;
	for (int x = 0;; ++x) {
		Thread::Sleep(0.1);
		ScopeLock game_lock(GetTickLock());
		const size_t count = GetContext()->GetObjectTable().size();
		if (count == last_count) {
			++stable_count;
		} else {
			stable_count = 0;
		}
		last_count = count;
		if (count == 0 || stable_count >= 5) {
			v_set(GetVariableScope(), kRtvarGameUsermessage, " ");
			if (count) {
				log_.Warningf("Unable to delete all objects, %i remaining.", count);
			} break;
		}
		if (x > 5) {
			v_set(GetVariableScope(), kRtvarGameUsermessage, "Cleaning up...");
		}
	}
	GetContext()->SetPostKillTimeout(0.01);
	GetResourceManager()->SetInjectTimeLimit(0.01);
}

void TrabantSimManager::PickObjects(const vec3& position, const vec3& direction, const vec2& range, IntList& picked_object_ids, Vec3List& picked_positions) {
	ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
	ScopeLock game_lock(GetTickLock());
	vec3 _direction = direction.GetNormalized();
	vec3 _position = position + _direction*range.x;
	int hit_objects[16];
	vec3 hit_positions[16];
	const int hits = GetPhysicsManager()->QueryRayPick(_position, _direction, range.y-range.x, hit_objects, hit_positions, LEPRA_ARRAY_COUNT(hit_objects));
	std::copy(&hit_objects[0], &hit_objects[hits], std::back_inserter(picked_object_ids));
	std::copy(&hit_positions[0], &hit_positions[hits], std::back_inserter(picked_positions));
}

bool TrabantSimManager::IsLoaded(int object_id) {
	ScopeLock game_lock(GetTickLock());
	return !!GetContext()->GetObject(object_id);
}

void TrabantSimManager::Explode(const vec3& pos, const vec3& vel, float strength, float volume) {
	ScopeLock game_lock(GetTickLock());

	collision_sound_manager_->OnCollision(strength*volume, pos, 0, "explosion");

	const float keep_on_going_factor = 1.0f;	// How much of the velocity energy, [0;1], should be transferred to the explosion particles.
	const int particles = std::max(10, Math::Lerp(8, 20, strength * 0.2f));
	const int fires    = particles;
	const int smokes   = particles;
	const int sparks   = particles/2;
	const int shrapnel = particles/3;
	vec3 start_fire_color(1.0f, 1.0f, 0.3f);
	vec3 fire_color(0.6f, 0.4f, 0.2f);
	vec3 start_smoke_color(0.4f, 0.4f, 0.4f);
	vec3 smoke_color(0.2f, 0.2f, 0.2f);
	vec3 shrapnel_color(0.3f, 0.3f, 0.3f);	// Default debris color is gray.
	uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
	particle_renderer->CreateExplosion(pos, strength, vel*keep_on_going_factor, 1, 1, start_fire_color, fire_color, start_smoke_color, smoke_color, shrapnel_color, fires, smokes, sparks, shrapnel);
}

void TrabantSimManager::PlaySound(const str& sound, const vec3& pos, const vec3& vel, float volume) {
	ScopeLock game_lock(GetTickLock());
	UiCure::UserSound3dResource* sound_resource = new UiCure::UserSound3dResource(ui_manager_, uilepra::SoundManager::kLoopNone);
	new UiCure::SoundReleaser(GetResourceManager(), ui_manager_, GetContext(), sound, sound_resource, pos, vel, volume, 1);
}

void TrabantSimManager::PopCollisions(CollisionList& collision_list) {
	{
		ScopeLock game_lock(GetTickLock());
		collision_list.splice(collision_list.end(), collision_list_);
	}

	// Reduce the number of collisions to avoid bandwidth problems.
	typedef std::unordered_map<unsigned,CollisionInfo> CollisionMap;
	CollisionMap reduced_collisions;
	for (CollisionList::iterator x = collision_list.begin(); x != collision_list.end(); ++x) {
		unsigned y = x->object_id_;
		unsigned z = x->other_object_id_;
		unsigned key = ((y^z)<<16) + (y+z);
		CollisionMap::iterator c = reduced_collisions.find(key);
		if (c != reduced_collisions.end()) {
			CollisionInfo& collision = c->second;
			if (collision.force_*x->force_ > 0) {
				collision.force_ = (collision.force_+x->force_)*0.5f;
				collision.position_ = (collision.position_+x->position_)*0.5f;
			}
		} else {
			reduced_collisions.insert(CollisionMap::value_type(key, *x));
		}
	}
	collision_list.clear();
	for (CollisionMap::iterator x = reduced_collisions.begin(); x != reduced_collisions.end(); ++x) {
		collision_list.push_back(x->second);
	}
}

void TrabantSimManager::GetKeys(strutil::strvec& keys) {
	ScopeLock game_lock(GetTickLock());
	for (KeyMap::iterator x = key_map_.begin(); x != key_map_.end();) {
		keys.push_back(uilepra::InputManager::GetKeyName(x->first));
		if (x->second) {
			x = key_map_.erase(x);
		} else {
			++x;
		}
	}
}

void TrabantSimManager::GetTouchDrags(DragList& drag_list) {
	ScopeLock game_lock(GetTickLock());
	drag_list = drag_list_;

	for (DragEraseList::iterator x = drag_erase_list_.begin(); x != drag_erase_list_.end(); ++x) {
		for (DragList::iterator y = drag_list_.begin(); y != drag_list_.end();) {
			if (y->start_.x == x->x && y->start_.y == x->y) {
				y = drag_list_.erase(y);
			} else {
				++y;
			}
		}
	}
	drag_erase_list_.clear();
}

vec3 TrabantSimManager::GetAccelerometer() const {
	return ui_manager_->GetAccelerometer();
}

vec3 TrabantSimManager::GetMouseMove() {
#ifdef LEPRA_TOUCH
	return vec3();
#else // Computer
	if (!is_mouse_controlled_) {
		is_mouse_controlled_ = true;
		set_cursor_invisible_ = true;
	}
	vec3 m(mouse_move_);
	mouse_move_.Set(0,0,0);
	if (((TrabantSimConsoleManager*)GetConsoleManager())->GetUiConsole()->IsVisible()) {
		m.Set(0,0,0);
	}
	return m;
#endif // touch device / computer
}

int TrabantSimManager::CreateJoystick(float x, float y, bool is_sloppy) {
	ScopeLock game_lock(GetTickLock());
	const float touch_side_scale = 1.28f;	// Inches.
	const float touch_scale = touch_side_scale / (float)ui_manager_->GetDisplayManager()->GetPhysicalScreenSize();
	const int screen_pixel_width = ui_manager_->GetDisplayManager()->GetWidth();
	const int minimum_touch_radius = (int)(screen_pixel_width*touch_scale*0.17f);	// 30 pixels in iPhone classic.
	PixelRect rect(0,0,10,10);
	Touchstick::InputMode mode = is_sloppy? Touchstick::kModeRelativeCenterNospring : Touchstick::kModeRelativeCenter;
	Touchstick* _stick = new Touchstick(ui_manager_->GetInputManager(), mode, rect, 0, minimum_touch_radius);
	const str name = strutil::Format("Touchstick%i", touchstick_list_.size());
	_stick->SetUniqueIdentifier(name);
	touchstick_list_.push_back(TouchstickInfo(_stick, x, y, -90, is_sloppy));
	touchstick_timer_.ReduceTimeDiff(-10);
	return (int)touchstick_list_.size()-1;
}

TrabantSimManager::JoystickDataList TrabantSimManager::GetJoystickData() const {
	ScopeLock game_lock(GetTickLock());
	JoystickDataList __list;
	int id = 0;
	TouchstickList::const_iterator x = touchstick_list_.begin();
	for (; x != touchstick_list_.end(); ++x, ++id) {
		float jx = 0;
		float jy = 0;
		bool is_pressing = false;
		x->stick_->GetValue(jx, jy, is_pressing);
		if (is_pressing) {
			__list.push_back(JoystickData(id, jx, -jy));
		}
		if (!x->is_sloppy_) {
			x->stick_->ResetTap();
		}
	}
	return __list;
}

float TrabantSimManager::GetAspectRatio() const {
	return ui_manager_->GetDisplayManager()->GetWidth()/(float)ui_manager_->GetDisplayManager()->GetHeight();
}

int TrabantSimManager::CreateEngine(int object_id, const str& engine_type, const vec2& max_velocity, float strength, float friction, const EngineTargetList& engine_targets) {
	ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
	ScopeLock game_lock(GetTickLock());
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	if (!_object) {
		return -1;
	}

	bool is_attachment = false;
	tbc::PhysicsEngine::EngineType _engine_type;
	vec2 _max_velocity(max_velocity);
	float _strength = strength * _object->GetMass();
	float _friction = friction*2;
	if (engine_type == "roll_turn") {
		_engine_type = tbc::PhysicsEngine::kEngineHinge2Turn;
		is_attachment = true;
	} else if (engine_type == "roll") {
		_engine_type = tbc::PhysicsEngine::kEngineHingeRoll;
		is_attachment = true;
	} else if (engine_type == "walk_abs") {
		_engine_type = tbc::PhysicsEngine::kEngineWalk;
	} else if (engine_type == "push_abs") {
		_engine_type = tbc::PhysicsEngine::kEnginePushAbsolute;
	} else if (engine_type == "push_rel") {
		_engine_type = tbc::PhysicsEngine::kEnginePushRelative;
	} else if (engine_type == "push_turn_abs") {
		_engine_type = tbc::PhysicsEngine::kEnginePushTurnAbsolute;
	} else if (engine_type == "push_turn_rel") {
		_engine_type = tbc::PhysicsEngine::kEnginePushTurnRelative;
	} else if (engine_type == "vel_abs_xy") {
		_engine_type = tbc::PhysicsEngine::kEngineVelocityAbsoluteXY;
	} else if (engine_type == "gyro") {
		_engine_type = tbc::PhysicsEngine::kEngineHingeGyro;
		is_attachment = true;
	} else if (engine_type == "rotor") {
		_engine_type = tbc::PhysicsEngine::kEngineRotor;
		is_attachment = true;
	} else if (engine_type == "tilt") {
		_engine_type = tbc::PhysicsEngine::kEngineRotorTilt;
		is_attachment = true;
	} else if (engine_type == "slider") {
		_engine_type = tbc::PhysicsEngine::kEngineSliderForce;
		is_attachment = true;
	} else if (engine_type == "stabilize") {
		_engine_type = tbc::PhysicsEngine::kEngineStabilize;
		_friction = friction;
	} else if (engine_type == "upright_stabilize") {
		_engine_type = tbc::PhysicsEngine::kEngineUprightStabilize;
		_friction = friction;
	} else if (engine_type == "forward_stabilize") {
		_engine_type = tbc::PhysicsEngine::kEngineForwardStabilize;
		_friction = friction;
	} else {
		return -1;
	}

	EngineTargetList targets(engine_targets);
	if (targets.empty()) {
		Object::Array objects = _object->GetAttachedObjects();
		cure::ContextObject* last_attached_object = objects.empty()? 0 : objects.back();
		cure::ContextObject* engine_object = is_attachment? last_attached_object : _object;
		if (!engine_object) {
			log_.Warningf("No object attached to create a %s engine to.", engine_type.c_str());
			return -1;
		}
		targets.push_back(EngineTarget(engine_object->GetInstanceId(),1));
	}

	tbc::PhysicsEngine* engine = new tbc::PhysicsEngine(_engine_type, _strength, _max_velocity.x, _max_velocity.y, _friction, _object->GetPhysics()->GetEngineCount()*4);
	for (EngineTargetList::iterator x = targets.begin(); x != targets.end(); ++x) {
		cure::ContextObject* engine_object = GetContext()->GetObject(x->instance_id_);
		tbc::ChunkyBoneGeometry* geometry = engine_object->GetPhysics()->GetBoneGeometry(0);
		engine->AddControlledGeometry(geometry, x->strength_);
	}
	_object->GetPhysics()->AddEngine(engine);
	if (is_attachment) {
		for (EngineTargetList::iterator x = targets.begin(); x != targets.end(); ++x) {
			cure::ContextObject* engine_object = GetContext()->GetObject(x->instance_id_);
			_object->AddAttachedObjectEngine(engine_object, engine);
		}
	}
	GetContext()->EnableMicroTickCallback(_object);
	return _object->GetPhysics()->GetEngineCount()-1;
}

int TrabantSimManager::CreateJoint(int object_id, const str& joint_type, int other_object_id, const vec3& axis, const vec2& stop, const vec2& spring_settings) {
	ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
	ScopeLock game_lock(GetTickLock());
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	Object* _object2 = (Object*)GetContext()->GetObject(other_object_id);
	if (!_object || !_object2) {
		return -1;
	}

	tbc::ChunkyBoneGeometry::ConnectorType type;
	float lo_stop = stop.x;
	float hi_stop = stop.y;
	float spring_constant = spring_settings.x;
	float spring_damping = spring_settings.y;
	if (joint_type == "hinge") {
		type = tbc::ChunkyBoneGeometry::kConnectorHinge;
		if (lo_stop == 0 && hi_stop == 0) {
			lo_stop = -100.0f;
			hi_stop = +100.0f;
		}
	} else if (joint_type == "suspend_hinge") {
		type = tbc::ChunkyBoneGeometry::kConnectorSuspendHinge;
		spring_constant = (spring_constant<=0)? 22 : spring_constant;
		spring_damping = (spring_damping<=0)? 0.8f : spring_damping;
		spring_constant *= _object->GetMass() * 100;
	} else if (joint_type == "turn_hinge") {
		type = tbc::ChunkyBoneGeometry::kConnectorHinge2;
		if (lo_stop == 0 && hi_stop == 0) {
			lo_stop = -0.5f;
			hi_stop = +0.5f;
		}
		spring_constant = (spring_constant<=0)? 22 : spring_constant;
		spring_damping = (spring_damping<=0)? 0.8f : spring_damping;
		spring_constant *= _object->GetMass() * 100;
	} else if (joint_type == "ball") {
		type = tbc::ChunkyBoneGeometry::kConnectorBall;
	} else if (joint_type == "slider") {
		type = tbc::ChunkyBoneGeometry::kConnectorSlider;
		if (lo_stop == 0 && hi_stop == 0) {
			lo_stop = -1.0f;
			hi_stop = +1.0f;
		}
	} else if (joint_type == "universal") {
		type = tbc::ChunkyBoneGeometry::kConnectorUniversal;
	} else if (joint_type == "fixed") {
		type = tbc::ChunkyBoneGeometry::kConnectorFixed;
		GetPhysicsManager()->MakeStatic(_object2->GetPhysics()->GetBoneGeometry(0)->GetBodyId());
	} else {
		return -1;
	}

	_object->GetPhysics()->GetBoneGeometry(0)->ClearConnectorTypes();
	_object->GetPhysics()->GetBoneGeometry(0)->AddConnectorType(type);
	tbc::ChunkyBoneGeometry::BodyDataBase& body_data = _object2->GetPhysics()->GetBoneGeometry(0)->GetBodyData();
	body_data.parameter_[tbc::ChunkyBoneGeometry::kParamEulerTheta] = axis.GetPolarCoordAngleZ();
	body_data.parameter_[tbc::ChunkyBoneGeometry::kParamEulerPhi] = axis.GetAngle(vec3(0,0,1));
	body_data.parameter_[tbc::ChunkyBoneGeometry::kParamLowStop] = lo_stop;
	body_data.parameter_[tbc::ChunkyBoneGeometry::kParamHighStop] = hi_stop;
	body_data.parameter_[tbc::ChunkyBoneGeometry::kParamSpringConstant] = spring_constant;
	body_data.parameter_[tbc::ChunkyBoneGeometry::kParamSpringDamping] = spring_damping;
	body_data.parameter_[tbc::ChunkyBoneGeometry::kParamOffsetX] = 0;
	body_data.parameter_[tbc::ChunkyBoneGeometry::kParamOffsetY] = 0;
	body_data.parameter_[tbc::ChunkyBoneGeometry::kParamOffsetZ] = 0;
	_object->AttachToObjectByBodyIndices(0, _object2, 0);
	const float total_mass = _object->GetMass() + _object2->GetMass();
	_object->SetMass(total_mass);	// Avoid collision sounds.
	_object2->SetMass(total_mass);
	return 0;
}

void TrabantSimManager::Position(int object_id, bool _set, vec3& position) {
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	if (!_object || !_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId()) {
		return;
	}
	if (_set) {
		ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
		GetMaster()->GetPhysicsManager(true)->SetBodyPosition(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), position);
		if (_object->GetPhysics()->GetPhysicsType() == tbc::ChunkyPhysics::kStatic) {
			ScopeLock game_lock(GetTickLock());
			_object->UiMove();
		}
	} else {
		position = GetMaster()->GetPhysicsManager(true)->GetBodyPosition(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId());
	}
}

void TrabantSimManager::Orientation(int object_id, bool _set, quat& orientation) {
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	if (!_object || !_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId()) {
		return;
	}
	if (_set) {
		orientation = orientation * _object->initial_orientation_;
		if (orientation.GetNorm() < 0.5) {
			return;
		}
		ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
		GetMaster()->GetPhysicsManager(true)->SetBodyOrientation(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), orientation);
		if (_object->GetPhysics()->GetPhysicsType() == tbc::ChunkyPhysics::kStatic) {
			ScopeLock game_lock(GetTickLock());
			_object->UiMove();
		}
	} else {
		quat _orientation = GetMaster()->GetPhysicsManager(true)->GetBodyOrientation(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId());
		orientation = _orientation * _object->initial_inverse_orientation_;
	}
}

void TrabantSimManager::Velocity(int object_id, bool _set, vec3& velocity) {
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	if (!_object || !_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId()) {
		return;
	}
	if (_set) {
		ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
		GetMaster()->GetPhysicsManager(true)->SetBodyVelocity(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), velocity);
	} else {
		GetMaster()->GetPhysicsManager(true)->GetBodyVelocity(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), velocity);
	}
}

void TrabantSimManager::AngularVelocity(int object_id, bool _set, vec3& angular_velocity) {
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	if (!_object || !_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId()) {
		return;
	}
	if (_set) {
		ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
		GetMaster()->GetPhysicsManager(true)->SetBodyAngularVelocity(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), angular_velocity);
	} else {
		GetMaster()->GetPhysicsManager(true)->GetBodyAngularVelocity(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), angular_velocity);
	}
}

void TrabantSimManager::Force(int object_id, bool _set, vec3& force) {
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	if (!_object || !_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId()) {
		return;
	}
	if (_set) {
		ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
		GetMaster()->GetPhysicsManager(true)->AddForce(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), force);
	}
	else {
		GetMaster()->GetPhysicsManager(true)->GetBodyAcceleration(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), 1, force);
	}
}

void TrabantSimManager::Torque(int object_id, bool _set, vec3& torque) {
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	if (!_object || !_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId()) {
		return;
	}
	if (_set) {
		ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
		GetMaster()->GetPhysicsManager(true)->SetBodyTorque(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), torque);
	}
	else {
		GetMaster()->GetPhysicsManager(true)->GetBodyTorque(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), torque);
	}
}

void TrabantSimManager::Mass(int object_id, bool _set, float& mass) {
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	if (!_object || !_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId()) {
		return;
	}
	if (_set) {
		ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
		GetMaster()->GetPhysicsManager(true)->SetBodyMass(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), mass);
		_object->QueryMass();
	} else {
		mass = GetMaster()->GetPhysicsManager(true)->GetBodyMass(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId());
	}
}

void TrabantSimManager::Scale(int object_id, bool _set, vec3& scale) {
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	if (!_object || !_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId()) {
		return;
	}
	if (_set) {
		ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
		const float prev_scale = _object->GetMeshResource(0)->GetRamData()->GetScale();
		GetMaster()->GetPhysicsManager(true)->Scale(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), scale/prev_scale);
		_object->GetMeshResource(0)->GetRamData()->SetScale(scale.x);
	}
	else {
		scale.Set(1, 1, 1);
	}
}

void TrabantSimManager::ObjectColor(int object_id, bool _set, vec3& color, float alpha) {
	ScopeLock game_lock(GetTickLock());
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	if (!_object) {
		return;
	}
	if (_set) {
		_object->GetMesh(0)->GetBasicMaterialSettings().diffuse_ = color;
		_object->GetMesh(0)->GetBasicMaterialSettings().alpha_ = alpha;
		_object->GetMesh(0)->SetAlwaysVisible(!!alpha);
	} else {
		color = _object->GetMesh(0)->GetBasicMaterialSettings().diffuse_;
	}
}

void TrabantSimManager::EngineForce(int object_id, int engine_index, bool _set, vec3& force) {
	ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
	ScopeLock game_lock(GetTickLock());
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	if (!_object) {
		return;
	}
	if (engine_index < 0 || engine_index >= _object->GetPhysics()->GetEngineCount()) {
		log_.Warningf("Object %i does not have an engine with index %i.", object_id, engine_index);
		return;
	}
	if (_set) {
		switch (_object->GetPhysics()->GetEngine(engine_index)->GetEngineType()) {
			case tbc::PhysicsEngine::kEngineWalk:
			case tbc::PhysicsEngine::kEnginePushAbsolute:
			case tbc::PhysicsEngine::kEnginePushRelative:
			case tbc::PhysicsEngine::kEngineVelocityAbsoluteXY:
				_object->SetEnginePower(engine_index*4+0, force.y);
				_object->SetEnginePower(engine_index*4+1, force.x);
				_object->SetEnginePower(engine_index*4+3, force.z);
				break;
			case tbc::PhysicsEngine::kEnginePushTurnAbsolute:
			case tbc::PhysicsEngine::kEnginePushTurnRelative:
				_object->SetEnginePower(engine_index*4+0, force.z);
				_object->SetEnginePower(engine_index*4+1, force.x);
				_object->SetEnginePower(engine_index*4+3, force.y);
				break;
			default:
				_object->SetEnginePower(engine_index*4+0, force.x);
				_object->SetEnginePower(engine_index*4+1, force.y);
				_object->SetEnginePower(engine_index*4+3, force.z);
				break;
		}
	} else {
		// That's fine, wouldn't you say?
	}
}

void TrabantSimManager::AddTag(int object_id, const str& tag_type, const FloatList& floats, const StringList& strings, const IntList& phys, const IntList& engines, const IntList& meshes) {
	ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
	ScopeLock game_lock(GetTickLock());
	Object* _object = (Object*)GetContext()->GetObject(object_id);
	if (!_object) {
		return;
	}
	const tbc::ChunkyPhysics* physics = _object->GetPhysics();
	for (IntList::const_iterator x = phys.begin(); x != phys.end(); ++x) {
		if (*x >= physics->GetBoneCount()) {
			log_.Warningf("Object %i does not have a body with index %i.", object_id, *x);
			return;
		}
	}
	for (IntList::const_iterator x = engines.begin(); x != engines.end(); ++x) {
		if (*x >= physics->GetEngineCount()) {
			log_.Warningf("Object %i does not have an engine with index %i.", object_id, *x);
			return;
		}
	}
	for (IntList::const_iterator x = meshes.begin(); x != meshes.end(); ++x) {
		if (*x >= (int)((uitbc::ChunkyClass*)_object->GetClass())->GetMeshCount()) {
			log_.Warningf("Object %i does not have a mesh with index %i.", object_id, *x);
			return;
		}
	}
	tbc::ChunkyClass::Tag tag;
	tag.tag_name_ = tag_type;
	tag.float_value_list_ = floats;
	tag.string_value_list_ = strings;
	tag.body_index_list_ = phys;
	tag.engine_index_list_ = engines;
	tag.mesh_index_list_ = meshes;
	((tbc::ChunkyClass*)_object->GetClass())->AddTag(tag);
}



void TrabantSimManager::CommandLoop() {
	is_controlled_ = false;
	is_control_timeout_ = false;
	{
		ScopeLock phys_lock(GetMaster()->GetPhysicsLock());
		ScopeLock game_lock(GetTickLock());
		GetPhysicsManager()->InitCurrentThread();
	}
#ifdef LEPRA_TOUCH
	const double network_timeout = 5;
#else // Computer
	const double network_timeout = 2;
#endif // touch / computer
	uint8 __data[128*1024];
	while (!command_thread_->GetStopRequest()) {
		const double this_timeout = is_controlled_? 0.2 : network_timeout;
		if (!is_controlled_ || is_control_timeout_) {
			resend_last_response_.clear();
		}
		const int l = command_socket_->ReceiveFrom(__data, sizeof(__data), last_remote_address_, this_timeout);
		if (command_thread_->GetStopRequest()) {
			break;
		}
		if (hide_watch_.IsStarted()) {
			if (hide_watch_.QuerySplitTime() < 0.5) {
				continue;
			}
			hide_watch_.Stop();
		}
		if (l <= 0 || __data[l-1] != '\n' || ::memcmp("disconnect\n", __data, 11) == 0) {
			if (l == 0) {
				bool allow_remote_sync;
				v_get(allow_remote_sync, =, UiCure::GetSettings(), "Simulator.AllowRemoteSync", false);
				if (allow_remote_sync != (local_address_ == open_local_address_)) {
					if (open_local_address_ != internal_local_address_) {
						delete command_socket_;
						command_socket_ = 0;
						OpenConnection();
					}
					if (allow_remote_sync) {
						file_server_->Start();
					} else {
						file_server_->Stop();
					}
				}
				bool allow_timeout = (startup_timer_.QueryTimeDiff() >= 12);
				if (is_controlled_) {
					// Try a re-send if we haven't had a response in a long time.
					const double waited = resend_time_of_last_packet_.QueryTimeDiff();
					allow_timeout &= (waited >= network_timeout);
					if (!allow_timeout) {
						if (!resend_last_response_.empty() && waited > std::max(resend_intermediate_packet_time_*2, 0.5)) {
							if (command_socket_->SendTo((const uint8*)resend_last_response_.c_str(), (int)resend_last_response_.length(), last_remote_address_) != (int)resend_last_response_.length()) {
								is_controlled_ = false;
							}
							resend_last_response_.clear();
						}
					}
				}
				if (allow_timeout) {
					is_control_timeout_ = true;
				}
			} else {
				is_controlled_ = false;
			}
			command_socket_->ClearErrors();
			/*for (int x = 0; x < 10; ++x) {
				if (command_socket_->ReceiveFrom(__data, sizeof(__data), last_remote_address_, 0.01) == 0) {
					break;
				}
			}*/
			continue;
		}
#ifdef LEPRA_TOUCH
		if (last_remote_address_.GetIP() != internal_local_address_.GetIP() &&
		    last_remote_address_.GetIP() != last_accepted_address_.GetIP()) {
			str _hostname;
			if (!last_remote_address_.ResolveIpToHostname(_hostname)) {
				_hostname = last_remote_address_.GetIP().GetAsString();
			}
			if (ConnectQuery(_hostname)) {
				last_accepted_address_ = last_remote_address_;
			} else {
				command_socket_->SendTo((const uint8*)"disconnect\n", 11, last_remote_address_);
				continue;
			}
		}
#endif // touch device.

		if (!is_controlled_ || is_control_timeout_) {
			UnfoldSimulator();
			resend_time_of_last_packet_.PopTimeDiff();
			resend_intermediate_packet_time_ = 10;	// Just make it big to begin with.
		}
		if (is_paused_ && is_controlled_) {
			Thread::Sleep(0.5);
			resend_time_of_last_packet_.PopTimeDiff();
			const str pause_response = "\x16pause\x16";
			if (command_socket_->SendTo((const uint8*)pause_response.c_str(), (int)pause_response.length(), last_remote_address_) != (int)pause_response.length()) {
				is_controlled_ = false;
			}
			continue;
		}
		is_controlled_ = true;
		is_control_timeout_ = false;
		resend_intermediate_packet_time_ = Math::Lerp(resend_intermediate_packet_time_, resend_time_of_last_packet_.PopTimeDiff(), 0.05);
		if (command_thread_->GetStopRequest()) {
			break;
		}
		__data[l-1] = 0;	// Drop last linefeed.
		const str command = str((char*)__data);
		if (strutil::StartsWith(command, "get-platform-name")) {	// Check if user started over without terminating.
			resend_intermediate_packet_time_ = 10;	// Just make it big to begin with.
		}
		if (!GetConsoleManager()) {
			break;
		}
		GetConsoleManager()->ExecuteCommand(command);
		thread_get_str("cmd_response", cmd_response);
		resend_last_response_ = cmd_response;
		if (command_socket_->SendTo((const uint8*)resend_last_response_.c_str(), (int)resend_last_response_.length(), last_remote_address_) != (int)resend_last_response_.length()) {
			is_controlled_ = false;
		}
		//mLog.Infof("Responded to %s with %s (to %s."), lCommand.c_str(), lCmdResponse.c_str(), mLastRemoteAddress.GetAsString().c_str());
	}
	log_.Info("Terminating command thread.");
	is_controlled_ = false;
	deb_assert(command_thread_);
	deb_assert(command_socket_);
}

bool TrabantSimManager::IsControlled() {
	if (!command_thread_ || !command_thread_->IsRunning()) {
		return false;
	}

	bool allow_power_down;
	v_get(allow_power_down, =, GetVariableScope(), kRtvarGameAllowpowerdown, true);
	const bool is_power_down = (allow_power_down && is_control_timeout_);
	const bool is_controlled = (!is_power_down || is_controlled_);
	if (is_controlled != was_controlled_) {
		if (is_controlled) {
			v_set(GetVariableScope(), kRtvarGameUsermessage, " ");
			const int micro_steps = GetVariableScope()->GetDefaultValue(cure::RuntimeVariableScope::kReadDefault, kRtvarPhysicsMicrosteps, 3);
			v_set(GetVariableScope(), kRtvarPhysicsMicrosteps, micro_steps);
		} else {
#ifdef LEPRA_TOUCH
			if (is_control_timeout_) {
				if (last_remote_address_.GetIP() == internal_local_address_.GetIP()) {	// Otherwise it's just a push on the ESC key.
					log_.Warning("Prototype script is not responding, possibly hung?");
				}
				Thread::Sleep(0.1);	// Wait for it to reach stdout reader thread.
			}
			FoldSuspendSimulator();
#else // Computer
			if (is_paused_) {
				menu_->DismissDialog();
				OnMenuAlternative(0);
			}
			v_set(GetVariableScope(), kRtvarPhysicsMicrosteps, 1);
			if (ui_manager_->GetDisplayManager()->IsFocused()) {
				v_set(GetVariableScope(), kRtvarGameUsermessage, "Controller died?");
			} else if (hide_counter_ == 0) {
				hide_counter_ = 1;
			}
#endif // touch device.
		}
		was_controlled_ = is_controlled;
	}
	if (!is_controlled) {
		set_cursor_visible_ = true;
		is_mouse_controlled_ = false;
	}
	return is_controlled;
}



void TrabantSimManager::SaveSettings() {
	GetConsoleManager()->ExecuteCommand("save-application-config-file "+GetApplicationCommandFilename());
}

void TrabantSimManager::SetRenderArea(const PixelRect& render_area) {
	Parent::SetRenderArea(render_area);
	UpdateTouchstickPlacement();
}

bool TrabantSimManager::Open() {
	bool _ok = Parent::Open();
	if (_ok) {
#ifndef LEPRA_TOUCH
		pause_button_ = new uitbc::Button(GREEN_BUTTON, L"Pause");
		pause_button_->SetOnClick(TrabantSimManager, OnPauseButton);
		uitbc::Button* _button = pause_button_;
#else
		back_button_ = new uitbc::Button(GREEN_BUTTON, L"Back");
		back_button_->SetOnClick(TrabantSimManager, OnBackButton);
		uitbc::Button* _button = back_button_;
#endif
		int x = render_area_.left_ + 2;
		int y = render_area_.top_ + 2;
		ui_manager_->GetDesktopWindow()->AddChild(_button, x, y);
		double font_height;
		v_get(font_height, =, GetVariableScope(), kRtvarUi2DFontheight, 30.0);
		_button->SetPreferredSize((int)(font_height*7/3), (int)font_height);
		_button->SetRoundedRadius(4);
		_button->SetVisible(true);
	}
	if (_ok) {
		menu_ = new life::Menu(ui_manager_, GetResourceManager());
	}
	return _ok;
}

void TrabantSimManager::Close() {
	ScopeLock lock(GetTickLock());
	ContextObjectTable::iterator x = created_objects_.begin();
	for (; x != created_objects_.end(); ++x) {
		Object* object = (Object*)x->second;
		delete object;
	}
	delete pause_button_;
	pause_button_ = 0;
	delete back_button_;
	back_button_ = 0;
	delete menu_;
	menu_ = 0;
	if (light_) {
		delete light_;
		light_ = 0;
	}
	Parent::Close();
}

bool TrabantSimManager::OpenConnection() {
	if (command_socket_) {
		return true;
	}

	bool allow_remote_sync;
	v_get(allow_remote_sync, =, UiCure::GetSettings(), "Simulator.AllowRemoteSync", false);
	local_address_ = allow_remote_sync? open_local_address_ : internal_local_address_;
	command_socket_ = new UdpSocket(local_address_, true);
	if (command_socket_->IsOpen()) {
		if (!command_thread_) {
			command_thread_ = new MemberThread<TrabantSimManager>("CommandRecvThread");
			command_thread_->Start(this, &TrabantSimManager::CommandLoop);
		}
		log_.Headlinef("Command server listening on %s.", local_address_.GetAsString().c_str());
		return true;
	} else {
		log_.Headlinef("Could not open server on %s. Shutting down.", local_address_.GetAsString().c_str());
		SystemManager::AddQuitRequest(1);
		return false;
	}
}

void TrabantSimManager::CloseConnection() {
	if (command_socket_) {
		if (command_thread_) command_thread_->RequestStop();
		//command_socket_->Shutdown(SocketBase::kShutdownRecv);
		command_socket_->SendTo((uint8*)"disconnect\n", 11, last_remote_address_);
		//command_socket_->Shutdown(SocketBase::kShutdownBoth);
		UdpSocket(SocketAddress(), false).SendTo((uint8*)"?", 1, command_socket_->GetLocalAddress());
		delete command_thread_;
		command_thread_ = 0;
		//Thread::Sleep(0.1);	// Wait for shutdown.
		delete command_socket_;
		command_socket_ = 0;
	}
	file_server_->Stop();
}

void TrabantSimManager::SetIsQuitting() {
	((TrabantSimConsoleManager*)GetConsoleManager())->GetUiConsole()->SetVisible(false);
	Parent::SetIsQuitting();
}

void TrabantSimManager::SetFade(float fade_amount) {
	(void)fade_amount;
}



bool TrabantSimManager::Paint() {
	if (!Parent::Paint()) {
		return false;
	}

	float r, g, b;
	v_get(r, =1-(float), UiCure::GetSettings(), kRtvarUi3DClearred, 1.0);
	v_get(g, =1-(float), UiCure::GetSettings(), kRtvarUi3DCleargreen, 1.0);
	v_get(b, =1-(float), UiCure::GetSettings(), kRtvarUi3DClearblue, 1.0);
	Color _color = Color::CreateColor(r,g,b,1);
	ui_manager_->GetPainter()->SetColor(_color, 0);
	TouchstickList::iterator x = touchstick_list_.begin();
	for (; x != touchstick_list_.end(); ++x) {
		DrawStick(x->stick_, x->is_sloppy_);
	}
	return true;
}

void TrabantSimManager::DrawStick(Touchstick* stick, bool is_sloppy) {
	PixelRect area = stick->GetArea();
	const int ow = area.GetWidth();
	const int margin = stick->GetFingerRadius() / 8;
	const int r = stick->GetFingerRadius() - margin;
	area.Shrink(margin*2);
	ui_manager_->GetPainter()->DrawArc(area.left_, area.top_, area.GetWidth(), area.GetHeight(), 0, 360, false);
	float x;
	float y;
	bool is_pressing;
	stick->GetValue(x, y, is_pressing);
	if (is_pressing || is_sloppy) {
		vec2 v(x, y);
		v.Mul((ow+margin*2) / (float)ow);
		const float __length = v.GetLength();
		if (__length > 1) {
			v.Div(__length);
		}
		x = v.x;
		y = v.y;
		x = 0.5f*x + 0.5f;
		y = 0.5f*y + 0.5f;
		const int w = area.GetWidth()  - r*2;
		const int h = area.GetHeight() - r*2;
		ui_manager_->GetPainter()->DrawArc(
			area.left_ + (int)(w*x),
			area.top_  + (int)(h*y),
			r*2, r*2, 0, 360, true);
	}
}



bool TrabantSimManager::InitializeUniverse() {
	light_ = new Light(ui_manager_);
	return true;
}

void TrabantSimManager::TickInput() {
	TickNetworkInput();
	TickUiInput();
	if (set_focus_) {
		set_focus_ = false;
		ui_manager_->GetDisplayManager()->SetFocus(true);
	}
	if (set_cursor_visible_) {
		set_cursor_visible_ = false;
		ui_manager_->GetInputManager()->SetCursorVisible(true);
		if (pause_button_) {
			pause_button_->SetVisible(true);
		}
	}
	if (set_cursor_invisible_) {
		set_cursor_invisible_ = false;
		ui_manager_->GetInputManager()->SetCursorVisible(false);
		if (pause_button_) {
			pause_button_->SetVisible(false);
		}
	}
}

void TrabantSimManager::UpdateTouchstickPlacement() {
	if (touchstick_timer_.QueryTimeDiff() < 3.0) {
		return;
	}
	touchstick_timer_.ClearTimeDiff();

	const float touch_side_scale = 1.28f;	// Inches.
	float physical_scale = (float)ui_manager_->GetDisplayManager()->GetPhysicalScreenSize();
	physical_scale -= 2.5f;
	physical_scale = ::sqrt(physical_scale);
	physical_scale += 2.5f;
	const float touch_scale = touch_side_scale / physical_scale;
	const int side = (int)(render_area_.GetHeight() * touch_scale);
	TouchstickList::iterator x = touchstick_list_.begin();
	for (; x != touchstick_list_.end(); ++x) {
		PixelRect stick_area(render_area_);
		stick_area.top_ = stick_area.bottom_ - side;
		stick_area.right_ = stick_area.left_ + stick_area.GetHeight();
		const int ox = (int)(x->x * (render_area_.GetWidth()-side));
		const int oy = -(int)(x->y * (render_area_.GetHeight()-side));
		stick_area.Offset(ox,oy);
		x->stick_->Move(stick_area, x->orientation_);
	}
}

void TrabantSimManager::TickUiInput() {
}

void TrabantSimManager::TickUiUpdate() {
	if (hide_counter_ > 0) {
		hide_watch_.Start();
		is_controlled_ = false;
		Suspend(true);
		ui_manager_->GetDisplayManager()->HideWindow(true);
		Resume(true);
		v_set(GetVariableScope(), kRtvarPhysicsMicrosteps, 1);
		hide_counter_ = -1;
	} else if (hide_counter_ < 0 && is_controlled_) {
		ui_manager_->GetDisplayManager()->HideWindow(false);
		const int micro_steps = GetVariableScope()->GetDefaultValue(cure::RuntimeVariableScope::kReadDefault, kRtvarPhysicsMicrosteps, 3);
		v_set(GetVariableScope(), kRtvarPhysicsMicrosteps, micro_steps);
		hide_counter_ = 0;
	}

	((TrabantSimConsoleManager*)GetConsoleManager())->GetUiConsole()->Tick();
	collision_sound_manager_->Tick(camera_transform_.GetPosition());
}

void TrabantSimManager::SetLocalRender(bool render) {
	(void)render;
}



void TrabantSimManager::AddCheckerTexturing(uitbc::TriangleBasedGeometry* mesh, float scale) {
	const float _scale = 1/scale;
	const float off = _scale*0.5f;
	const float* vertices = mesh->GetVertexData();
	const vtx_idx_t* triangles = mesh->GetIndexData();
	const unsigned tc = mesh->GetTriangleCount();
	std::vector<float> u_vs;
	u_vs.resize(mesh->GetVertexCount()*2);
	for (unsigned t = 0; t < tc; ++t) {
		int v0,v1,v2;
		v0 = triangles[t*3+0];
		v1 = triangles[t*3+1];
		v2 = triangles[t*3+2];
		vec3 u0(vertices[v0*3+0], vertices[v0*3+1], vertices[v0*3+2]);
		vec3 u1(vertices[v1*3+0], vertices[v1*3+1], vertices[v1*3+2]);
		vec3 u2(vertices[v2*3+0], vertices[v2*3+1], vertices[v2*3+2]);
		vec3 n = (u1-u0).Cross(u2-u0);
		n.x = fabs(n.x); n.y = fabs(n.y); n.z = fabs(n.z);
		u0 *= _scale; u1 *= _scale; u2 *= _scale;
		if (n.x > n.y && n.x > n.z) {
			u_vs[v0*2+0] = u0.y+off;
			u_vs[v0*2+1] = u0.z+off;
			u_vs[v1*2+0] = u1.y+off;
			u_vs[v1*2+1] = u1.z+off;
			u_vs[v2*2+0] = u2.y+off;
			u_vs[v2*2+1] = u2.z+off;
		} else if (n.y > n.z) {
			u_vs[v0*2+0] = u0.x+off;
			u_vs[v0*2+1] = u0.z+off;
			u_vs[v1*2+0] = u1.x+off;
			u_vs[v1*2+1] = u1.z+off;
			u_vs[v2*2+0] = u2.x+off;
			u_vs[v2*2+1] = u2.z+off;
		} else {
			u_vs[v0*2+0] = u0.x+off;
			u_vs[v0*2+1] = u0.y+off;
			u_vs[v1*2+0] = u1.x+off;
			u_vs[v1*2+1] = u1.y+off;
			u_vs[v2*2+0] = u2.x+off;
			u_vs[v2*2+1] = u2.y+off;
		}
	}
	mesh->AddUVSet(u_vs.data());
}



cure::ContextObject* TrabantSimManager::CreateContextObject(const str& class_id) const {
	UiCure::Machine* _object = new Object(GetResourceManager(), class_id, ui_manager_);
	_object->SetAllowNetworkLogic(true);
	return _object;
}

void TrabantSimManager::OnLoadCompleted(cure::ContextObject* object, bool ok) {
	if (!ok) {
		log_.Errorf("Could not load object of type %s.", object->GetClassId().c_str());
		GetContext()->PostKillObject(object->GetInstanceId());
	}
}

void TrabantSimManager::OnTrigger(tbc::PhysicsManager::BodyID trigger, int trigger_listener_id, int other_object_id, tbc::PhysicsManager::BodyID body_id, const vec3& position, const vec3& normal) {
	Parent::OnTrigger(trigger, trigger_listener_id, other_object_id, body_id, position, normal);
	PushCollision(trigger_listener_id, vec3(), position, other_object_id);
}

void TrabantSimManager::OnCollision(const vec3& force, const vec3& torque, const vec3& position,
	cure::ContextObject* object1, cure::ContextObject* object2,
	tbc::PhysicsManager::BodyID body1_id, tbc::PhysicsManager::BodyID body2_id) {
	(void)body2_id;

	if (object1->GetPhysics()->GetPhysicsType() != tbc::ChunkyPhysics::kDynamic) {
		return;
	}

	collision_sound_manager_->OnCollision(force, torque, position, object1, object2, body1_id, 5000, false);

	PushCollision(object1->GetInstanceId(), force, position, object2->GetInstanceId());
}

void TrabantSimManager::PushCollision(cure::GameObjectId object_id1, const vec3& force, const vec3& position, cure::GameObjectId object_id2) {
	ScopeLock game_lock(GetTickLock());
	CollisionInfo ci;
	ci.object_id_ = object_id1;
	ci.force_ = force;
	ci.position_ = position;
	ci.other_object_id_ = object_id2;
	collision_list_.push_back(ci);
	if (collision_list_.size() > 300) {
		collision_list_.pop_front();
	}
}


void TrabantSimManager::OnPauseButton(uitbc::Button* button) {
	uitbc::Dialog* d = menu_->CreateTbcDialog(life::Menu::ButtonAction(this, &TrabantSimManager::OnMenuAlternative), 0.5f, 0.3f);
	if (!d) {
		return;
	}

	if (button) {
		menu_->OnTapSound(button);
	}
	pause_button_->SetVisible(false);
	is_paused_ = true;

	d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
	d->SetDirection(+1, false);
	uitbc::FixedLayouter layouter(d);

	uitbc::Label* label = new uitbc::Label(BRIGHT_TEXT, L"Paused");
	label->SetFontId(ui_manager_->SetScaleFont(1.2f));
	ui_manager_->SetMasterFont();
	label->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::TextComponent::kIconCenter);
	label->SetAdaptive(false);
	layouter.AddComponent(label, 0, 2, 0, 1, 1);

	uitbc::Button* unpause_button = new uitbc::Button(GREEN_BUTTON, L">");
	layouter.AddButton(unpause_button, -1000, 1, 2, 0, 1, 1, true);

	v_set(GetVariableScope(), kRtvarPhysicsHalt, true);
}

void TrabantSimManager::OnBackButton(uitbc::Button*) {
	FoldSuspendSimulator();
}

void TrabantSimManager::OnMenuAlternative(uitbc::Button*) {
	pause_button_->SetVisible(true);
	is_paused_ = false;
	HiResTimer::StepCounterShadow();
	v_set(GetVariableScope(), kRtvarPhysicsHalt, false);
}



void TrabantSimManager::ScriptPhysicsTick() {
	{
		ScopeLock object_lock(&objects_lock_);
		ContextObjectTable::const_iterator x = created_objects_.begin();
		for (; x != created_objects_.end(); ++x) {
			cure::GameObjectId object_id = x->first;
			Object* object = (Object*)x->second;
			AddContextObject(object, cure::kNetworkObjectLocalOnly, object_id);
			objects_.insert(object_id);
			static int physics_index = 0;
			object->physics_index_ = physics_index;
			object->CreatePhysics(object->generated_physics_);
		}
		created_objects_.clear();
		DeletedObjectSet::const_iterator y = deleted_objects_.begin();
		for (; y != deleted_objects_.end(); ++y) {
			int object_id = *y;
			GetContext()->PostKillObject(object_id);
			std::set<cure::GameObjectId>::iterator z = objects_.find(object_id);
			if (z != objects_.end()) {
				objects_.erase(z);
			}
		}
		deleted_objects_.clear();
	}

	// Camera moves in a "moving average" kinda curve (halfs the distance in x seconds).
	const float physics_time = GetTimeManager()->GetAffordedPhysicsTotalTime();
	if (physics_time > 1e-5) {
		float gx,gy,gz;
		v_get(gx, =(float), GetVariableScope(), kRtvarPhysicsGravityx, 0.0);
		v_get(gy, =(float), GetVariableScope(), kRtvarPhysicsGravityy, 0.0);
		v_get(gz, =(float), GetVariableScope(), kRtvarPhysicsGravityz, -9.8);
		GetPhysicsManager()->SetGravity(vec3(gx,gy,gz));
		uitbc::ParticleRenderer* particle_renderer = (uitbc::ParticleRenderer*)ui_manager_->GetRenderer()->GetDynamicRenderer("particle");
		particle_renderer->SetGravity(vec3(gx,gy,gz));
		MoveCamera(physics_time);
		light_->Tick(camera_transform_.orientation_);
		UpdateCameraPosition(true);

		UpdateUserMessage();
	}

	double sound_volume;
	v_get(sound_volume, =, GetVariableScope(), kRtvarUiSoundMastervolume, 1.0);
	if (sound_volume != last_sound_volume_) {
		master_volume_ = sound_volume;
	}
	const double _volume = IsControlled()? master_volume_ : 0.0;
	v_set(GetVariableScope(), kRtvarUiSoundMastervolume, _volume);
	ui_manager_->GetSoundManager()->SetMasterVolume((float)_volume);
	last_sound_volume_ = _volume;

	{
		ScopeLock game_lock(GetTickLock());
		const UiDragList drags = ui_manager_->GetDragManager()->GetDragList();
		for (UiDragList::const_iterator x = drags.begin(); x != drags.end(); ++x) {
			if (x->flags_&uilepra::touch::kDraggingOther) {
				// Ignore drag if pressing some kUi element, but don't ignore release if it originated outside of a stick.
				if (x->is_press_ || ((x->flags_&uilepra::touch::kDraggingStick) && uilepra::touch::TouchstickInputDevice::GetByCoordinate(ui_manager_->GetInputManager(), x->start_))) {
					continue;
				}
			}

			bool found = false;
			for (DragList::iterator y = drag_list_.begin(); y != drag_list_.end(); ++y) {
				if (y->start_.x == x->start_.x && y->start_.y == x->start_.y) {
					found = true;
					const float tf = 1 / (float)y->timer_.PopTimeDiff();
					if (tf > 1e-3) {
						y->velocity_ = Math::Lerp(y->velocity_, vec2((x->last_.x - y->last_.x)*tf, (x->last_.y - y->last_.y)*tf), 0.5f);
					}
					y->last_ = x->last_;
					y->is_press_ = x->is_press_;
				}
			}
			if (!found) {
				Drag d;
				d.start_ = x->start_;
				d.last_ = x->last_;
				d.is_press_ = x->is_press_;
				d.button_mask_ = x->button_mask_;
				drag_list_.push_back(d);
			}
			if (!x->is_press_) {
				drag_erase_list_.push_back(x->start_);
			}
		}
	}

	Parent::ScriptPhysicsTick();
	GetResourceManager()->ForceFreeCache();
}

void TrabantSimManager::MoveCamera(float frame_time) {
	int ctgt = 0;
	float clx,cly,clz,cdist,cax,cay,caz,crx,cry,crz,smooth;
	bool car;
	v_get(ctgt, =, GetVariableScope(), kRtvarUi3DCamtargetobject, 0);
	v_get(car, =, GetVariableScope(), kRtvarUi3DCamanglerelative, false);
	v_get(clx, =(float), GetVariableScope(), kRtvarUi3DCamlookatx, 0.0);
	v_get(cly, =(float), GetVariableScope(), kRtvarUi3DCamlookaty, 0.0);
	v_get(clz, =(float), GetVariableScope(), kRtvarUi3DCamlookatz, 0.0);
	v_get(cdist, =(float), GetVariableScope(), kRtvarUi3DCamdistance, 3.0);
	v_get(cax, =(float), GetVariableScope(), kRtvarUi3DCamanglex, 0.0);
	v_get(cay, =(float), GetVariableScope(), kRtvarUi3DCamangley, 0.0);
	v_get(caz, =(float), GetVariableScope(), kRtvarUi3DCamanglez, 0.0);
	v_get(crx, =(float), GetVariableScope(), kRtvarUi3DCamrotatex, 0.0);
	v_get(cry, =(float), GetVariableScope(), kRtvarUi3DCamrotatey, 0.0);
	v_get(crz, =(float), GetVariableScope(), kRtvarUi3DCamrotatez, 0.0);
	v_get(smooth, =(float), GetVariableScope(), kRtvarUi3DCamsmooth, 0.0);
	vec3 look_at(clx,cly,clz);
	Object* _object = 0;
	if (ctgt) {
		_object = (Object*)GetContext()->GetObject(ctgt);
		if (_object) {
			look_at = _object->GetPosition() + _object->GetOrientation()*look_at;
			camera_velocity_ = _object->GetVelocity();
		}
	}
	xform t(quat(), look_at+vec3(0,-cdist,0));
	t.RotateAroundAnchor(look_at, vec3(0,1,0), cay);
	t.RotateAroundAnchor(look_at, vec3(1,0,0), cax);
	t.RotateAroundAnchor(look_at, vec3(0,0,1), caz);
	if (_object && car) {
		xform pt;
		GetPhysicsManager()->GetBodyTransform(_object->GetPhysics()->GetBoneGeometry(0)->GetBodyId(), pt);
		quat q = pt.orientation_ * _object->initial_inverse_orientation_;
		t.orientation_ = q * t.orientation_;
		t.position_ = q*(t.position_-look_at) + look_at;
	}
	t.RotateAroundAnchor(look_at, vec3(0,1,0), camera_angle_.y);
	t.RotateAroundAnchor(look_at, vec3(1,0,0), camera_angle_.x);
	t.RotateAroundAnchor(look_at, vec3(0,0,1), camera_angle_.z);
	camera_angle_.x = fmod(camera_angle_.x+crx*frame_time*2*PIF,2*PIF);
	camera_angle_.y = fmod(camera_angle_.y+cry*frame_time*2*PIF,2*PIF);
	camera_angle_.z = fmod(camera_angle_.z+crz*frame_time*2*PIF,2*PIF);
	camera_transform_.position_ = Math::Lerp(t.position_, camera_transform_.position_, smooth);
	camera_transform_.orientation_.Slerp(t.orientation_, camera_transform_.orientation_, smooth);
}

void TrabantSimManager::UpdateCameraPosition(bool update_mic_position) {
	ui_manager_->SetCameraPosition(camera_transform_);
	if (update_mic_position) {
		ui_manager_->SetMicrophonePosition(camera_transform_, camera_velocity_);
	}
}

void TrabantSimManager::UpdateUserMessage() {
	str user_message;
	v_get(user_message, =, GetVariableScope(), kRtvarGameUsermessage, " ");
	if (strutil::Strip(user_message, " \t\r\n").empty()) {
		if (user_info_dialog_) {
			menu_->DismissDialog();
			user_info_dialog_ = 0;
			user_info_label_ = 0;
		}
	} else {
		wstr wide_user_message = wstrutil::Encode(user_message);
		if (user_info_dialog_) {
			if (user_info_label_->GetText() != wide_user_message) {
				user_info_label_->SetText(wide_user_message);
			}
		} else {
			uitbc::Dialog* d = menu_->CreateTbcDialog(life::Menu::ButtonAction(this, &TrabantSimManager::OnMenuAlternative), 0.8f, 0.5f);
			if (!d) {
				return;
			}
			d->SetColor(BG_COLOR, OFF_BLACK, BLACK, BLACK);
			d->SetDirection(+1, false);
			uitbc::FixedLayouter layouter(d);

			uitbc::Label* label = new uitbc::Label(BRIGHT_TEXT, wide_user_message);
			label->SetIcon(uitbc::Painter::kInvalidImageid, uitbc::TextComponent::kIconCenter);
			label->SetAdaptive(false);
			layouter.AddComponent(label, 0, 1, 0, 1, 1);
			user_info_dialog_ = d;
			user_info_label_ = label;
		}
	}
}



void TrabantSimManager::PrintText(const wstr& s, int x, int y) const {
	Color old_color = ui_manager_->GetPainter()->GetColor(0);
	ui_manager_->GetPainter()->SetColor(DARK_BLUE, 0);
	ui_manager_->GetPainter()->PrintText(s, x, y+1);
	ui_manager_->GetPainter()->SetColor(old_color, 0);
	ui_manager_->GetPainter()->PrintText(s, x, y);
}

void TrabantSimManager::DrawImage(uitbc::Painter::ImageID image_id, float cx, float cy, float w, float h, float angle) const {
	cx -= 0.5f;

	const float ca = ::cos(angle);
	const float sa = ::sin(angle);
	const float w2 = w*0.5f;
	const float h2 = h*0.5f;
	const float x = cx - w2*ca - h2*sa;
	const float y = cy - h2*ca + w2*sa;
	const vec2 c[] = { vec2(x, y), vec2(x+w*ca, y-w*sa), vec2(x+w*ca+h*sa, y+h*ca-w*sa), vec2(x+h*sa, y+h*ca) };
	const vec2 t[] = { vec2(0, 0), vec2(1, 0), vec2(1, 1), vec2(0, 1) };
#define V(z) std::vector<vec2>(z, z+LEPRA_ARRAY_COUNT(z))
	ui_manager_->GetPainter()->DrawImageFan(image_id, V(c), V(t));
}



bool TrabantSimManager::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	if (!Parent::OnKeyDown(key_code)) {
		ScopeLock game_lock(GetTickLock());
		key_map_.insert(KeyMap::value_type(key_code,false));
		return false;
	}
	return true;
}

bool TrabantSimManager::OnKeyUp(uilepra::InputManager::KeyCode key_code) {
	if (key_code == uilepra::InputManager::kInKbdEsc && is_mouse_controlled_) {
		if (pause_button_) {
			if (pause_button_->IsVisible()) {
				set_cursor_invisible_ = true;
			} else {
				set_cursor_visible_ = true;
			}
		}
	}
	if (key_code == uilepra::InputManager::kInKbdF5) {
		if (is_controlled_) {
			command_socket_->SendTo((const unsigned char*)"disconnect\n", 11, last_remote_address_);
			is_controlled_ = false;
		}
		hide_counter_ = 1;
	}

	{
		ScopeLock game_lock(GetTickLock());
		key_map_[key_code] = true;
	}
	return Parent::OnKeyUp(key_code);
}

void TrabantSimManager::OnInput(uilepra::InputElement* element) {
	if (element->GetInterpretation() == uilepra::InputElement::kRelativeAxis) {
		uilepra::InputDevice* mouse = ui_manager_->GetInputManager()->GetMouse();
		if (element == mouse->GetAxis(0)) {
			mouse_move_.x += element->GetValue();
		} else if (element == mouse->GetAxis(1)) {
			mouse_move_.y += element->GetValue();
		} else {
			mouse_move_.z += element->GetValue();
		}
	}
	return Parent::OnInput(element);
}

void TrabantSimManager::PainterImageLoadCallback(UiCure::UserPainterKeepImageResource* resource) {
	if (resource->GetLoadState() == cure::kResourceLoadComplete) {
		ui_manager_->GetDesktopWindow()->GetImageManager()->AddLoadedImage(*resource->GetRamData(), resource->GetData(),
			uitbc::GUIImageManager::kCentered, uitbc::GUIImageManager::kAlphablend, 255);
	}
}



loginstance(kGame, TrabantSimManager);



}
