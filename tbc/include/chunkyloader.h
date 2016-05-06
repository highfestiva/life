
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



// Chunky is a format of our own (who could have guessed) that is similar to good 'ol IFF.

// All chunks starts with the four-byte file signature, followed by a four-byte file data length.
// Immediately after follows the chunk's data, which may be raw data or a sub-chunk, depending
// on usage. All list items, for instance, are sub-chunks of the list chunk.

// The file itself has a top chunk which only contains the file data - the main purpose of this
// is to verify that file size is not corrupted.

// The design of the Chunky loaders and savers are quite complex. The core of the loader is the
// method AllocLoadChunkyList(), which both allocates (new()) and loads chunks. If you are in need
// of non-standard chunk layouts, you should override LoadElement() which defaults to skipping the
// current element, returning true.

// Note that all data is loaded and saved as native endian. This means that you need a conversion
// tool if you're going to load it on a different platform than you saved it on. On the upside
// is of course the performance gain, especially when loading BLOBs.

// Animation format:
// - File tag.
//   |- Number of keyframes.
//   |- ...
//   |- Root node name (for instance "l")
//   |- Keyframe data tag.
//      |- Keyframe time for keyframe #0.
//      |- Keyframe transformations for all bones for keyframe #0.
//      |- ...
//      |- Keyframe time for keyframe #n.
//      |- Keyframe transformations for all bones for keyframe #n.

// Physics format:
// - File tag.
//   |- Number of parts (=bones).
//   |- ...
//   |- Physics type (for instance "dynamic")
//   |- Bone data tag.
//      |- List of child bone IDs for bone #0.
//      |- Bone transformation for bone #0.
//      |- Bone shape for bone #0.
//      |- ...
//      |- List of child bone IDs for bone #n.
//      |- Bone transformation for bone #n.
//      |- Bone shape for bone #n.

#pragma once

#include "../../lepra/include/lepraassert.h"
#include <list>
#include "../../lepra/include/file.h"
#include "tbc.h"



namespace tbc {



class BoneAnimation;
class ChunkyClass;
class ChunkyGroup;
class ChunkyPhysics;
class ChunkyWorld;


#define ENUMROT(c, bits)	(((int)c)<<bits)
#define ENUMIFY(a, b, c, d)	ENUMROT(a, 24) + ENUMROT(b, 16) + ENUMROT(c, 8) +  ENUMROT(d, 0)

enum ChunkyType {
	// File type: mesh. Unly used by graphics applications.
	kChunkMesh                         = ENUMIFY('M','E','S','H'),	// Mesh file type.
	kChunkMeshVertices                = ENUMIFY('M','E','V','X'),	// Mandatory.
	kChunkMeshNormals                 = ENUMIFY('M','E','N','O'),	// Mandatory.
	kChunkMeshUv                      = ENUMIFY('M','E','U','V'),	// Optional.
	kChunkMeshUvsPerVertex          = ENUMIFY('M','E','U','P'),	// Optional.
	kChunkMeshColor                   = ENUMIFY('M','E','C','O'),	// Optional.
	kChunkMeshColorFormat            = ENUMIFY('M','E','C','F'),	// Optional.
	kChunkMeshTriangles               = ENUMIFY('M','E','T','R'),	// Mandatory, but could mean triangle strip, quad or something else.
	kChunkMeshPrimitive               = ENUMIFY('M','E','P','R'),	// Optional.
	kChunkMeshVolatility              = ENUMIFY('M','E','V','O'),	// Optional.
	kChunkMeshCastsShadows           = ENUMIFY('M','E','C','S'),	// Optional.
	kChunkMeshShadowDeviation        = ENUMIFY('M','E','S','D'),	// Optional.
	kChunkMeshTwoSided               = ENUMIFY('M','E','T','S'),	// Optional.
	kChunkMeshRecvNoShadows         = ENUMIFY('M','E','R','S'),	// Optional.

