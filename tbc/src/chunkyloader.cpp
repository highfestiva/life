
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/chunkyloader.h"
#include "../../lepra/include/packer.h"
#include "../include/bones.h"
#include "../include/chunkybonegeometry.h"
#include "../include/chunkyclass.h"
#include "../include/chunkyphysics.h"
#include "../include/physicsengine.h"
#include "../include/physicsspawner.h"
#include "../include/physicstrigger.h"



namespace tbc {



ChunkyLoader::ChunkyFileElement::ChunkyFileElement():
	type_((ChunkyType)0),
	load_callback_(false),
	int_pointer_(0),
	pointer_(0),
	s_(0),
	field_size_(0),
	element_count_(0),
	is_element_loaded_(false) {
}

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType type, void** pointer, uint32* field_size, int element_count):
	type_(type),
	load_callback_(false),
	int_pointer_(0),
	pointer_(pointer),
	s_(0),
	field_size_(field_size),
	element_count_(element_count),
	is_element_loaded_(false) {
}

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType type, int32* i, uint32* field_size, int element_count):
	type_(type),
	load_callback_(false),
	int_pointer_(i),
	pointer_(0),
	s_(0),
	field_size_(field_size),
	element_count_(element_count),
	is_element_loaded_(false) {
}

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType type, str* s, int element_count):
	type_(type),
	load_callback_(false),
	int_pointer_(0),
	pointer_(0),
	s_(s),
	field_size_(0),
	element_count_(element_count),
	is_element_loaded_(false) {
}

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType type, void* pointer, int element_count):
	type_(type),
	load_callback_(true),
	int_pointer_(0),
	pointer_((void**)pointer),
	s_(0),
	field_size_(0),
	element_count_(element_count),
	is_element_loaded_(false) {
}

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(const ChunkyFileElement& original) {
	*this = original;
}

void ChunkyLoader::ChunkyFileElement::operator=(const ChunkyFileElement& original) {
	type_ = original.type_;
	load_callback_ = original.load_callback_;
	int_pointer_ = original.int_pointer_;
	pointer_ = original.pointer_;
	s_ = original.s_;
	field_size_ = original.field_size_;
	element_count_ = original.element_count_;
	is_element_loaded_ = original.is_element_loaded_;
}



ChunkyLoader::ChunkyLoader(File* file, bool is_file_owner):
	file_(file),
	is_file_owner_(is_file_owner) {
}

ChunkyLoader::~ChunkyLoader() {
	if (is_file_owner_) {
		delete (file_);
	}
	file_ = 0;
}



