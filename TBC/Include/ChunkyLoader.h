
// Author: Jonas Bystr�m
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
class ChunkyPhysics;
class ChunkyWorld;


#define ENUMROT(c, bits)	(((int)c)<<bits)
#define ENUMIFY(a, b, c, d)	ENUMROT(a, 24) + ENUMROT(b, 16) + ENUMROT(c, 8) +  ENUMROT(d, 0)

enum ChunkyType
{
	// File type: mesh. Unly used by graphics applications.
	CHUNK_MESH                         = ENUMIFY('M','E','S','H'),	// Mesh file type.
	CHUNK_MESH_VERTICES                = ENUMIFY('M','E','V','X'),	// Mandatory.
	CHUNK_MESH_NORMALS                 = ENUMIFY('M','E','N','O'),	// Mandatory.
	CHUNK_MESH_UV                      = ENUMIFY('M','E','U','V'),	// Optional.
	CHUNK_MESH_COLOR                   = ENUMIFY('M','E','C','O'),	// Optional.
	CHUNK_MESH_COLOR_FORMAT            = ENUMIFY('M','E','C','F'),	// Optional.
	CHUNK_MESH_TRIANGLES               = ENUMIFY('M','E','T','R'),	// A mesh can contain EITHER this type OR the strips type.
	CHUNK_MESH_STRIPS                  = ENUMIFY('M','E','S','T'),	// A mesh can contain EITHER this type OR the triangles type.
	CHUNK_MESH_VOLATILITY              = ENUMIFY('M','E','V','O'),	// Optional.

	// File type: skin. Unly used by graphics applications.
	CHUNK_SKIN                         = ENUMIFY('S','K','I','N'),	// Skin file type.
	// Skin sub element: bone weight group. Used by skinning to determine bone-weight-per-vertex.
	CHUNK_SKIN_BONE_WEIGHT_GROUP       = ENUMIFY('S','K','W','G'),	// Optional.
	CHUNK_SKIN_BWG_BONES               = ENUMIFY('S','K','W','B'),	// Sub-object is mandatory if weight group present.
	CHUNK_SKIN_BWG_VERTICES            = ENUMIFY('S','K','W','V'),	// Sub-object is mandatory if weight group present.
	CHUNK_SKIN_BWG_WEIGHTS             = ENUMIFY('S','K','W','W'),	// Sub-object is mandatory if weight group present.

	// File type: animation. Used by both graphics and text applications.
	CHUNK_ANIMATION                    = ENUMIFY('A','N','I','M'),	// Animation file type.
	CHUNK_ANIMATION_MODE               = ENUMIFY('A','N','M','O'),	// "Looped" or "play once". Mandatory.
	CHUNK_ANIMATION_FRAME_COUNT        = ENUMIFY('A','N','F','C'),	// Number of keyframes. Mandatory.
	CHUNK_ANIMATION_BONE_COUNT         = ENUMIFY('A','N','B','C'),	// Number of bones. Mandatory.
	CHUNK_ANIMATION_USE_SPLINES        = ENUMIFY('A','N','U','S'),	// Hehe, love the abbreviation... Mandatory.
	CHUNK_ANIMATION_TIME               = ENUMIFY('A','N','T','I'),	// Animation length in seconds. Mandatory.
	CHUNK_ANIMATION_ROOT_NODE          = ENUMIFY('A','N','R','N'),	// A single NAME that corresponds to the start node in the physics hierarchy. Mandatory.
	// Animation sub element: keyframe.
	CHUNK_ANIMATION_KEYFRAME           = ENUMIFY('A','N','K','A'),	// A keyframe and its transformations. Means "duck" in swedish. Mandatory.
	CHUNK_ANIMATION_KEYFRAME_TIME      = ENUMIFY('A','N','T','M'),	// Time for this keyframe. Mandatory.
	CHUNK_ANIMATION_KEYFRAME_TRANSFORM = ENUMIFY('A','N','K','T'),	// Transformations for this keyframe for all bones in one BLOB. Mandatory.