	// File type: skin. Unly used by graphics applications.
	kChunkSkin                         = ENUMIFY('S','K','I','N'),	// Skin file type.
	// Skin sub element: bone weight group. Used by skinning to determine bone-weight-per-vertex.
	kChunkSkinBoneWeightGroup       = ENUMIFY('S','K','W','G'),	// Optional.
	kChunkSkinBwgBones               = ENUMIFY('S','K','W','B'),	// Sub-object is mandatory if weight group present.
	kChunkSkinBwgVertices            = ENUMIFY('S','K','W','V'),	// Sub-object is mandatory if weight group present.
	kChunkSkinBwgWeights             = ENUMIFY('S','K','W','W'),	// Sub-object is mandatory if weight group present.

	// File type: animation. Used by both graphics and text applications.
	kChunkAnimation                    = ENUMIFY('A','N','I','M'),	// Animation file type.
	kChunkAnimationMode               = ENUMIFY('A','N','M','O'),	// "Looped" or "play once". Mandatory.
	kChunkAnimationFrameCount        = ENUMIFY('A','N','F','C'),	// Number of keyframes. Mandatory.
	kChunkAnimationBoneCount         = ENUMIFY('A','N','B','C'),	// Number of bones. Mandatory.
	kChunkAnimationUseSplines        = ENUMIFY('A','N','U','S'),	// Hehe, love the abbreviation... Mandatory.
	kChunkAnimationTime               = ENUMIFY('A','N','T','I'),	// Animation length in seconds. Mandatory.
	kChunkAnimationRootNode          = ENUMIFY('A','N','R','N'),	// A single NAME that corresponds to the start node in the physics hierarchy. Mandatory.
	// Animation sub element: keyframe.
	kChunkAnimationKeyframe           = ENUMIFY('A','N','K','A'),	// A keyframe and its transformations. Means "duck" in swedish. Mandatory.
	kChunkAnimationKeyframeTime      = ENUMIFY('A','N','T','M'),	// Time for this keyframe. Mandatory.
	kChunkAnimationKeyframeTransform = ENUMIFY('A','N','K','T'),	// Transformations for this keyframe for all bones in one BLOB. Mandatory.

	// File type: physics. Used by both graphics and text applications.
	kChunkPhysics                      = ENUMIFY('P','H','Y','S'),	// Physics file type.
	kChunkPhysicsBoneCount           = ENUMIFY('P','H','B','C'),	// Number of bones. Mandatory.
	kChunkPhysicsPhysicsType         = ENUMIFY('P','H','P','T'),	// Physics type: dynamic, static, etc. Mandatory.
	kChunkPhysicsPhysicsGuideMode   = ENUMIFY('P','H','G','M'),	// Never, sometimes or always guided. Optional.
	kChunkPhysicsEngineCount         = ENUMIFY('P','H','E','C'),	// Number of engines. Mandatory.
	kChunkPhysicsTriggerCount        = ENUMIFY('P','H','T','C'),	// Number of triggers. Mandatory.
	kChunkPhysicsSpawnerCount        = ENUMIFY('P','H','S','C'),	// Number of triggers. Mandatory.
	// Physics sub element: bone.
	kChunkPhysicsBoneContainer       = ENUMIFY('P','H','B','O'),	// A bone and its transformation, type. Mandatory.
	kChunkPhysicsBoneChildList      = ENUMIFY('P','H','C','L'),	// The indices of this bone's children. Optional for bones without children, mandatory otherwise.
	kChunkPhysicsBoneTransform       = ENUMIFY('P','H','B','T'),	// The transform of this bone. Mandatory.
	kChunkPhysicsBoneShape           = ENUMIFY('P','H','S','H'),	// Bone shape (i.e. capsule, box, sphere, portal...), mandatory with one and one shape only per bone.
	// Physics sub element: engine.
	kChunkPhysicsEngineContainer     = ENUMIFY('P','H','E','O'),	// Engine array. Mandatory.
	kChunkPhysicsEngine               = ENUMIFY('P','H','E','N'),	// An engine and its parameters, type. Mandatory.
	// Physics sub element: trigger.
	kChunkPhysicsTriggerContainer    = ENUMIFY('P','H','T','O'),	// Trigger array. Mandatory.
	kChunkPhysicsTrigger              = ENUMIFY('P','H','T','R'),	// A trigger and its parameters, type. Mandatory.
	// Physics sub element: spawner.
	kChunkPhysicsSpawnerContainer    = ENUMIFY('P','H','S','O'),	// Spawner array. Mandatory.
	kChunkPhysicsSpawner              = ENUMIFY('P','H','S','R'),	// A spawner and its parameters, type. Mandatory.