bool ChunkyLoader::AllocLoadChunkyList(FileElementList& load_list, int64 chunk_end) {
	bool loaded_all_elements = false;
	bool ok = true;
	bool already_loaded = false;
	int64 head_position = file_->Tell();
	while (!loaded_all_elements && ok && file_->Tell() < chunk_end) {
		ChunkyType _type = (ChunkyType)0;
		uint32 _size = 0;
		int64 _chunk_end_position = 0;

		// Load element head.
		if (ok) {
			head_position = file_->Tell();
			ok = LoadHead(_type, _size, _chunk_end_position);
			deb_assert(ok);
#define C(t)	_type == t
			deb_assert(C(kChunkMesh) || C(kChunkMeshVertices) || C(kChunkMeshNormals) || C(kChunkMeshUv) || C(kChunkMeshUvsPerVertex) ||
				C(kChunkMeshColor) || C(kChunkMeshColorFormat) || C(kChunkMeshTriangles) ||
				C(kChunkMeshPrimitive) || C(kChunkMeshVolatility) || C(kChunkMeshCastsShadows) || C(kChunkMeshShadowDeviation) || C(kChunkMeshTwoSided) || C(kChunkMeshRecvNoShadows) ||
				C(kChunkSkin) || C(kChunkSkinBoneWeightGroup) || C(kChunkSkinBwgBones) ||
				C(kChunkSkinBwgVertices) || C(kChunkSkinBwgWeights) || C(kChunkAnimation) || C(kChunkAnimationMode) ||
				C(kChunkAnimationFrameCount) || C(kChunkAnimationBoneCount) || C(kChunkAnimationUseSplines) || C(kChunkAnimationTime) ||
				C(kChunkAnimationRootNode) || C(kChunkAnimationKeyframe) || C(kChunkAnimationKeyframeTime) || C(kChunkAnimationKeyframeTransform) ||
				C(kChunkPhysics) || C(kChunkPhysicsBoneCount) || C(kChunkPhysicsPhysicsType) || C(kChunkPhysicsPhysicsGuideMode) ||
				C(kChunkPhysicsEngineCount) || C(kChunkPhysicsTriggerCount) || C(kChunkPhysicsSpawnerCount) ||
				C(kChunkPhysicsBoneContainer) || C(kChunkPhysicsBoneChildList) || C(kChunkPhysicsBoneTransform) ||
				C(kChunkPhysicsBoneShape) || C(kChunkPhysicsEngineContainer) || C(kChunkPhysicsEngine) || C(kChunkPhysicsTriggerContainer) ||
				C(kChunkPhysicsTrigger) || C(kChunkPhysicsSpawnerContainer) || C(kChunkPhysicsSpawner) ||
				C(kChunkClass) || C(kChunkClassInheritanceList) || C(kChunkClassPhysics) ||
				C(kChunkClassSettings) || C(kChunkClassMeshList) || C(kChunkClassPhysMesh) ||
				C(kChunkClassTagList) || C(kChunkClassTag) || C(kChunkGroupClassList) ||
				C(kChunkGroupSettings) || C(kChunkWorldInfo) || C(kChunkWorldQuadList) || C(kChunkWorldGroupList));
		}

		// Load element contents, or skip it if the supplied list does not want it.
		bool element_found = false;
		if (ok) {
			FileElementList::iterator x = load_list.begin();
			for (; ok && !element_found && x != load_list.end(); ++x) {
				ChunkyFileElement& element = *x;
				if (ok && element.type_ == _type) {
					element_found = true;
					already_loaded = element.is_element_loaded_;
					ok = !already_loaded;
					if (ok) {
						FileElementList::iterator y = x;
						// Check that elements appear in the same order as given in the load list, otherwise
						// we're looking at a new (optional) element in a chunk array.
						for (++y; ok && y != load_list.end(); ++y) {
							const ChunkyFileElement& _element = *y;
							already_loaded = _element.is_element_loaded_;
							ok = !already_loaded;
						}
					}
					if (ok) {
						char c[4];
						c[0] = (char)((_type>>24)&0x7F);
						c[1] = (char)((_type>>16)&0x7F);
						c[2] = (char)((_type>>8)&0x7F);
						c[3] = (char)((_type>>0)&0x7F);
						log_volatile(log_.Tracef("Loading chunk '%c%c%c%c'.",
							c[0], c[1], c[2], c[3]));
					}
					element.is_element_loaded_ = true;
					const int max_element_count = ::abs(element.element_count_);
					for (int y = 0; ok && y < max_element_count; ++y) {
						if (element.load_callback_) {
							ok = LoadElementCallback(_type, _size, _chunk_end_position, element.pointer_);
							deb_assert(ok);
						} else if (element.int_pointer_) {
							ok = (file_->Read(element.int_pointer_[y]) == kIoOk);
							deb_assert(ok);
							if (element.field_size_) {
								*element.field_size_ += sizeof(int32);
							}
						} else if (element.pointer_) {
							ok = (file_->AllocReadData(&element.pointer_[y], _size) == kIoOk);
							deb_assert(ok);
							if (ok) {
								element.field_size_[y] = _size;
							}
						} else if (element.s_) {
							uint8* _s = 0;
							ok = (_size >= 4 && (_size&1) == 0);
							deb_assert(ok);
							if (ok) {
								ok = (file_->AllocReadData((void**)&_s, _size) == kIoOk);
								deb_assert(ok);
							}
							if (ok) {
								int string_length = PackerUnicodeString::Unpack(element.s_[y], _s, _size);
								ok = (string_length == (int)_size || string_length == (int)_size-2);
								deb_assert(ok);
								if (!ok) {
									log_.Error("Could not unpack string!");
								}
							}
							delete[] (_s);
						}

						if (file_->Tell() >= _chunk_end_position) {
							ok = (y == element.element_count_-1 || element.element_count_ <= 0);
							deb_assert(ok);
							if (!ok) {
								log_.Errorf("Trying to load %i elements,"
									" but only %i present. Format definition error?",
									element.element_count_, y+1);
							} break;
						}
					}
				}
			}
			if (ok) {
				if (element_found) {
					ok = (file_->Tell() == _chunk_end_position);
					deb_assert(ok);	// This probably means we put in too few elements in our load array when calling.
				} else {
					// Unknown element, try to load it separately.
					ok = (file_->Skip(_size) == kIoOk);
					deb_assert(ok);
				}
			}
		}

		// Check if all desired elements have been loaded.
		if (ok && element_found) {
			FileElementList::iterator x = load_list.begin();
			for (; x != load_list.end(); ++x) {
				ChunkyFileElement& element = *x;
				loaded_all_elements = element.is_element_loaded_;
				if (!loaded_all_elements) {
					break;
				}
			}
		}
	}
	if (!ok && already_loaded) {
		ok = (file_->SeekSet(head_position) == head_position);
	}
	if (ok) {
		ok = (file_->Tell() <= chunk_end);
	}
	deb_assert(ok);
	return (ok);
}



bool ChunkyLoader::SaveSingleString(ChunkyType type, const str& s) {
	const uint32 _size = PackerUnicodeString::Pack(0, s);	// Padding added.
	int64 _chunk_end_position = 0;
	bool ok = true;
	if (ok) {
		ok = SaveHead(type, _size, _chunk_end_position);
	}
	if (ok) {
		uint8* _data = new uint8[_size];
		::memset(_data, 0, _size);
		PackerUnicodeString::Pack(_data, s);
		ok = (file_->WriteData(_data, _size) == kIoOk);
		delete[] (_data);
	}
	if (ok) {
		ok = (file_->Tell() == _chunk_end_position);
	}
	return (ok);
}

