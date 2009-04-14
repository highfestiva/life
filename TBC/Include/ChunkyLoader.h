
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



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

// Structure format:
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



#ifndef CHUNKYLOADER_H
#define CHUNKYLOADER_H



#include <assert.h>
#include <list>
#include "../../Lepra/Include/File.h"
#include "TBC.h"



namespace TBC
{



class BoneAnimation;
class ChunkyClass;
class ChunkyGroup;
class ChunkyStructure;
class ChunkyWorld;



enum ChunkyType
{
	// File type: mesh. Unly used by graphics applications.
	CHUNK_MESH                         = 'MESH',	// Mesh file type.
	CHUNK_MESH_VERTICES                = 'MEVX',	// Mandatory.
	CHUNK_MESH_NORMALS                 = 'MENO',	// Mandatory.
	CHUNK_MESH_UV                      = 'MEUV',	// Optional.
	CHUNK_MESH_COLOR                   = 'MECO',	// Optional.
	CHUNK_MESH_COLOR_FORMAT            = 'MECF',	// Optional.
	CHUNK_MESH_TRIANGLES               = 'METR',	// A mesh can contain EITHER this type OR the strips type.
	CHUNK_MESH_STRIPS                  = 'MEST',	// A mesh can contain EITHER this type OR the triangles type.
	CHUNK_MESH_VOLATILITY              = 'MEVO',	// Optional.

	// File type: skin. Unly used by graphics applications.
	CHUNK_SKIN                         = 'SKIN',	// Skin file type.
	// Skin sub element: bone weight group. Used by skinning to determine bone-weight-per-vertex.
	CHUNK_SKIN_BONE_WEIGHT_GROUP       = 'SKWG',	// Optional.
	CHUNK_SKIN_BWG_BONES               = 'SKWB',	// Sub-object is mandatory if weight group present.
	CHUNK_SKIN_BWG_VERTICES            = 'SKWV',	// Sub-object is mandatory if weight group present.
	CHUNK_SKIN_BWG_WEIGHTS             = 'SKWW',	// Sub-object is mandatory if weight group present.

	// File type: animation. Used by both graphics and text applications.
	CHUNK_ANIMATION                    = 'ANIM',	// Animation file type.
	CHUNK_ANIMATION_MODE               = 'ANMO',	// "Looped" or "play once". Mandatory.
	CHUNK_ANIMATION_FRAME_COUNT        = 'ANFC',	// Number of keyframes. Mandatory.
	CHUNK_ANIMATION_BONE_COUNT         = 'ANBC',	// Number of bones. Mandatory.
	CHUNK_ANIMATION_USE_SPLINES        = 'ANUS',	// Hehe, love the abbreviation... Mandatory.
	CHUNK_ANIMATION_TIME               = 'ANTI',	// Animation length in seconds. Mandatory.
	CHUNK_ANIMATION_ROOT_NODE          = 'ANRN',	// A single NAME that corresponds to the start node in the structure hierarchy. Mandatory.
	// Animation sub element: keyframe.
	CHUNK_ANIMATION_KEYFRAME           = 'ANKA',	// A keyframe and its transformations. Means "duck" in swedish. Mandatory.
	CHUNK_ANIMATION_KEYFRAME_TIME      = 'ANTM',	// Time for this keyframe. Mandatory.
	CHUNK_ANIMATION_KEYFRAME_TRANSFORM = 'ANKT',	// Transformations for this keyframe for all bones in one BLOB. Mandatory.