	// File type: class. Used by both graphics and text applications.
	kChunkClass                        = ENUMIFY('C','L','A','S'),	// Class file type.
	kChunkClassInheritanceList       = ENUMIFY('C','L','I','L'),	// Parent class information. List arranged after parent priority. Optional.
	kChunkClassPhysics                = ENUMIFY('C','L','P','H'),	// An physics filename of the physics that forms the shapes of this class. Optional - absent when physics inherited or for abstract nouns ("clan" and "weather").
	kChunkClassSettings               = ENUMIFY('C','L','S','E'),	// str keys and their corresponding default string values. Example "stand_animation":"heavy_walk". Optional.
	kChunkClassMeshList              = ENUMIFY('C','L','M','L'),	// A list of mesh filenames that forms the looks of this class. Only used by graphics applications, and completly ignored by text applications. Optional - absent when using inherited or for abstract nouns.
	kChunkClassPhysMesh              = ENUMIFY('C','L','P','M'),	// Physical connection between a bone and a mesh. Mandatory in sub-chunk array.
	kChunkClassTagList               = ENUMIFY('C','L','T','L'),	// A list of tags (usually for connecting arbitrary engines, bones or mesh nodes). Optional - absent when using inherited or for abstract nouns.
	kChunkClassTag                    = ENUMIFY('C','L','T','A'),	// Connection between bones, engines and meshes. Mainly used for graphical scripting behavior.

	// File type: Group. Used by both graphics and text applications.
	kChunkGroupClassList             = ENUMIFY('G','C','L','I'),	// A list of class filenames (each with a group-unique NAME) that forms the nouns of this group. Mandatory.
	kChunkGroupSettings               = ENUMIFY('G','R','S','E'),	// A list of settings and attachments. A setting/an attachment contains two strings. Example: (groupNAME1.physics_NAME1) and (groupNAME2.physics_NAME2). May override class settings. Optional.

	// File type: world. Used by both graphics and text applications.
	kChunkWorldInfo                   = ENUMIFY('W','I','N','F'),	// Global world data, such as pos+size. Mandatory.
	kChunkWorldQuadList              = ENUMIFY('W','Q','U','L'),	// Just a recursive container for a world group list. Container for kChunkWorldGroupList. Mandatory.
	kChunkWorldGroupList             = ENUMIFY('W','G','R','L'),	// A list where each element is a group filename and a transformation. Each instance hangs on branches and leafs in the kChunkWorldQuadList. Mandatory.
};



class ChunkyLoader {
public:
	// Used for both loading and saving. Hard-coded to some degree, but I actually find the code more
	// readable and flexible like this.
	class ChunkyFileElement {
	public:
		ChunkyFileElement();
		// Used for any non-32 bit, statically formed data. Allocates a byte array for each element loaded.
		// Field sizes are stored in field_size[e], where e is the element index.
		ChunkyFileElement(ChunkyType type, void** pointer, uint32* field_size, int element_count = 1);
		// Used for 32-bits (floats may use them too). Does not allocate. Stores total field size in *field_size.
		ChunkyFileElement(ChunkyType type, int32* i, uint32* field_size = 0, int element_count = 1);
		// Used for strings.
		ChunkyFileElement(ChunkyType type, str* s, int element_count = 1);
		// Used for receiving callbacks. Will call LoadElementCallback if this chunk type is found.
		ChunkyFileElement(ChunkyType type, void* pointer, int element_count = 1);
		ChunkyFileElement(const ChunkyFileElement& original);
		void operator=(const ChunkyFileElement& original);

		ChunkyType type_;
		bool load_callback_;
		int32* int_pointer_;
		void** pointer_;
		str* s_;
		uint32* field_size_;
		int element_count_;

		bool is_element_loaded_;
	};