/*bool ChunkyLoader::SaveStringList(const std::list<str>& pStringList) {
	// Compute size of all strings.
	size_t _size = 0;
	std::list<str>::const_iterator x = pStringList.begin();
	for (; x != pStringList.end(); ++x) {
		const str& _s = *x;
		const int HEAD_SIZE = 4*2;
		_size += HEAD_SIZE + ((_s.length()+1+3)&(~3));
	}

	int64 _chunk_end_position = 0;
	bool ok = true;
	if (ok) {
		ok = SaveHead(CHUNK_STRING_LIST, _size, _chunk_end_position);
	}
	x = pStringList.begin();
	for (; ok && x != pStringList.end(); ++x) {
		const str& _s = *x;
		ok = SaveSingleString(_s);
	}
	if (ok) {
		ok = (file_->Tell() == _chunk_end_position);
	}
	return (ok);
}*/

bool ChunkyLoader::SaveInt(ChunkyType type, int32 i) {
	int64 _chunk_end_position = 0;
	bool ok = true;
	if (ok) {
		ok = SaveHead(type, 4, _chunk_end_position);
	}
	if (ok) {
		ok = (file_->Write(i) == kIoOk);
	}
	if (ok) {
		ok = (file_->Tell() == _chunk_end_position);
	}
	return (ok);
}

bool ChunkyLoader::SaveChunkyList(const FileElementList& save_list) {
	bool ok = true;
	FileElementList::const_iterator x = save_list.begin();
	for (; ok && x != save_list.end(); ++x) {
		const ChunkyFileElement& element = *x;
		for (int y = 0; ok && y < element.element_count_; ++y) {
			int64 _chunk_end_position = 0;
			if (element.int_pointer_) {
				ok = SaveInt(element.type_, element.int_pointer_[y]);
			} else if (element.pointer_) {
				ok = SaveHead(element.type_, element.field_size_[y], _chunk_end_position);
				if (ok) {
					ok = (file_->WriteData(element.pointer_[y], element.field_size_[y]) == kIoOk);
				}
			} else if (element.s_) {
				ok = SaveSingleString(element.type_, element.s_[y]);
			}
		}
	}
	return (ok);
}



bool ChunkyLoader::LoadElementCallback(ChunkyType, uint32, int64 chunk_end_position, void*) {
	// Default behavior is to simply skip the chunk.
	bool ok = (file_->SeekSet(chunk_end_position) == chunk_end_position);
	return (ok);
}



bool ChunkyLoader::VerifyFileType(ChunkyType type) {
	uint32 _size = 0;
	int64 _chunk_end_position = 0;
	bool ok = LoadRequiredHead(type, _size, _chunk_end_position);
	if (ok && _chunk_end_position != file_->GetSize()) {
		ok = false;
	}
	return (ok);
}

bool ChunkyLoader::WriteFileType(ChunkyType type) {
	uint32 _size = 0;
	int64 _chunk_end_position;
	bool ok = SaveHead(type, _size, _chunk_end_position);
	return (ok);
}



bool ChunkyLoader::LoadHead(ChunkyType& type, uint32& size, int64& chunk_end_position) {
	bool ok = true;
	int32 temp_type = 0;
	if (ok) {
		ok = (file_->Read(temp_type) == kIoOk);
		type = (ChunkyType)temp_type;
	}
	if (ok) {
		uint32 _size;
		ok = (file_->Read(_size) == kIoOk);
		size = _size;
	}
	if (ok) {
		chunk_end_position = file_->Tell()+((size+3)&(~3));
	}
	return (ok);
}

bool ChunkyLoader::LoadRequiredHead(ChunkyType type, uint32& size, int64& chunk_end_position) {
	bool ok = true;
	ChunkyType file_type = (ChunkyType)0;
	if (ok) {
		ok = LoadHead(file_type, size, chunk_end_position);
	}
	if (ok) {
		ok = (type == file_type);
	}
	return (ok);
}

bool ChunkyLoader::SaveHead(ChunkyType type, uint32 size, int64& chunk_end_position) {
	bool ok = true;
	if (ok) {
		int32 temp_type = (int32)type;
		ok = (file_->Write(temp_type) == kIoOk);
	}
	if (ok) {
		uint32 _size = (uint32)size;
		ok = (file_->Write(_size) == kIoOk);
	}
	if (ok) {
		chunk_end_position = file_->Tell()+((size+3)&(~3));
	}
	return (ok);
}

bool ChunkyLoader::RewriteChunkSize(int64 chunk_start_position) {
	uint32 _size = (uint32)(file_->Tell() - chunk_start_position);
	file_->SeekSet(chunk_start_position-4);
	bool ok = (file_->Write(_size) == kIoOk);
	file_->SeekEnd(0);
	return (ok);
}

uint32* ChunkyLoader::AllocInitBigEndian(const float* data, unsigned count) {
	if (!data || !count) {
		return (0);
	}
	uint32* _data = new uint32[count];
	for (unsigned x = 0; x < count; ++x) {
		_data[x] = Endian::HostToBigF(data[x]);
	}
	return (_data);
}