	// File type: structure. Used by both graphics and text applications.
	CHUNK_STRUCTURE                    = 'STRU',	// Structure file type.
	CHUNK_STRUCTURE_PART_COUNT         = 'STPC',	// Number of bones. Mandatory.
	CHUNK_STRUCTURE_ROOT_BONE_INDEX    = 'STRI',	// Index of root bone. Mandatory.
	CHUNK_STRUCTURE_PHYSICS_TYPE       = 'STPT',	// Physics type: dynamic, static, etc. Mandatory.
	// Structure sub element: bone.
	CHUNK_STRUCTURE_BONE               = 'STBO',	// A bone and its transformation, type. Mandatory.
	CHUNK_STRUCTURE_BONE_CHILD_LIST    = 'STBC',	// The indices of this bone's children. Optional for bones without children, mandatory otherwise.
	CHUNK_STRUCTURE_BONE_TRANSFORM     = 'STBT',	// The transform of this bone. Mandatory.
	CHUNK_STRUCTURE_BONE_SHAPE_CAPSULE = 'STSC',	// A capsule; mandatory with one and one shape only per bone.
	CHUNK_STRUCTURE_BONE_SHAPE_BOX     = 'STSB',	// A box; mandatory with one and one shape only per bone.
	CHUNK_STRUCTURE_BONE_SHAPE_PORTAL  = 'STSP',	// A portal; mandatory with one and one shape only per bone.

	// File type: class. Used by both graphics and text applications.
	CHUNK_CLASS_INHERITANCE_LIST       = 'CLIL',	// Parent class information. List arranged after parent priority. Optional.
	CHUNK_CLASS_STRUCTURE              = 'CLST',	// An structure filename of the structure that forms the shapes of this class. Optional - absent when structure inherited or for abstract nouns ("clan" and "weather").
	CHUNK_CLASS_SETTINGS               = 'CLSE',	// String keys and their corresponding default string values. Example "stand_nimation":"heavy_walk". Optional.
	CHUNK_CLASS_MESH_LIST              = 'CLML',	// A list of mesh filenames that forms the looks of this class. Only used by graphics applications, and completly ignored by text applications. Optional - absent when using inherited or for abstract nouns.

	// File type: Group. Used by both graphics and text applications.
	CHUNK_GROUP_CLASS_LIST             = 'GCLI',	// A list of class filenames (each with a group-unique NAME) that forms the nouns of this group. Mandatory.
	CHUNK_GROUP_SETTINGS               = 'GRSE',	// A list of settings and attachments. A setting/an attachment contains two strings. Example: (groupNAME1.structure_NAME1) and (groupNAME2.structure_NAME2). May override class settings. Optional.

	// File type: world. Used by both graphics and text applications.
	CHUNK_WORLD_INFO                   = 'WINF',	// Global world data, such as pos+size. Mandatory.
	CHUNK_WORLD_QUAD_LIST              = 'WQUL',	// Just a recursive container for a world group list. Container for CHUNK_WORLD_GROUP_LIST. Mandatory.
	CHUNK_WORLD_GROUP_LIST             = 'WGRL',	// A list where each element is a group filename and a transformation. Each instance hangs on branches and leafs in the CHUNK_WORLD_QUAD_LIST. Mandatory.
};



class ChunkyLoader
{
public:
	// Used for both loading and saving. Hard-coded to some degree, but I actually find the code more
	// readable and flexible like this.
	class ChunkyFileElement
	{
	public:
		ChunkyFileElement();
		// Used for any non-32 bit, statically formed data. Allocates a byte array for each element loaded.
		// Field sizes are stored in pFieldSize[e], where e is the element index.
		ChunkyFileElement(ChunkyType pType, void** pPointer, unsigned* pFieldSize, unsigned pElementCount = 1);
		// Used for 32-bits (floats may use them too). Does not allocate. Stores total field size in *pFieldSize.
		ChunkyFileElement(ChunkyType pType, Lepra::int32* pInt, unsigned* pFieldSize = 0, unsigned pElementCount = 1);
		// Used for strings.
		ChunkyFileElement(ChunkyType pType, Lepra::String* pString, unsigned pElementCount = 1);
		// Used for receiving callbacks. Will call LoadElementCallback if this chunk type is found.
		ChunkyFileElement(ChunkyType pType, void* pPointer, unsigned pElementCount = 1);
		ChunkyFileElement(const ChunkyFileElement& pOriginal);
		void operator=(const ChunkyFileElement& pOriginal);

