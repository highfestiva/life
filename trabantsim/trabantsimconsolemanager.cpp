
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "trabantsimconsolemanager.h"
#include "../cure/include/contextmanager.h"
#include "../lepra/include/cyclicarray.h"
#include "../lepra/include/path.h"
#include "../lepra/include/systemmanager.h"
#include "trabantsimmanager.h"
#include "rtvar.h"



namespace TrabantSim {



// Must lie before TrabantSimConsoleManager to compile.
const TrabantSimConsoleManager::CommandPair TrabantSimConsoleManager::command_id_list_[] =
{
	{"reset", kCommandReset},
	{"get-platform-name", kCommandGetPlatformName},
	{"create-object", kCommandCreateObject},
	{"create-clones", kCommandCreateClones},
	{"delete-object", kCommandDeleteObject},
	{"delete-all-objects", kCommandDeleteAllObjects},
	{"pick-objects", kCommandPickObjects},
	{"clear-phys", kCommandClearPhys},
	{"prep-phys-box", kCommandPrepPhysBox},
	{"prep-phys-sphere", kCommandPrepPhysSphere},
	{"prep-phys-capsule", kCommandPrepPhysCapsule},
	{"prep-phys-mesh", kCommandPrepPhysMesh},
	{"prep-gfx-mesh", kCommandPrepGfxMesh},
	{"set-vertices", kCommandSetVertices},
	{"add-vertices", kCommandAddVertices},
	{"set-indices", kCommandSetIndices},
	{"add-indices", kCommandAddIndices},
	{"are-loaded", kCommandAreLoaded},
	{"wait-until-loaded", kCommandWaitUntilLoaded},
	{"explode", kCommandExplode},
	{"play-sound", kCommandPlaySound},
	{"pop-collisions", kCommandPopCollisions},
	{"get-keys", kCommandGetKeys},
	{"get-touch-drags", kCommandGetTouchDrags},
	{"get-accelerometer", kCommandGetAccelerometer},
	{"get-mousemove", kCommandGetMousemove},
	{"create-joystick", kCommandCreateJoystick},
	{"get-joystick-data", kCommandGetJoystickData},
	{"get-aspect-ratio", kCommandGetAspectRatio},
	{"create-engine", kCommandCreateEngine},
	{"create-joint", kCommandCreateJoint},
	{"position", kCommandPosition},
	{"orientation", kCommandOrientation},
	{"velocity", kCommandVelocity},
	{"angular-velocity", kCommandAngularVelocity},
	{"force", kCommandForce},
	{"torque", kCommandTorque},
	{"mass", kCommandMass},
	{"color", kCommandColor},
	{"engine-force", kCommandEngineForce},
	{"set-tag-floats", kCommandSetTagFloats},
	{"set-tag-strings", kCommandSetTagStrings},
	{"set-tag-phys", kCommandSetTagPhys},
	{"set-tag-engine", kCommandSetTagEngine},
	{"set-tag-mesh", kCommandSetTagMesh},
	{"add-tag", kCommandAddTag},
};



struct ParameterException {
};

std::vector<float> Strs2Flts(const strutil::strvec& strs, size_t index=0) {
	std::vector<float> flts;
	strutil::strvec::const_iterator x = strs.begin() + index;
	for(; x != strs.end(); ++x) {
		double d = 0;
		strutil::StringToDouble(*x, d);
		flts.push_back((float)d);
	}
	return flts;
}

int StrToUInt(const char* s, const char** _end) {
	int i = 0;
	for(; *s >= '0' && *s <= '9'; ++s) {
		i = i*10 + (*s-'0');
	}
	*_end = s;
	return i;
}

float StrToFloat(const char* s, const char** _end) {
	float f = 0;
	float ff = 0.1f;
	float e = 0;
	bool before_dot = true;
	bool negative = false;
	bool exponent = false;
	bool negative_exponent = false;
	for (;; ++s) {
		if (*s >= '0' && *s <= '9') {
			if (exponent) {
				e = e*10 + (*s-'0');
			} else if (before_dot) {
				f = f*10 + (*s-'0');
			} else {
				f += (*s-'0')*ff;
				ff *= 0.1f;
			}
		} else if (*s == '.') {
			before_dot = false;
		} else if (*s == '-') {
			if (exponent) {
				negative_exponent = true;
			} else {
				negative = true;
			}
		} else if (*s == 'e') {
			exponent = true;
		} else {
			break;
		}
	}
	*_end = s;
	f = negative? -f : f;
	if (!exponent) {
		return f;
	}
	e = negative_exponent? -e : e;
	return f*(float)pow(10,e);
}

str ParamToStr(const strutil::strvec& param, size_t index) {
	if (index >= param.size()) {
		throw ParameterException();
	}
	return param[index];
}

bool ParamToBool(const strutil::strvec& param, size_t index) {
	if (index >= param.size()) {
		throw ParameterException();
	}
	return param[index] == "true";
}

int ParamToInt(const strutil::strvec& param, size_t index, bool* is_set = 0) {
	int value = 0;
	if (index >= param.size() || !strutil::StringToInt(param[index], value)) {
		if (!is_set) {
			throw ParameterException();
		}
		*is_set = false;
	} else if (is_set) {
		*is_set = true;
	}
	return value;
}

float ParamToFloat(const strutil::strvec& param, size_t index, bool* is_set = 0) {
	double value = 0;
	if (index >= param.size() || !strutil::StringToDouble(param[index], value)) {
		if (!is_set) {
			throw ParameterException();
		}
		*is_set = false;
	} else if (is_set) {
		*is_set = true;
	}
	return (float)value;
}

vec2 ParamToVec2(const strutil::strvec& param, size_t index) {
	return vec2(ParamToFloat(param,index,0), ParamToFloat(param,index+1,0));
}

vec3 ParamToVec3(const strutil::strvec& param, size_t index, bool* is_set = 0) {
	return vec3(ParamToFloat(param,index,is_set), ParamToFloat(param,index+1,is_set), ParamToFloat(param,index+2,is_set));
}

quat ParamToQuat(const strutil::strvec& param, size_t index, bool* is_set = 0) {
	return quat(ParamToFloat(param,index,is_set), ParamToFloat(param,index+1,is_set), ParamToFloat(param,index+2,is_set), ParamToFloat(param,index+3,is_set));
}

void Params2Ints(const strutil::strvec& param, std::vector<int>& ints) {
	strutil::strvec::const_iterator p;
	for (p = param.begin(); p != param.end(); ++p) {
		const char* s = p->c_str();
		const char* __end;
		do {
			int i = StrToUInt(s, &__end);
			ints.push_back(i);
			s = __end+1;
		} while (*__end == ',');
	}
}

void Params2Floats(const strutil::strvec& param, std::vector<float>& floats, size_t param_index=0) {
	strutil::strvec::const_iterator p;
	for (p = param.begin()+param_index; p != param.end(); ++p) {
		const char* s = p->c_str();
		const char* __end;
		do {
			float f = StrToFloat(s, &__end);
			floats.push_back(f);
			s = __end+1;
		} while (*__end == ',');
	}
}

strstream& operator<<(strstream& os, const strutil::strvec& vec) {
	int j = 0;
	for (strutil::strvec::const_iterator i = vec.begin(); i != vec.end(); ++i, ++j) {
		if (j) {
			os << ' ';
		}
		os << *i;
	}
	return os;
}

strstream& operator<<(strstream& os, char c) {
	(*(std::ostream*)&os) << c;
	return os;
}

strstream& operator<<(strstream& os, const char* s) {
	(*(std::ostream*)&os) << s;
	return os;
}

strstream& operator<<(strstream& os, const str& s) {
	(*(std::ostream*)&os) << s;
	return os;
}

strstream& operator<<(strstream& os, int i) {
	return os << strutil::IntToString(i, 10);
}

strstream& operator<<(strstream& os, float f) {
	return os << strutil::FastDoubleToString(f);
}

strstream& operator<<(strstream& os, const vec3& vec) {
	return os << vec.x << ' ' << vec.y << ' ' << vec.z;
}

strstream& operator<<(strstream& os, const quat& _quat) {
	return os << _quat.a << ' ' << _quat.b << ' ' << _quat.c << ' ' << _quat.d;
}



TrabantSimConsoleManager::TrabantSimConsoleManager(cure::ResourceManager* resource_manager, cure::GameManager* game_manager,
	UiCure::GameUiManager* ui_manager, cure::RuntimeVariableScope* variable_scope, const PixelRect& area):
	Parent(resource_manager, game_manager, ui_manager, variable_scope, area) {
	InitCommands();
	SetSecurityLevel(1);
}

TrabantSimConsoleManager::~TrabantSimConsoleManager() {
}

bool TrabantSimConsoleManager::Start() {
#ifndef LEPRA_TOUCH
	return Parent::Start();
#else // touch
	return true;	// touch device don't need an interactive console.
#endif // Computer / touch
}

const str TrabantSimConsoleManager::GetActiveResponse() const {
	return active_response_.str();
}



unsigned TrabantSimConsoleManager::GetCommandCount() const {
	return Parent::GetCommandCount() + LEPRA_ARRAY_COUNT(command_id_list_);
}

const TrabantSimConsoleManager::CommandPair& TrabantSimConsoleManager::GetCommand(unsigned index) const {
	if (index < Parent::GetCommandCount()) {
		return (Parent::GetCommand(index));
	}
	return (command_id_list_[index-Parent::GetCommandCount()]);
}

int TrabantSimConsoleManager::OnCommand(const HashedString& command, const strutil::strvec& parameter_vector) {
	active_response_.str(str());
	active_response_ << "ok\n";
	int result = Parent::OnCommand(command, parameter_vector);
	if (result < 0) {
		result = 0;
		TrabantSimManager* manager = ((TrabantSimManager*)GetGameManager());

		CommandClient _command = (CommandClient)TranslateCommand(command);
		try {
			switch ((int)_command) {
				case kCommandReset: {
					manager->UserReset();
				} break;
				case kCommandGetPlatformName: {
#if defined(LEPRA_IOS)
					active_response_ << "iOS";
#elif defined(LEPRA_MAC)
					active_response_ << "Mac";
#elif defined(LEPRA_WINDOWS)
					active_response_ << "Win";
#else
					active_response_ << "Unknown";
#endif // Platform

				} break;
				case kCommandCreateObject: {
					const str type = ParamToStr(parameter_vector, 0);
					const str material_info = ParamToStr(parameter_vector, 1);
					const vec3 position = ParamToVec3(parameter_vector, 2);
					const quat orientation = ParamToQuat(parameter_vector, 5);
					// Work out material.
					const bool is_static = strutil::StartsWith(type, "static");
					const bool is_trigger = (type.find("trigger") != str::npos);
					ObjectMaterial material = MaterialSmooth;
					if (material_info == "flat") material = MaterialFlat;
					else if (material_info == "checker") material = MaterialChecker;
					else if (material_info == "noise") material = MaterialNoise;

					const int object_id = manager->CreateObject(orientation, position, gfx_mesh_, phys_objects_, material, is_static, is_trigger);
					if (object_id == -1) {
						throw ParameterException();
					}
					active_response_ << object_id;
				} break;
				case kCommandCreateClones: {
					const int original_id = ParamToInt(parameter_vector, 0);
					const str type = ParamToStr(parameter_vector, 1);
					const str material_info = ParamToStr(parameter_vector, 2);
					std::vector<float> placements;
					Params2Floats(parameter_vector, placements, 3);
					// Work out material.
					const bool is_static = (type == "static");
					ObjectMaterial material = MaterialSmooth;
					if (material_info == "flat") material = MaterialFlat;
					else if (material_info == "checker") material = MaterialChecker;
					else if (material_info == "noise") material = MaterialNoise;

					std::vector<xform> transforms;
					int x = 0, cnt = (int)placements.size();
					for (; x <= cnt-7; x += 7) {
						transforms.push_back(xform(quat(&placements[x+3]), vec3(&placements[x])));
					}
					std::vector<int> object_ids;
					manager->CreateClones(object_ids, original_id, transforms, material, is_static);
					if (object_ids.empty()) {
						throw ParameterException();
					}
					x = 0;
					for (std::vector<int>::iterator y = object_ids.begin(); y != object_ids.end(); ++y, ++x) {
						if (x) {
							active_response_ << ',';
						}
						active_response_ << *y;
					}
				} break;
				case kCommandDeleteObject: {
					manager->DeleteObject(ParamToInt(parameter_vector, 0));
				} break;
				case kCommandDeleteAllObjects: {
					manager->DeleteAllObjects();
				} break;
				case kCommandPickObjects: {
					const vec3 position = ParamToVec3(parameter_vector, 0);
					const vec3 direction = ParamToVec3(parameter_vector, 3);
					const vec2 range = ParamToVec2(parameter_vector, 6);
					std::vector<int> object_ids;
					std::vector<vec3> positions;
					manager->PickObjects(position, direction, range, object_ids, positions);
					size_t c = object_ids.size();
					for (size_t x = 0; x < c; ++x) {
						if (x) {
							active_response_ << ',';
						}
						const vec3& v = positions[x];
						active_response_ << object_ids[x] << ',' << v.x << ',' << v.y << ',' << v.z;
					}
				} break;
				case kCommandClearPhys: {
					PhysObjectArray::iterator x;
					for (x = phys_objects_.begin(); x != phys_objects_.end(); ++x) {
						delete *x;
					}
					phys_objects_.clear();
				} break;
				case kCommandPrepPhysBox: {
					if (parameter_vector.size() != 10) {
						log_.Warningf("usage: %s followed by ten float arguments (quaternion, position, size)", command.c_str());
						return 1;
					}
					std::vector<float> _floats = Strs2Flts(parameter_vector);
					PlacedObject* box = new BoxObject(quat(&_floats[0]), vec3(&_floats[4]), vec3(&_floats[7]));
					phys_objects_.push_back(box);
				} break;
				case kCommandPrepPhysSphere: {
					if (parameter_vector.size() != 8) {
						log_.Warningf("usage: %s followed by eight float arguments (quaternion, position, radius)", command.c_str());
						return 1;
					}
					std::vector<float> _floats = Strs2Flts(parameter_vector);
					PlacedObject* sphere = new SphereObject(quat(&_floats[0]), vec3(&_floats[4]), _floats[7]);
					phys_objects_.push_back(sphere);
				} break;
				case kCommandPrepPhysCapsule: {
					if (parameter_vector.size() != 9) {
						log_.Warningf("usage: %s followed by eight float arguments (quaternion, position, radius, length)", command.c_str());
						return 1;
					}
					std::vector<float> _floats = Strs2Flts(parameter_vector);
					PlacedObject* capsule = new CapsuleObject(quat(&_floats[0]), vec3(&_floats[4]), _floats[7], _floats[8]);
					phys_objects_.push_back(capsule);
				} break;
				case kCommandPrepPhysMesh: {
					if (parameter_vector.size() != 7) {
						log_.Warningf("usage: %s followed by seven float arguments (quaternion and position)", command.c_str());
						return 1;
					}
					if (vertices_.size() % 3 || indices_.size() % 3) {
						throw ParameterException();
					}
					std::vector<float> _floats = Strs2Flts(parameter_vector);
					MeshObject* mesh = new MeshObject(quat(&_floats[0]), vec3(&_floats[4]));
					mesh->vertices_.insert(mesh->vertices_.end(), vertices_.begin(), vertices_.end());
					mesh->indices_.insert(mesh->indices_.end(), indices_.begin(), indices_.end());
					phys_objects_.push_back(mesh);
				} break;
				case kCommandPrepGfxMesh: {
					if (parameter_vector.size() != 7) {
						log_.Warningf("usage: %s followed by seven float arguments (quaternion and position)", command.c_str());
						return 1;
					}
					if (vertices_.size() % 3 || indices_.size() % 3) {
						throw ParameterException();
					}
					std::vector<float> _floats = Strs2Flts(parameter_vector);
					gfx_mesh_.orientation_.Set(&_floats[0]);
					gfx_mesh_.pos_.Set(&_floats[4]);
					gfx_mesh_.vertices_.clear();
					gfx_mesh_.indices_.clear();
					gfx_mesh_.vertices_.insert(gfx_mesh_.vertices_.end(), vertices_.begin(), vertices_.end());
					gfx_mesh_.indices_.insert(gfx_mesh_.indices_.end(), indices_.begin(), indices_.end());
				} break;
				case kCommandSetVertices:
					vertices_.clear();
					// TRICKY: fall through!
				case kCommandAddVertices: {
					Params2Floats(parameter_vector, vertices_);
				} break;
				case kCommandSetIndices:
					indices_.clear();
					// TRICKY: fall through!
				case kCommandAddIndices: {
					Params2Ints(parameter_vector, indices_);
				} break;
				case kCommandAreLoaded: {
					std::vector<int> object_ids;
					Params2Ints(parameter_vector, object_ids);
					bool first = true;
					for (std::vector<int>::iterator y = object_ids.begin(); y != object_ids.end(); ++y) {
						if (!first) {
							active_response_ << ',';
						}
						first = false;
						active_response_ << (manager->IsLoaded(*y)? '1' : '0');
					}
				} break;
				case kCommandWaitUntilLoaded: {
					int object_id = ParamToInt(parameter_vector, 0);
					bool loaded = false;
					for (int x = 0; !loaded && x < 150; ++x) {
						loaded = manager->IsLoaded(object_id);
						if (!loaded) {
							Thread::Sleep(0.01);
						}
					}
					if (!loaded) {
						log_.Warningf("Object %i did not load in time. Try calling again.", object_id);
					}
				} break;
				case kCommandExplode: {
					manager->Explode(ParamToVec3(parameter_vector, 0), ParamToVec3(parameter_vector, 3), ParamToFloat(parameter_vector, 6), ParamToFloat(parameter_vector, 7));
				} break;
				case kCommandPlaySound: {
					manager->PlaySound(ParamToStr(parameter_vector, 0), ParamToVec3(parameter_vector, 1), ParamToVec3(parameter_vector, 4), ParamToFloat(parameter_vector, 7));
				} break;
				case kCommandPopCollisions: {
					typedef TrabantSimManager::CollisionList CollisionList;
					CollisionList list;
					manager->PopCollisions(list);
					int y = 0;
					for (CollisionList::iterator x = list.begin(); x != list.end() && y < 10; ++x, ++y) {
						if (y) {
							active_response_ << '\n';
						}
						active_response_ << x->object_id_ << ' ' << x->force_ << ' ' << x->position_ << ' ' << x->other_object_id_;
					}
				} break;
				case kCommandGetKeys: {
					strutil::strvec keys;
					manager->GetKeys(keys);
					active_response_ << strutil::Join(keys, "\n");
				} break;
				case kCommandGetTouchDrags: {
					PixelRect rect = manager->GetRenderArea();
					const float sx = 1.0f/rect.GetWidth();
					const float sy = 1.0f/rect.GetHeight();
					typedef TrabantSimManager::DragList DragList;
					DragList list;
					manager->GetTouchDrags(list);
					int y = 0;
					for (DragList::iterator x = list.begin(); x != list.end(); ++x, ++y) {
						if (y) {
							active_response_ << '\n';
						}
						active_response_ << x->last_.x*sx << ' ' << x->last_.y*sy << ' ' << x->start_.x*sx << ' ' << x->start_.y*sy << ' '
								<< x->velocity_.x*sx << ' ' << x->velocity_.y*sy << ' ' << (x->is_press_?"true":"false") << ' ' << x->button_mask_;
					}
				} break;
				case kCommandGetAccelerometer: {
					const vec3 a = manager->GetAccelerometer();
					active_response_ << a;
				} break;
				case kCommandGetMousemove: {
					const vec3 m = manager->GetMouseMove();
					active_response_ << m;
				} break;
				case kCommandCreateJoystick: {
					const int joy_id = manager->CreateJoystick(ParamToFloat(parameter_vector, 0), ParamToFloat(parameter_vector, 1), ParamToBool(parameter_vector, 2));
					active_response_ << joy_id;
				} break;
				case kCommandGetJoystickData: {
					typedef TrabantSimManager::JoystickDataList JoyList;
					JoyList list = manager->GetJoystickData();
					int y = 0;
					for (JoyList::iterator x = list.begin(); x != list.end(); ++x, ++y) {
						if (y) {
							active_response_ << '\n';
						}
						active_response_ << x->joystick_id_ << ' ' << x->x << ' ' << x->y;
					}
				} break;
				case kCommandGetAspectRatio: {
					active_response_ << manager->GetAspectRatio();
				} break;
				case kCommandCreateEngine: {
					const int object_id = ParamToInt(parameter_vector, 0);
					const str engine_type = ParamToStr(parameter_vector, 1);
					const vec2 max_velocity = ParamToVec2(parameter_vector, 2);
					const float strength = ParamToFloat(parameter_vector, 4);
					const float friction = ParamToFloat(parameter_vector, 5);
					TrabantSimManager::EngineTargetList targets;
					size_t count = parameter_vector.size();
					for (size_t x = 6; x+1 < count; x += 2) {
						targets.push_back(TrabantSimManager::EngineTarget(ParamToInt(parameter_vector, x), ParamToFloat(parameter_vector, x+1)));
					}
					const int engine_id = manager->CreateEngine(object_id, engine_type, max_velocity, strength, friction, targets);
					if (engine_id < 0) {
						throw ParameterException();
					}
					active_response_ << engine_id;
				} break;
				case kCommandCreateJoint: {
					const int object_id = ParamToInt(parameter_vector, 0);
					const str joint_type = ParamToStr(parameter_vector, 1);
					const int other_object_id = ParamToInt(parameter_vector, 2);
					const vec3 axis = ParamToVec3(parameter_vector, 3);
					const vec2 stop = ParamToVec2(parameter_vector, 6);
					const vec2 spring = ParamToVec2(parameter_vector, 8);
					const int joint_id = manager->CreateJoint(object_id, joint_type, other_object_id, axis, stop, spring);
					if (joint_id < 0) {
						throw ParameterException();
					}
					active_response_ << joint_id;
				} break;
				case kCommandPosition: {
					bool _is_set;
					vec3 value = ParamToVec3(parameter_vector, 1, &_is_set);
					manager->Position(ParamToInt(parameter_vector, 0), _is_set, value);
					if (!_is_set) {
						active_response_ << value;
					}
				} break;
				case kCommandOrientation: {
					bool _is_set;
					quat value = ParamToQuat(parameter_vector, 1, &_is_set);
					manager->Orientation(ParamToInt(parameter_vector, 0), _is_set, value);
					if (!_is_set) {
						active_response_ << value;
					}
				} break;
				case kCommandVelocity: {
					bool _is_set;
					vec3 value = ParamToVec3(parameter_vector, 1, &_is_set);
					manager->Velocity(ParamToInt(parameter_vector, 0), _is_set, value);
					if (!_is_set) {
						active_response_ << value;
					}
				} break;
				case kCommandAngularVelocity: {
					bool _is_set;
					vec3 value = ParamToVec3(parameter_vector, 1, &_is_set);
					manager->AngularVelocity(ParamToInt(parameter_vector, 0), _is_set, value);
					if (!_is_set) {
						active_response_ << value;
					}
				} break;
				case kCommandForce: {
					bool _is_set;
					vec3 value = ParamToVec3(parameter_vector, 1, &_is_set);
					manager->Force(ParamToInt(parameter_vector, 0), _is_set, value);
					if (!_is_set) {
						active_response_ << value;
					}
				} break;
				case kCommandTorque: {
					bool _is_set;
					vec3 value = ParamToVec3(parameter_vector, 1, &_is_set);
					manager->Torque(ParamToInt(parameter_vector, 0), _is_set, value);
					if (!_is_set) {
						active_response_ << value;
					}
				} break;
				case kCommandMass: {
					bool _is_set;
					float value = ParamToFloat(parameter_vector, 1, &_is_set);
					manager->Mass(ParamToInt(parameter_vector, 0), _is_set, value);
					if (!_is_set) {
						active_response_ << value;
					}
				} break;
				case kCommandColor: {
					bool _is_set;
					vec3 value = ParamToVec3(parameter_vector, 1, &_is_set);
					float alpha = ParamToFloat(parameter_vector, 4, &_is_set);
					manager->ObjectColor(ParamToInt(parameter_vector, 0), _is_set, value, alpha);
					if (!_is_set) {
						active_response_ << value;
					}
				} break;
				case kCommandEngineForce: {
					bool _is_set;
					vec3 value = ParamToVec3(parameter_vector, 2, &_is_set);
					manager->EngineForce(ParamToInt(parameter_vector, 0), ParamToInt(parameter_vector, 1), _is_set, value);
					if (!_is_set) {
						active_response_ << value;
					}
				} break;
				case kCommandSetTagFloats: {
					tag_floats_.clear();
					Params2Floats(parameter_vector, tag_floats_);
				} break;
				case kCommandSetTagStrings: {
					tag_strings_ = parameter_vector;
				} break;
				case kCommandSetTagPhys: {
					tag_phys_.clear();
					Params2Ints(parameter_vector, tag_phys_);
				} break;
				case kCommandSetTagEngine: {
					tag_engines_.clear();
					Params2Ints(parameter_vector, tag_engines_);
				} break;
				case kCommandSetTagMesh: {
					tag_meshes_.clear();
					Params2Ints(parameter_vector, tag_meshes_);
				} break;
				case kCommandAddTag: {
					const int object_id = ParamToInt(parameter_vector, 0);
					const str tag_type = ParamToStr(parameter_vector, 1);
					manager->AddTag(object_id, tag_type, tag_floats_, tag_strings_, tag_phys_, tag_engines_, tag_meshes_);
				} break;
				default: {
					result = -1;
				} break;
			}
		} catch (ParameterException) {
			result = 1;
			active_response_.str(str());
			active_response_ << "ERROR: bad or missing parameter for " << command << '(' << parameter_vector << ")!\n";
			log_.Warningf("%s has missing or bad arguments (%s)", command.c_str(), strutil::Join(parameter_vector, ", ").c_str());
		}
	}
	return (result);
}



loginstance(kConsole, TrabantSimConsoleManager);



}