uint32* ChunkyLoader::AllocInitBigEndian(const uint32* data, unsigned count) {
	if (!data || !count) {
		return (0);
	}
	uint32* _data = new uint32[count];
	for (unsigned x = 0; x < count; ++x) {
		_data[x] = Endian::HostToBig(data[x]);
	}
	return (_data);
}

uint32* ChunkyLoader::AllocInitBigEndian(const uint16* data, unsigned count) {
	if (!data || !count) {
		return (0);
	}
	uint32* _data = new uint32[count];
	for (unsigned x = 0; x < count; ++x) {
		_data[x] = Endian::HostToBig(data[x]);
	}
	return (_data);
}



ChunkyAnimationLoader::ChunkyAnimationLoader(File* file, bool is_file_owner):
	Parent(file, is_file_owner) {
}

ChunkyAnimationLoader::~ChunkyAnimationLoader() {
}

bool ChunkyAnimationLoader::Load(BoneAnimation* animation) {
	bool ok = true;
	if (ok) {
		ok = VerifyFileType(kChunkAnimation);
	}

	int32 default_mode = -1;
	keyframe_count_ = -1;	// Using member variable for callback l8r on.
	bone_count_ = -1;	// Using member variable for callback l8r on.
	int32 use_splines = -1;
	float animation_time_length = 0;
	str root_node_name;
	if (ok) {
		FileElementList _load_list;
		_load_list.push_back(ChunkyFileElement(kChunkAnimationMode, &default_mode));
		_load_list.push_back(ChunkyFileElement(kChunkAnimationFrameCount, &keyframe_count_));
		_load_list.push_back(ChunkyFileElement(kChunkAnimationBoneCount, &bone_count_));
		_load_list.push_back(ChunkyFileElement(kChunkAnimationUseSplines, &use_splines));
		_load_list.push_back(ChunkyFileElement(kChunkAnimationTime, (int32*)&animation_time_length));
		_load_list.push_back(ChunkyFileElement(kChunkAnimationRootNode, &root_node_name));
		ok = AllocLoadChunkyList(_load_list, file_->GetSize());
	}
	if (ok) {
		// Check that all mandatories have been found.
		ok = (default_mode != -1 && keyframe_count_ != -1 && bone_count_ != -1 && use_splines != -1 &&
			animation_time_length > 0 && animation_time_length < 1e4 && root_node_name.length() > 0);
	}
	if (ok) {
		animation->SetDefaultMode((BoneAnimation::Mode)default_mode);
		animation->SetKeyframeCount(keyframe_count_, use_splines != 0);
		animation->SetBoneCount(bone_count_);
		animation->SetTimeTag(bone_count_, animation_time_length);
		animation->SetRootNodeName(root_node_name);
	}
	if (ok) {
		FileElementList _load_list;
		current_keyframe_ = 0;
		_load_list.push_back(ChunkyFileElement(kChunkAnimationKeyframe, (void*)animation, keyframe_count_));
		ok = AllocLoadChunkyList(_load_list, file_->GetSize());
	}
	return (ok);
}

bool ChunkyAnimationLoader::Save(const BoneAnimation* animation) {
	// Write file header. We will come back to it later to re-write the actual size.
	bool ok = true;
	if (ok) {
		ok = WriteFileType(kChunkAnimation);
	}
	int64 file_data_start = file_->Tell();

	// Write animation settings and similar stuff.
	int32 default_mode = animation->GetDefaultMode();
	int32 keyframe_count = animation->GetKeyframeCount();
	int32 bone_count = animation->GetBoneCount();
	int32 use_splines = animation->GetUseSplines();
	float animation_time_length = animation->GetTimeTag(keyframe_count);
	str root_node_name = animation->GetRootNodeName();
	if (ok) {
		ok = (default_mode >= BoneAnimation::kModePlayOnce && keyframe_count > 0 &&
			bone_count > 0 && animation_time_length > 0 && root_node_name.length() > 0);
	}
	if (ok) {
		FileElementList _save_list;
		_save_list.push_back(ChunkyFileElement(kChunkAnimationMode, &default_mode));
		_save_list.push_back(ChunkyFileElement(kChunkAnimationFrameCount, &keyframe_count));
		_save_list.push_back(ChunkyFileElement(kChunkAnimationBoneCount, &bone_count));
		_save_list.push_back(ChunkyFileElement(kChunkAnimationUseSplines, &use_splines));
		_save_list.push_back(ChunkyFileElement(kChunkAnimationTime, (int32*)&animation_time_length));
		_save_list.push_back(ChunkyFileElement(kChunkAnimationRootNode, &root_node_name));
		ok = SaveChunkyList(_save_list);
	}

	// Write actual keyframes.
	const unsigned transform_float_count = 3+4;
	if (ok) {
		// Write all keyframes into a single chunk (multiple element chunk).
		const uint32 frame_size = (sizeof(uint32)*5 + bone_count*transform_float_count*sizeof(float)) * keyframe_count;
		int64 _chunk_end_position = 0;
		ok = SaveHead(kChunkAnimationKeyframe, frame_size, _chunk_end_position);
	}
	if (ok) {
		for (int kf = 0; ok && kf < keyframe_count; ++kf) {
			// Save time as sub-chunk to the keyframe chunk.
			float animation_time = animation->GetTimeTag(kf);
			ok = SaveInt(kChunkAnimationKeyframeTime, *(int32*)&animation_time);

			// Save transformations for all bones as sub-chunk to the keyframe chunk.
			const uint32 transform_size = bone_count*transform_float_count*sizeof(float);
			int64 _chunk_end_position = 0;
			ok = SaveHead(kChunkAnimationKeyframeTransform, transform_size, _chunk_end_position);
			for (int bc = 0; ok && bc < bone_count; ++bc) {
				float transform[transform_float_count];
				animation->GetBoneTransformation(kf, bc).Get(transform);
				ok = (file_->WriteData(transform, sizeof(transform)) == kIoOk);
			}
		}
	}

	// Re-write file header size.
	if (ok) {
		ok = RewriteChunkSize(file_data_start);
	}

	return (ok);
}