		ChunkyType mType;
		bool mLoadCallback;
		Lepra::int32* mIntPointer;
		void** mPointer;
		Lepra::String* mString;
		size_t* mFieldSize;
		unsigned mElementCount;

		bool mIsElementLoaded;
	};

	typedef std::list<ChunkyFileElement> FileElementList;

	ChunkyLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyLoader();

	//bool LoadSingleString(ChunkyType pType, Lepra::String& pString);
	//bool LoadStringList(std::list<Lepra::String>& pStringList);
	//bool LoadInt(ChunkyType pType, Lepra::int32& pInt);
	bool AllocLoadChunkyList(FileElementList& pLoadList);

	bool SaveSingleString(ChunkyType pType, const Lepra::String& pString);
	//bool SaveStringList(const std::list<Lepra::String>& pStringList, ChunkyType pType);
	bool SaveInt(ChunkyType pType, Lepra::int32 pInt);
	bool SaveChunkyList(const FileElementList& pSaveList);

protected:
	virtual bool LoadElementCallback(ChunkyType pType, size_t pSize, Lepra::int64 pChunkEndPosition, void* pStorage);

	bool VerifyFileType(ChunkyType pType);
	bool WriteFileType(ChunkyType pType);

	bool LoadHead(ChunkyType& pType, size_t& pSize, Lepra::int64& pChunkEndPosition);
	bool LoadRequiredHead(ChunkyType pRequiredType, size_t& pSize, Lepra::int64& pChunkEndPosition);
	bool SaveHead(ChunkyType pType, size_t pSize, Lepra::int64& pChunkEndPosition);

	Lepra::File* mFile;
	bool mIsFileOwner;
};



class ChunkyAnimationLoader: public ChunkyLoader	// For bone animations.
{
public:
	ChunkyAnimationLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyAnimationLoader();
	virtual bool Load(BoneAnimation* pAnimation);
	virtual bool Save(const BoneAnimation* pAnimation);

private:
	bool LoadElementCallback(TBC::ChunkyType pType, size_t pSize, Lepra::int64 pChunkEndPosition, void* pStorage);

	Lepra::int32 mKeyframeCount;
	Lepra::int32 mBoneCount;
	Lepra::int32 mCurrentKeyframe;
};

class ChunkyStructureLoader: public ChunkyLoader	// For physics and skinning. Loads the bone hierachy.
{
public:
	ChunkyStructureLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyStructureLoader();
	virtual bool Load(ChunkyStructure* pData);
	virtual bool Save(const ChunkyStructure* pData);

private:
	bool LoadElementCallback(TBC::ChunkyType pType, size_t pSize, Lepra::int64 pChunkEndPosition, void* pStorage);
	void SetBoneChildren(int pBoneIndex);

	Lepra::int32 mCurrentBoneIndex;
};

/*// Contans information on structure and animation names. Derived class handles UI: mesh, materials, sounds, etc.
class ChunkyClassLoader: public ChunkyLoader
{
public:
	ChunkyClassLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyClassLoader();
	virtual bool Load(ChunkyClass* pData);
	virtual bool Save(const ChunkyClass* pData);
};

// Contains class/group objects and links between them.
class ChunkyGroupLoader: public ChunkyLoader
{
public:
	ChunkyGroupLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyGroupLoader();
	virtual bool Load(ChunkyGroup* pData);
	virtual bool Save(const ChunkyGroup* pData);
};

class ChunkyWorldLoader: public ChunkyLoader	// Contains groups and terrain. A bit bold at this point? :)
{
public:
	ChunkyWorldLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyWorldLoader();
	virtual bool Load(ChunkyWorld* pData);
	virtual bool Save(const ChunkyWorld* pData);
};*/



}



#endif // !CHUNKYLOADER_H