	typedef std::list<ChunkyFileElement> FileElementList;

	ChunkyLoader(File* file, bool is_file_owner);
	virtual ~ChunkyLoader();

	//bool LoadSingleString(ChunkyType type, str& s);
	//bool LoadStringList(std::list<str>& pStringList);
	//bool LoadInt(ChunkyType type, int32& i);
	bool AllocLoadChunkyList(FileElementList& load_list, int64 chunk_end);

	bool SaveSingleString(ChunkyType type, const str& s);
	//bool SaveStringList(const std::list<str>& pStringList, ChunkyType type);
	bool SaveInt(ChunkyType type, int32 i);
	bool SaveChunkyList(const FileElementList& save_list);

protected:
	virtual bool LoadElementCallback(ChunkyType type, uint32 size, int64 chunk_end_position, void* storage);

	bool VerifyFileType(ChunkyType type);
	bool WriteFileType(ChunkyType type);

	bool LoadHead(ChunkyType& type, uint32& size, int64& chunk_end_position);
	bool LoadRequiredHead(ChunkyType required_type, uint32& size, int64& chunk_end_position);
	bool SaveHead(ChunkyType type, uint32 size, int64& chunk_end_position);
	bool RewriteChunkSize(int64 chunk_start_position);

	uint32* AllocInitBigEndian(const float* data, unsigned count);
	uint32* AllocInitBigEndian(const uint32* data, unsigned count);
	uint32* AllocInitBigEndian(const uint16* data, unsigned count);

	File* file_;
	bool is_file_owner_;

	logclass();
};



class ChunkyAnimationLoader: public ChunkyLoader {	// For bone animations.
	typedef ChunkyLoader Parent;
public:
	ChunkyAnimationLoader(File* file, bool is_file_owner);
	virtual ~ChunkyAnimationLoader();
	virtual bool Load(BoneAnimation* animation);
	virtual bool Save(const BoneAnimation* animation);

private:
	bool LoadElementCallback(tbc::ChunkyType type, uint32 size, int64 chunk_end_position, void* storage);

	int32 keyframe_count_;
	int32 bone_count_;
	int32 current_keyframe_;

	logclass();
};

class ChunkyPhysicsLoader: public ChunkyLoader {	// For physics and skinning. Loads the bone hierachy.
	typedef ChunkyLoader Parent;
public:
	ChunkyPhysicsLoader(File* file, bool is_file_owner);
	virtual ~ChunkyPhysicsLoader();
	virtual bool Load(ChunkyPhysics* data);
	virtual bool Save(const ChunkyPhysics* data);

private:
	bool LoadElementCallback(tbc::ChunkyType type, uint32 size, int64 chunk_end_position, void* storage);
	void SetBoneChildren(int bone_index);

	int32 current_bone_index_;

	logclass();
};

// Contans information on physics and animation names. Derived class handles kUi: mesh, materials, sounds, etc.
class ChunkyClassLoader: public ChunkyLoader {
	typedef ChunkyLoader Parent;
public:
	ChunkyClassLoader(File* file, bool is_file_owner);
	virtual ~ChunkyClassLoader();
	virtual bool Load(ChunkyClass* data);

protected:
	virtual void AddLoadElements(FileElementList& element_list, ChunkyClass* data);
	bool LoadElementCallback(ChunkyType type, uint32 size, int64 chunk_end_position, void* storage);

private:
	logclass();
};

/*// Contains class/group objects and links between them.
class ChunkyGroupLoader: public ChunkyLoader {
public:
	ChunkyGroupLoader(File* file, bool is_file_owner);
	virtual ~ChunkyGroupLoader();
	virtual bool Load(ChunkyGroup* data);
	virtual bool Save(const ChunkyGroup* data);
};

class ChunkyWorldLoader: public ChunkyLoader {	// Contains groups and terrain. A bit bold at this point? :)
public:
	ChunkyWorldLoader(File* file, bool is_file_owner);
	virtual ~ChunkyWorldLoader();
	virtual bool Load(ChunkyWorld* data);
	virtual bool Save(const ChunkyWorld* data);
};*/



}