bool ChunkyAnimationLoader::LoadElementCallback(ChunkyType type, uint32 size, int64 chunk_end_position, void* storage) {
	bool ok = false;
	if (type == kChunkAnimationKeyframe) {
		// Setup pointers and counters for list loading.
		float keyframe_time = 0;
		float* transform_array = 0;
		uint32 float_byte_size = 0;
		FileElementList _load_list;
		_load_list.push_back(ChunkyFileElement(kChunkAnimationKeyframeTime, (int32*)&keyframe_time));
		_load_list.push_back(ChunkyFileElement(kChunkAnimationKeyframeTransform, (void**)&transform_array, &float_byte_size));
		ok = AllocLoadChunkyList(_load_list, chunk_end_position);

		const unsigned transform_float_count = 3+4;
		if (ok) {
			const unsigned float_count = transform_float_count*bone_count_;
			ok = (float_byte_size == float_count*sizeof(float));
		}

		if (ok) {
			BoneAnimation* _animation = (BoneAnimation*)storage;
			_animation->SetTimeTag(current_keyframe_, keyframe_time);
			for (int bone = 0; bone < bone_count_; ++bone) {
				xform transform(&transform_array[bone*transform_float_count]);
				_animation->SetBoneTransformation(current_keyframe_, bone, transform);
			}
			++current_keyframe_;
		}

		delete[] (transform_array);
	} else {
		ok = Parent::LoadElementCallback(type, size, chunk_end_position, storage);
	}
	return (ok);
}



ChunkyPhysicsLoader::ChunkyPhysicsLoader(File* file, bool is_file_owner):
	Parent(file, is_file_owner) {
}

ChunkyPhysicsLoader::~ChunkyPhysicsLoader() {
}

bool ChunkyPhysicsLoader::Load(ChunkyPhysics* physics) {
	bool ok = true;
	if (ok) {
		ok = VerifyFileType(kChunkPhysics);
	}

	int32 bone_count = -1;
	int32 physics_type = -1;
	int32 guide_mode = 1;
	int32 engine_count = -1;
	int32 trigger_count = -1;
	int32 spawner_count = -1;
	if (ok) {
		FileElementList _load_list;
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsBoneCount, &bone_count));
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsPhysicsType, &physics_type));
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsPhysicsGuideMode, &guide_mode));
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsEngineCount, &engine_count));
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsTriggerCount, &trigger_count));
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsSpawnerCount, &spawner_count));
		ok = AllocLoadChunkyList(_load_list, file_->GetSize());
		deb_assert(ok);
	}
	if (ok) {
		// Check that all mandatories have been found.
		ok = (bone_count >= 1 && bone_count < 10000 &&
			(physics_type == ChunkyPhysics::kWorld || physics_type == ChunkyPhysics::kStatic || physics_type == ChunkyPhysics::kDynamic) &&
			(guide_mode >= ChunkyPhysics::kGuideNever && guide_mode <= ChunkyPhysics::kGuideAlways) &&
			engine_count >= 0 && engine_count < 1000 &&
			trigger_count >= 0 && trigger_count < 1000);
	}
	if (ok) {
		physics->SetBoneCount(bone_count);
		physics->SetPhysicsType((ChunkyPhysics::PhysicsType)physics_type);
		physics->SetGuideMode((ChunkyPhysics::GuideMode)guide_mode);

		FileElementList _load_list;
		current_bone_index_ = 0;
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsBoneContainer, (void*)physics, bone_count));
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsEngineContainer, (void*)physics, engine_count));
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsTriggerContainer, (void*)physics, trigger_count));
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsSpawnerContainer, (void*)physics, spawner_count));
		ok = AllocLoadChunkyList(_load_list, file_->GetSize());
		deb_assert(ok);
	}
	if (ok) {
		ok = (engine_count == physics->GetEngineCount());
		deb_assert(ok);
	}
	if (ok) {
		ok = (trigger_count == physics->GetTriggerCount());
		deb_assert(ok);
	}

	return (ok);
}