	// File type: physics. Used by both graphics and text applications.
	CHUNK_PHYSICS                      = ENUMIFY('P','H','Y','S'),	// Physics file type.
	CHUNK_PHYSICS_BONE_COUNT           = ENUMIFY('P','H','B','C'),	// Number of bones. Mandatory.
	CHUNK_PHYSICS_PHYSICS_TYPE         = ENUMIFY('P','H','P','T'),	// Physics type: dynamic, static, etc. Mandatory.
	CHUNK_PHYSICS_ENGINE_COUNT         = ENUMIFY('P','H','E','C'),	// Number of engines. Mandatory.
	// Physics sub element: bone.
	CHUNK_PHYSICS_BONE_CONTAINER       = ENUMIFY('P','H','B','O'),	// A bone and its transformation, type. Mandatory.
	CHUNK_PHYSICS_BONE_CHILD_LIST      = ENUMIFY('S','B','C','L'),	// The indices of this bone's children. Optional for bones without children, mandatory otherwise.
	CHUNK_PHYSICS_BONE_TRANSFORM       = ENUMIFY('P','H','B','T'),	// The transform of this bone. Mandatory.
	CHUNK_PHYSICS_BONE_SHAPE           = ENUMIFY('P','H','S','H'),	// Bone shape (i.e. capsule, box, sphere, portal...), mandatory with one and one shape only per bone.
	// Physics sub element: engine.
	CHUNK_PHYSICS_ENGINE_CONTAINER     = ENUMIFY('P','H','E','O'),	// Engine array. Mandatory.
	CHUNK_PHYSICS_ENGINE               = ENUMIFY('P','H','E','N'),	// An engine and its parameters, type. Mandatory.

	// File type: class. Used by both graphics and text applications.
	CHUNK_CLASS                        = ENUMIFY('C','L','A','S'),	// Class file type.
	CHUNK_CLASS_INHERITANCE_LIST       = ENUMIFY('C','L','I','L'),	// Parent class information. List arranged after parent priority. Optional.
	CHUNK_CLASS_PHYSICS                = ENUMIFY('C','L','P','H'),	// An physics filename of the physics that forms the shapes of this class. Optional - absent when physics inherited or for abstract nouns ("clan" and "weather").
	CHUNK_CLASS_SETTINGS               = ENUMIFY('C','L','S','E'),	// String keys and their corresponding default string values. Example "stand_animation":"heavy_walk". Optional.
	CHUNK_CLASS_MESH_LIST              = ENUMIFY('C','L','M','L'),	// A list of mesh filenames that forms the looks of this class. Only used by graphics applications, and completly ignored by text applications. Optional - absent when using inherited or for abstract nouns.
	CHUNK_CLASS_PHYS_MESH              = ENUMIFY('C','L','P','M'),	// Connection between a bone and a mesh. Mandatory in sub-chunk array.

	// File type: Group. Used by both graphics and text applications.
	CHUNK_GROUP_CLASS_LIST             = ENUMIFY('G','C','L','I'),	// A list of class filenames (each with a group-unique NAME) that forms the nouns of this group. Mandatory.
	CHUNK_GROUP_SETTINGS               = ENUMIFY('G','R','S','E'),	// A list of settings and attachments. A setting/an attachment contains two strings. Example: (groupNAME1.physics_NAME1) and (groupNAME2.physics_NAME2). May override class settings. Optional.

	// File type: world. Used by both graphics and text applications.
	CHUNK_WORLD_INFO                   = ENUMIFY('W','I','N','F'),	// Global world data, such as pos+size. Mandatory.
	CHUNK_WORLD_QUAD_LIST              = ENUMIFY('W','Q','U','L'),	// Just a recursive container for a world group list. Container for CHUNK_WORLD_GROUP_LIST. Mandatory.
	CHUNK_WORLD_GROUP_LIST             = ENUMIFY('W','G','R','L'),	// A list where each element is a group filename and a transformation. Each instance hangs on branches and leafs in the CHUNK_WORLD_QUAD_LIST. Mandatory.
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
		ChunkyFileElement(ChunkyType pType, void** pPointer, Lepra::uint32* pFieldSize, int pElementCount = 1);
		// Used for 32-bits (floats may use them too). Does not allocate. Stores total field size in *pFieldSize.
		ChunkyFileElement(ChunkyType pType, Lepra::int32* pInt, Lepra::uint32* pFieldSize = 0, int pElementCount = 1);
		// Used for strings.
		ChunkyFileElement(ChunkyType pType, Lepra::String* pString, int pElementCount = 1);
		// Used for receiving callbacks. Will call LoadElementCallback if this chunk type is found.
		ChunkyFileElement(ChunkyType pType, void* pPointer, int pElementCount = 1);
		ChunkyFileElement(const ChunkyFileElement& pOriginal);
		void operator=(const ChunkyFileElement& pOriginal);