bool ChunkyPhysicsLoader::Save(const ChunkyPhysics* physics) {
	bool ok = (physics->GetBoneCount() > 0);

	// Write file header. We will come back to it later to re-write the actual size.
	if (ok) {
		ok = WriteFileType(kChunkPhysics);
	}
	const int64 file_data_start = file_->Tell();

	// Write physics modes and similar stuff.
	int32 bone_count = physics->GetBoneCount();
	int32 physics_type = physics->GetPhysicsType();
	int32 guide_mode = physics->GetGuideMode();
	int32 engine_count = physics->GetEngineCount();
	int32 trigger_count = physics->GetTriggerCount();
	int32 spawner_count = physics->GetSpawnerCount();
	if (ok) {
		ok = (bone_count > 0 && bone_count < 10000 &&
			physics_type >= ChunkyPhysics::kWorld &&
			physics_type <= ChunkyPhysics::kDynamic &&
			guide_mode >= ChunkyPhysics::kGuideNever &&
			engine_count >= 0 && engine_count < 1000 &&
			trigger_count >= 0 && trigger_count < 1000);
	}
	if (ok) {
		FileElementList _save_list;
		_save_list.push_back(ChunkyFileElement(kChunkPhysicsBoneCount, &bone_count));
		_save_list.push_back(ChunkyFileElement(kChunkPhysicsPhysicsType, &physics_type));
		_save_list.push_back(ChunkyFileElement(kChunkPhysicsPhysicsGuideMode, &guide_mode));
		_save_list.push_back(ChunkyFileElement(kChunkPhysicsEngineCount, &engine_count));
		_save_list.push_back(ChunkyFileElement(kChunkPhysicsTriggerCount, &trigger_count));
		_save_list.push_back(ChunkyFileElement(kChunkPhysicsSpawnerCount, &spawner_count));
		ok = SaveChunkyList(_save_list);
	}

	// Write actual bone positioning and geometries.
	if (ok) {
		// Write all bone stuff into a single chunk (multiple element chunk).
		int64 _chunk_end_position = 0;
		ok = SaveHead(kChunkPhysicsBoneContainer, 0, _chunk_end_position);
	}
	const int64 bone_chunk_start = file_->Tell();
	for (int b = 0; ok && b < bone_count; ++b) {
		// Save children.
		const int cc = physics->GetBoneChildCount(b);
		if (cc > 0) {
			int64 _chunk_end_position = 0;
			ok = SaveHead(kChunkPhysicsBoneChildList, cc*sizeof(int32), _chunk_end_position);
			for (int c = 0; ok && c < cc; ++c) {
				int32 child_index = physics->GetChildIndex(b, c);
				file_->Write(child_index);
			}
		}

		// Save bone's original transformation.
		if (ok) {
			const unsigned transform_float_count = 3+4;
			const uint32 transform_size = transform_float_count*sizeof(float);
			int64 _chunk_end_position = 0;
			ok = SaveHead(kChunkPhysicsBoneTransform, transform_size, _chunk_end_position);
			uint32 transform[transform_float_count];
			physics->GetBoneTransformation(b).Get((float*)transform);
			// Convert to network byte order.
			for (unsigned x = 0; x < transform_float_count; ++x) {
				transform[x] = Endian::HostToBig(transform[x]);
			}
			ok = (file_->WriteData(transform, sizeof(transform)) == kIoOk);
		}

		// Save geometries.
		if (ok) {
			ChunkyBoneGeometry* geometry = physics->GetBoneGeometry(b);
			if (geometry) {
				int64 _chunk_end_position = 0;
				unsigned _size = geometry->GetChunkySize();
				ok = SaveHead(kChunkPhysicsBoneShape, _size, _chunk_end_position);
				if (ok) {
					char* _data = new char[_size];
					geometry->SaveChunkyData(physics, _data);
					deb_assert(geometry->GetChunkySize(_data) == _size);
					ok = (file_->WriteData(_data, _size) == kIoOk);
					delete (_data);
				}
			}
		}
	}
	if (ok) {
		ok = RewriteChunkSize(bone_chunk_start);
	}

	// Write engine header.
	if (ok) {
		// Write all bone stuff into a single chunk (multiple element chunk).
		int64 _chunk_end_position = 0;
		ok = SaveHead(kChunkPhysicsEngineContainer, 0, _chunk_end_position);
	}
	const int64 engine_chunk_start = file_->Tell();
	// Write engines.
	for (int e = 0; ok && e < engine_count; ++e) {
		PhysicsEngine* engine = physics->GetEngine(e);
		deb_assert(engine);
		int64 _chunk_end_position = 0;
		unsigned _size = engine->GetChunkySize();
		ok = SaveHead(kChunkPhysicsEngine, _size, _chunk_end_position);
		if (ok) {
			char* _data = new char[_size];
			engine->SaveChunkyData(physics, _data);
			ok = (file_->WriteData(_data, _size) == kIoOk);
			delete (_data);
		}
	}
	if (ok) {
		ok = RewriteChunkSize(engine_chunk_start);
	}

	// Write trigger header.
	if (ok) {
		// Write all bone stuff into a single chunk (multiple element chunk).
		int64 _chunk_end_position = 0;
		ok = SaveHead(kChunkPhysicsTriggerContainer, 0, _chunk_end_position);
	}
	const int64 trigger_chunk_start = file_->Tell();
	// Write triggers.
	for (int t = 0; ok && t < trigger_count; ++t) {
		const PhysicsTrigger* trigger = physics->GetTrigger(t);
		deb_assert(trigger);
		int64 _chunk_end_position = 0;
		unsigned _size = trigger->GetChunkySize();
		ok = SaveHead(kChunkPhysicsTrigger, _size, _chunk_end_position);
		if (ok) {
			char* _data = new char[_size];
			trigger->SaveChunkyData(physics, _data);
			ok = (file_->WriteData(_data, _size) == kIoOk);
			delete (_data);
		}
	}
	if (ok) {
		ok = RewriteChunkSize(trigger_chunk_start);
	}

	// Write spawner header.
	if (ok) {
		int64 _chunk_end_position = 0;
		ok = SaveHead(kChunkPhysicsSpawnerContainer, 0, _chunk_end_position);
	}
	const int64 spawner_chunk_start = file_->Tell();
	// Write triggers.
	for (int t = 0; ok && t < spawner_count; ++t) {
		const PhysicsSpawner* spawner = physics->GetSpawner(t);
		deb_assert(spawner);
		int64 _chunk_end_position = 0;
		unsigned _size = spawner->GetChunkySize();
		ok = SaveHead(kChunkPhysicsSpawner, _size, _chunk_end_position);
		if (ok) {
			char* _data = new char[_size];
			spawner->SaveChunkyData(physics, _data);
			ok = (file_->WriteData(_data, _size) == kIoOk);
			delete (_data);
		}
	}
	if (ok) {
		ok = RewriteChunkSize(spawner_chunk_start);
	}

	// Re-write file header size.
	if (ok) {
		ok = RewriteChunkSize(file_data_start);
	}

	return (ok);
}

bool ChunkyPhysicsLoader::LoadElementCallback(ChunkyType type, uint32 size, int64 chunk_end_position, void* storage) {
	ChunkyPhysics* _physics = (ChunkyPhysics*)storage;
	bool ok = false;
	if (type == kChunkPhysicsBoneContainer) {
		// Setup pointers and counters for list loading.
		const int MAXIMUM_CHILD_BONES = 256;
		int32 child_array[MAXIMUM_CHILD_BONES];
		::memset(child_array, -1, sizeof(child_array));
		unsigned child_byte_size = 0;
		uint32* transform_array = 0;
		unsigned float_byte_size = 0;
		uint32* geometry_array = 0;
		unsigned geometry_byte_size = 0;
		FileElementList _load_list;
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsBoneChildList, child_array, &child_byte_size, -MAXIMUM_CHILD_BONES));
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsBoneTransform, (void**)&transform_array, (unsigned*)&float_byte_size));
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsBoneShape, (void**)&geometry_array, (unsigned*)&geometry_byte_size));
		ok = AllocLoadChunkyList(_load_list, chunk_end_position);

		// Check child array byte size.
		int child_count = 0;
		if (ok) {
			ok = ((child_byte_size&3) == 0);
			child_count = child_byte_size/sizeof(int32);
		}
		// Check bone transformation byte size.
		const unsigned transform_float_count = 3+4;
		if (ok) {
			const unsigned float_count = transform_float_count;
			ok = (float_byte_size == float_count*sizeof(float));
		}

		if (ok) {
			log_volatile(log_.Tracef("Current bone index is %i.",
				current_bone_index_));
			_physics->SetBoneChildCount(current_bone_index_, child_count);
			for (int x = 0; ok && x < child_count; ++x) {
				ok = (child_array[x] >= 0 && child_array[x] < _physics->GetBoneCount());
				if (ok) {
					_physics->SetChildIndex(current_bone_index_, x, child_array[x]);
				}
			}
			// Convert to host endian.
			for (unsigned x = 0; x < transform_float_count; ++x) {
				transform_array[x] = Endian::BigToHost(transform_array[x]);
			}
			xform transform((const float*)transform_array);
			_physics->SetOriginalBoneTransformation(current_bone_index_, transform);
		}

		if (ok && geometry_array) {
			ChunkyBoneGeometry* geometry = ChunkyBoneGeometry::Load(_physics,
				geometry_array, geometry_byte_size);
			ok = (geometry != 0);
			if (ok) {
				_physics->AddBoneGeometry(geometry);
			}
		}

		deb_assert(ok);

		delete[] (geometry_array);
		delete[] (transform_array);
		++current_bone_index_;
	} else if (type == kChunkPhysicsEngineContainer) {
		uint32* engine_array = 0;
		unsigned engine_byte_size = 0;
		FileElementList _load_list;
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsEngine, (void**)&engine_array, (unsigned*)&engine_byte_size));
		ok = AllocLoadChunkyList(_load_list, chunk_end_position);

		PhysicsEngine* engine = 0;
		if (ok) {
			engine = PhysicsEngine::Load(_physics, engine_array, engine_byte_size);
			ok = (engine != 0);
		}
		if (ok) {
			_physics->AddEngine(engine);
		}

		delete[] (engine_array);
	} else if (type == kChunkPhysicsTriggerContainer) {
		uint32* trigger_array = 0;
		unsigned trigger_byte_size = 0;
		FileElementList _load_list;
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsTrigger, (void**)&trigger_array, (unsigned*)&trigger_byte_size));
		ok = AllocLoadChunkyList(_load_list, chunk_end_position);

		PhysicsTrigger* trigger = 0;
		if (ok) {
			trigger = PhysicsTrigger::Load(_physics, trigger_array, trigger_byte_size);
			ok = (trigger != 0);
			deb_assert(ok);
		}
		if (ok) {
			_physics->AddTrigger(trigger);
		}

		delete[] (trigger_array);
	} else if (type == kChunkPhysicsSpawnerContainer) {
		uint32* spawner_array = 0;
		unsigned spawner_byte_size = 0;
		FileElementList _load_list;
		_load_list.push_back(ChunkyFileElement(kChunkPhysicsSpawner, (void**)&spawner_array, (unsigned*)&spawner_byte_size));
		ok = AllocLoadChunkyList(_load_list, chunk_end_position);

		PhysicsSpawner* spawner = 0;
		if (ok) {
			spawner = PhysicsSpawner::Load(_physics, spawner_array, spawner_byte_size);
			ok = (spawner != 0);
			deb_assert(ok);
		}
		if (ok) {
			_physics->AddSpawner(spawner);
		}

		delete[] (spawner_array);
	} else {
		ok = Parent::LoadElementCallback(type, size, chunk_end_position, storage);
	}
	deb_assert(ok);
	return (ok);
}