		ChunkyType mType;
		bool mLoadCallback;
		Lepra::int32* mIntPointer;
		void** mPointer;
		Lepra::String* mString;
		Lepra::uint32* mFieldSize;
		int mElementCount;

		bool mIsElementLoaded;
	};

	typedef std::list<ChunkyFileElement> FileElementList;

	ChunkyLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyLoader();

	//bool LoadSingleString(ChunkyType pType, Lepra::String& pString);
	//bool LoadStringList(std::list<Lepra::String>& pStringList);
	//bool LoadInt(ChunkyType pType, Lepra::int32& pInt);
	bool AllocLoadChunkyList(FileElementList& pLoadList, Lepra::int64 pChunkEnd);

	bool SaveSingleString(ChunkyType pType, const Lepra::String& pString);
	//bool SaveStringList(const std::list<Lepra::String>& pStringList, ChunkyType pType);
	bool SaveInt(ChunkyType pType, Lepra::int32 pInt);
	bool SaveChunkyList(const FileElementList& pSaveList);

protected:
	virtual bool LoadElementCallback(ChunkyType pType, Lepra::uint32 pSize, Lepra::int64 pChunkEndPosition, void* pStorage);

	bool VerifyFileType(ChunkyType pType);
	bool WriteFileType(ChunkyType pType);

	bool LoadHead(ChunkyType& pType, Lepra::uint32& pSize, Lepra::int64& pChunkEndPosition);
	bool LoadRequiredHead(ChunkyType pRequiredType, Lepra::uint32& pSize, Lepra::int64& pChunkEndPosition);
	bool SaveHead(ChunkyType pType, Lepra::uint32 pSize, Lepra::int64& pChunkEndPosition);
	bool RewriteChunkSize(Lepra::int64 pChunkStartPosition);

	Lepra::uint32* AllocInitBigEndian(const float* pData, unsigned pCount);
	Lepra::uint32* AllocInitBigEndian(const Lepra::uint32* pData, unsigned pCount);

	Lepra::File* mFile;
	bool mIsFileOwner;

	LOG_CLASS_DECLARE();
};



class ChunkyAnimationLoader: public ChunkyLoader	// For bone animations.
{
	typedef ChunkyLoader Parent;
public:
	ChunkyAnimationLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyAnimationLoader();
	virtual bool Load(BoneAnimation* pAnimation);
	virtual bool Save(const BoneAnimation* pAnimation);

private:
	bool LoadElementCallback(TBC::ChunkyType pType, Lepra::uint32 pSize, Lepra::int64 pChunkEndPosition, void* pStorage);

	Lepra::int32 mKeyframeCount;
	Lepra::int32 mBoneCount;
	Lepra::int32 mCurrentKeyframe;

	LOG_CLASS_DECLARE();
};

class ChunkyPhysicsLoader: public ChunkyLoader	// For physics and skinning. Loads the bone hierachy.
{
	typedef ChunkyLoader Parent;
public:
	ChunkyPhysicsLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyPhysicsLoader();
	virtual bool Load(ChunkyPhysics* pData);
	virtual bool Save(const ChunkyPhysics* pData);

private:
	bool LoadElementCallback(TBC::ChunkyType pType, Lepra::uint32 pSize, Lepra::int64 pChunkEndPosition, void* pStorage);
	void SetBoneChildren(int pBoneIndex);

	Lepra::int32 mCurrentBoneIndex;

	LOG_CLASS_DECLARE();
};

// Contans information on physics and animation names. Derived class handles UI: mesh, materials, sounds, etc.
class ChunkyClassLoader: public ChunkyLoader
{
	typedef ChunkyLoader Parent;
public:
	ChunkyClassLoader(Lepra::File* pFile, bool pIsFileOwner);
	virtual ~ChunkyClassLoader();
	virtual bool Load(ChunkyClass* pData);

protected:
	virtual void AddLoadElements(FileElementList& pElementList, ChunkyClass* pData);

private:
	LOG_CLASS_DECLARE();
};

/*// Contains class/group objects and links between them.
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