ChunkyClassLoader::ChunkyClassLoader(File* file, bool is_file_owner):
	Parent(file, is_file_owner) {
}

ChunkyClassLoader::~ChunkyClassLoader() {
}

bool ChunkyClassLoader::Load(ChunkyClass* data) {
	bool ok = true;
	if (ok) {
		ok = VerifyFileType(kChunkClass);
	}

	if (ok) {
		FileElementList _load_list;
		AddLoadElements(_load_list, data);
		ok = AllocLoadChunkyList(_load_list, file_->GetSize());
	}
	if (ok) {
		ok = (!data->GetPhysicsBaseName().empty() &&
			true);	// TODO: check other tags (e.g. settings).
		if (!ok) {
			log_.Errorf("Could not load contents of class file %s!", data->GetPhysicsBaseName().c_str());
		}
	}

	return (ok);
}

void ChunkyClassLoader::AddLoadElements(FileElementList& element_list, ChunkyClass* data) {
	element_list.push_back(ChunkyFileElement(kChunkClassPhysics, &data->GetPhysicsBaseName()));
	element_list.push_back(ChunkyFileElement(kChunkClassTagList, (void*)data, -1000));
}

bool ChunkyClassLoader::LoadElementCallback(ChunkyType type, uint32 size, int64 chunk_end_position, void* storage) {
	ChunkyClass* clazz = (ChunkyClass*)storage;
	bool ok = false;
	if (type == kChunkClassTagList) {
		FileElementList _load_list;
		_load_list.push_back(ChunkyFileElement(kChunkClassTag, (void*)clazz));
		ok = AllocLoadChunkyList(_load_list, chunk_end_position);
		deb_assert(ok);
	} else if (type == kChunkClassTag) {
		uint8* buffer = 0;
		ok = (file_->AllocReadData((void**)&buffer, size) == kIoOk);
		deb_assert(ok);
		if (ok) {
			ok = clazz->UnpackTag(buffer, size);
			deb_assert(ok);
		}
		delete[] (buffer);
	} else if (type == tbc::kChunkClassMeshList) {
		FileElementList _load_list;
		_load_list.push_back(ChunkyFileElement(tbc::kChunkClassPhysMesh, (void*)clazz));
		ok = AllocLoadChunkyList(_load_list, chunk_end_position);
		deb_assert(ok);
	} else if (type == tbc::kChunkClassPhysMesh) {
		uint8* buffer = 0;
		ok = (file_->AllocReadData((void**)&buffer, size) == kIoOk);
		deb_assert(ok);
		if (ok) {
			int32 physics_index = Endian::BigToHost(*(int32*)buffer);
			clazz->AddPhysRoot(physics_index);
		}
		delete[] (buffer);
	} else {
		ok = Parent::LoadElementCallback(type, size, chunk_end_position, storage);
		deb_assert(ok);
	}
	return (ok);
}



loginstance(kGeneralResources, ChunkyLoader);
loginstance(kGeneralResources, ChunkyAnimationLoader);
loginstance(kGeneralResources, ChunkyPhysicsLoader);
loginstance(kGeneralResources, ChunkyClassLoader);



}
