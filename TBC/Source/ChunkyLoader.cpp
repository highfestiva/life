
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games



#include "../Include/ChunkyLoader.h"
#include "../../Lepra/Include/Packer.h"
#include "../Include/Bones.h"
#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/ChunkyClass.h"
#include "../Include/ChunkyPhysics.h"
#include "../Include/PhysicsEngine.h"
#include "../Include/PhysicsSpawner.h"
#include "../Include/PhysicsTrigger.h"



namespace TBC
{



ChunkyLoader::ChunkyFileElement::ChunkyFileElement():
	mType((ChunkyType)0),
	mLoadCallback(false),
	mIntPointer(0),
	mPointer(0),
	mString(0),
	mFieldSize(0),
	mElementCount(0),
	mIsElementLoaded(false)
{
}

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType pType, void** pPointer, uint32* pFieldSize, int pElementCount):
	mType(pType),
	mLoadCallback(false),
	mIntPointer(0),
	mPointer(pPointer),
	mString(0),
	mFieldSize(pFieldSize),
	mElementCount(pElementCount),
	mIsElementLoaded(false)
{
}

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType pType, int32* pInt, uint32* pFieldSize, int pElementCount):
	mType(pType),
	mLoadCallback(false),
	mIntPointer(pInt),
	mPointer(0),
	mString(0),
	mFieldSize(pFieldSize),
	mElementCount(pElementCount),
	mIsElementLoaded(false)
{
}

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType pType, str* pString, int pElementCount):
	mType(pType),
	mLoadCallback(false),
	mIntPointer(0),
	mPointer(0),
	mString(pString),
	mFieldSize(0),
	mElementCount(pElementCount),
	mIsElementLoaded(false)
{
}

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType pType, void* pPointer, int pElementCount):
	mType(pType),
	mLoadCallback(true),
	mIntPointer(0),
	mPointer((void**)pPointer),
	mString(0),
	mFieldSize(0),
	mElementCount(pElementCount),
	mIsElementLoaded(false)
{
}

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(const ChunkyFileElement& pOriginal)
{
	*this = pOriginal;
}

void ChunkyLoader::ChunkyFileElement::operator=(const ChunkyFileElement& pOriginal)
{
	mType = pOriginal.mType;
	mLoadCallback = pOriginal.mLoadCallback;
	mIntPointer = pOriginal.mIntPointer;
	mPointer = pOriginal.mPointer;
	mString = pOriginal.mString;
	mFieldSize = pOriginal.mFieldSize;
	mElementCount = pOriginal.mElementCount;
	mIsElementLoaded = pOriginal.mIsElementLoaded;
}



ChunkyLoader::ChunkyLoader(File* pFile, bool pIsFileOwner):
	mFile(pFile),
	mIsFileOwner(pIsFileOwner)
{
}

ChunkyLoader::~ChunkyLoader()
{
	if (mIsFileOwner)
	{
		delete (mFile);
	}
	mFile = 0;
}



bool ChunkyLoader::AllocLoadChunkyList(FileElementList& pLoadList, int64 pChunkEnd)
{
	bool lLoadedAllElements = false;
	bool lOk = true;
	bool lAlreadyLoaded = false;
	int64 lHeadPosition = mFile->Tell();
	while (!lLoadedAllElements && lOk && mFile->Tell() < pChunkEnd)
	{
		ChunkyType lType = (ChunkyType)0;
		uint32 lSize = 0;
		int64 lChunkEndPosition = 0;

		// Load element head.
		if (lOk)
		{
			lHeadPosition = mFile->Tell();
			lOk = LoadHead(lType, lSize, lChunkEndPosition);
			assert(lOk);
#define C(t)	lType == t
			assert(C(CHUNK_MESH) || C(CHUNK_MESH_VERTICES) || C(CHUNK_MESH_NORMALS) || C(CHUNK_MESH_UV) ||
				C(CHUNK_MESH_COLOR) || C(CHUNK_MESH_COLOR_FORMAT) || C(CHUNK_MESH_TRIANGLES) || C(CHUNK_MESH_STRIPS) ||
				C(CHUNK_MESH_VOLATILITY) || C(CHUNK_MESH_CASTS_SHADOWS) ||
				C(CHUNK_SKIN) || C(CHUNK_SKIN_BONE_WEIGHT_GROUP) || C(CHUNK_SKIN_BWG_BONES) ||
				C(CHUNK_SKIN_BWG_VERTICES) || C(CHUNK_SKIN_BWG_WEIGHTS) || C(CHUNK_ANIMATION) || C(CHUNK_ANIMATION_MODE) ||
				C(CHUNK_ANIMATION_FRAME_COUNT) || C(CHUNK_ANIMATION_BONE_COUNT) || C(CHUNK_ANIMATION_USE_SPLINES) || C(CHUNK_ANIMATION_TIME) ||
				C(CHUNK_ANIMATION_ROOT_NODE) || C(CHUNK_ANIMATION_KEYFRAME) || C(CHUNK_ANIMATION_KEYFRAME_TIME) || C(CHUNK_ANIMATION_KEYFRAME_TRANSFORM) ||
				C(CHUNK_PHYSICS) || C(CHUNK_PHYSICS_BONE_COUNT) || C(CHUNK_PHYSICS_PHYSICS_TYPE) || C(CHUNK_PHYSICS_PHYSICS_GUIDE_MODE) ||
				C(CHUNK_PHYSICS_ENGINE_COUNT) || C(CHUNK_PHYSICS_TRIGGER_COUNT) || C(CHUNK_PHYSICS_SPAWNER_COUNT) ||
				C(CHUNK_PHYSICS_BONE_CONTAINER) || C(CHUNK_PHYSICS_BONE_CHILD_LIST) || C(CHUNK_PHYSICS_BONE_TRANSFORM) ||
				C(CHUNK_PHYSICS_BONE_SHAPE) || C(CHUNK_PHYSICS_ENGINE_CONTAINER) || C(CHUNK_PHYSICS_ENGINE) || C(CHUNK_PHYSICS_TRIGGER_CONTAINER) ||
				C(CHUNK_PHYSICS_TRIGGER) || C(CHUNK_PHYSICS_SPAWNER_CONTAINER) || C(CHUNK_PHYSICS_SPAWNER) || 
				C(CHUNK_CLASS) || C(CHUNK_CLASS_INHERITANCE_LIST) || C(CHUNK_CLASS_PHYSICS) ||
				C(CHUNK_CLASS_SETTINGS) || C(CHUNK_CLASS_MESH_LIST) || C(CHUNK_CLASS_PHYS_MESH) ||
				C(CHUNK_CLASS_TAG_LIST) || C(CHUNK_CLASS_TAG) || C(CHUNK_GROUP_CLASS_LIST) ||
				C(CHUNK_GROUP_SETTINGS) || C(CHUNK_WORLD_INFO) || C(CHUNK_WORLD_QUAD_LIST) || C(CHUNK_WORLD_GROUP_LIST));
		}

		// Load element contents, or skip it if the supplied list does not want it.
		bool lElementFound = false;
		if (lOk)
		{
			FileElementList::iterator x = pLoadList.begin();
			for (; lOk && !lElementFound && x != pLoadList.end(); ++x)
			{
				ChunkyFileElement& lElement = *x;
				if (lOk && lElement.mType == lType)
				{
					lElementFound = true;
					lAlreadyLoaded = lElement.mIsElementLoaded;
					lOk = !lAlreadyLoaded;
					if (lOk)
					{
						FileElementList::iterator y = x;
						// Check that elements appear in the same order as given in the load list, otherwise
						// we're looking at a new (optional) element in a chunk array.
						for (++y; lOk && y != pLoadList.end(); ++y)
						{
							const ChunkyFileElement& lElement = *y;
							lAlreadyLoaded = lElement.mIsElementLoaded;
							lOk = !lAlreadyLoaded;
						}
					}
					if (lOk)
					{
						tchar c[4];
						c[0] = (tchar)((lType>>24)&0x7F);
						c[1] = (tchar)((lType>>16)&0x7F);
						c[2] = (tchar)((lType>>8)&0x7F);
						c[3] = (tchar)((lType>>0)&0x7F);
						log_volatile(mLog.Tracef(_T("Loading chunk '%c%c%c%c'."),
							c[0], c[1], c[2], c[3]));
					}
					lElement.mIsElementLoaded = true;
					const int lMaxElementCount = ::abs(lElement.mElementCount);
					for (int y = 0; lOk && y < lMaxElementCount; ++y)
					{
						if (lElement.mLoadCallback)
						{
							lOk = LoadElementCallback(lType, lSize, lChunkEndPosition, lElement.mPointer);
							assert(lOk);
						}
						else if (lElement.mIntPointer)
						{
							lOk = (mFile->Read(lElement.mIntPointer[y]) == IO_OK);
							assert(lOk);
							if (lElement.mFieldSize)
							{
								*lElement.mFieldSize += sizeof(int32);
							}
						}
						else if (lElement.mPointer)
						{
							lOk = (mFile->AllocReadData(&lElement.mPointer[y], lSize) == IO_OK);
							assert(lOk);
							if (lOk)
							{
								lElement.mFieldSize[y] = lSize;
							}
						}
						else if (lElement.mString)
						{
							uint8* lString = 0;
							lOk = (lSize >= 4 && (lSize&1) == 0);
							assert(lOk);
							if (lOk)
							{
								lOk = (mFile->AllocReadData((void**)&lString, lSize) == IO_OK);
								assert(lOk);
							}
							if (lOk)
							{
								int lStringLength = PackerUnicodeString::Unpack(lElement.mString[y], lString, lSize);
								lOk = (lStringLength == (int)lSize || lStringLength == (int)lSize-2);
								assert(lOk);
								if (!lOk)
								{
									mLog.AError("Could not unpack string!");
								}
							}
							delete[] (lString);
						}

						if (mFile->Tell() >= lChunkEndPosition)
						{
							lOk = (y == lElement.mElementCount-1 || lElement.mElementCount <= 0);
							assert(lOk);
							if (!lOk)
							{
								mLog.Errorf(_T("Trying to load %i elements,")
									_T(" but only %i present. Format definition error?"),
									lElement.mElementCount, y+1);
							}
							break;
						}
					}
				}
			}
			if (lOk)
			{
				if (lElementFound)
				{
					lOk = (mFile->Tell() == lChunkEndPosition);
					assert(lOk);	// This probably means we put in too few elements in our load array when calling.
				}
				else
				{
					// Unknown element, try to load it separately.
					lOk = (mFile->Skip(lSize) == IO_OK);
					assert(lOk);
				}
			}
		}

		// Check if all desired elements have been loaded.
		if (lOk && lElementFound)
		{
			FileElementList::iterator x = pLoadList.begin();
			for (; x != pLoadList.end(); ++x)
			{
				ChunkyFileElement& lElement = *x;
				lLoadedAllElements = lElement.mIsElementLoaded;
				if (!lLoadedAllElements)
				{
					break;
				}
			}
		}
	}
	if (!lOk && lAlreadyLoaded)
	{
		lOk = (mFile->SeekSet(lHeadPosition) == lHeadPosition);
	}
	if (lOk)
	{
		lOk = (mFile->Tell() <= pChunkEnd);
	}
	assert(lOk);
	return (lOk);
}



bool ChunkyLoader::SaveSingleString(ChunkyType pType, const str& pString)
{
	const uint32 lSize = PackerUnicodeString::Pack(0, wstrutil::Encode(pString));	// Padding added.
	int64 lChunkEndPosition = 0;
	bool lOk = true;
	if (lOk)
	{
		lOk = SaveHead(pType, lSize, lChunkEndPosition);
	}
	if (lOk)
	{
		uint8* lData = new uint8[lSize];
		::memset(lData, 0, lSize);
		PackerUnicodeString::Pack(lData, wstrutil::Encode(pString));
		lOk = (mFile->WriteData(lData, lSize) == IO_OK);
		delete[] (lData);
	}
	if (lOk)
	{
		lOk = (mFile->Tell() == lChunkEndPosition);
	}
	return (lOk);
}

/*bool ChunkyLoader::SaveStringList(const std::list<str>& pStringList)
{
	// Compute size of all strings.
	size_t lSize = 0;
	std::list<str>::const_iterator x = pStringList.begin();
	for (; x != pStringList.end(); ++x)
	{
		const str& lString = *x;
		const int HEAD_SIZE = 4*2;
		lSize += HEAD_SIZE + ((lString.length()+1+3)&(~3));
	}

	int64 lChunkEndPosition = 0;
	bool lOk = true;
	if (lOk)
	{
		lOk = SaveHead(CHUNK_STRING_LIST, lSize, lChunkEndPosition);
	}
	x = pStringList.begin();
	for (; lOk && x != pStringList.end(); ++x)
	{
		const str& lString = *x;
		lOk = SaveSingleString(lString);
	}
	if (lOk)
	{
		lOk = (mFile->Tell() == lChunkEndPosition);
	}
	return (lOk);
}*/

bool ChunkyLoader::SaveInt(ChunkyType pType, int32 pInt)
{
	int64 lChunkEndPosition = 0;
	bool lOk = true;
	if (lOk)
	{
		lOk = SaveHead(pType, 4, lChunkEndPosition);
	}
	if (lOk)
	{
		lOk = (mFile->Write(pInt) == IO_OK);
	}
	if (lOk)
	{
		lOk = (mFile->Tell() == lChunkEndPosition);
	}
	return (lOk);
}

bool ChunkyLoader::SaveChunkyList(const FileElementList& pSaveList)
{
	bool lOk = true;
	FileElementList::const_iterator x = pSaveList.begin();
	for (; lOk && x != pSaveList.end(); ++x)
	{
		const ChunkyFileElement& lElement = *x;
		for (int y = 0; lOk && y < lElement.mElementCount; ++y)
		{
			int64 lChunkEndPosition = 0;
			if (lElement.mIntPointer)
			{
				lOk = SaveInt(lElement.mType, lElement.mIntPointer[y]);
			}
			else if (lElement.mPointer)
			{
				lOk = SaveHead(lElement.mType, lElement.mFieldSize[y], lChunkEndPosition);
				if (lOk)
				{
					lOk = (mFile->WriteData(lElement.mPointer[y], lElement.mFieldSize[y]) == IO_OK);
				}
			}
			else if (lElement.mString)
			{
				lOk = SaveSingleString(lElement.mType, lElement.mString[y]);
			}
		}
	}
	return (lOk);
}



bool ChunkyLoader::LoadElementCallback(ChunkyType, uint32, int64 pChunkEndPosition, void*)
{
	// Default behavior is to simply skip the chunk.
	bool lOk = (mFile->SeekSet(pChunkEndPosition) == pChunkEndPosition);
	return (lOk);
}



bool ChunkyLoader::VerifyFileType(ChunkyType pType)
{
	uint32 lSize = 0;
	int64 lChunkEndPosition = 0;
	bool lOk = LoadRequiredHead(pType, lSize, lChunkEndPosition);
	if (lOk && lChunkEndPosition != mFile->GetSize())
	{
		lOk = false;
	}
	return (lOk);
}

bool ChunkyLoader::WriteFileType(ChunkyType pType)
{
	uint32 lSize = 0;
	int64 lChunkEndPosition;
	bool lOk = SaveHead(pType, lSize, lChunkEndPosition);
	return (lOk);
}



bool ChunkyLoader::LoadHead(ChunkyType& pType, uint32& pSize, int64& pChunkEndPosition)
{
	bool lOk = true;
	int32 lTempType = 0;
	if (lOk)
	{
		lOk = (mFile->Read(lTempType) == IO_OK);
		pType = (ChunkyType)lTempType;
	}
	if (lOk)
	{
		uint32 lSize;
		lOk = (mFile->Read(lSize) == IO_OK);
		pSize = lSize;
	}
	if (lOk)
	{
		pChunkEndPosition = mFile->Tell()+((pSize+3)&(~3));
	}
	return (lOk);
}

bool ChunkyLoader::LoadRequiredHead(ChunkyType pType, uint32& pSize, int64& pChunkEndPosition)
{
	bool lOk = true;
	ChunkyType lFileType = (ChunkyType)0;
	if (lOk)
	{
		lOk = LoadHead(lFileType, pSize, pChunkEndPosition);
	}
	if (lOk)
	{
		lOk = (pType == lFileType);
	}
	return (lOk);
}

bool ChunkyLoader::SaveHead(ChunkyType pType, uint32 pSize, int64& pChunkEndPosition)
{
	bool lOk = true;
	if (lOk)
	{
		int32 lTempType = (int32)pType;
		lOk = (mFile->Write(lTempType) == IO_OK);
	}
	if (lOk)
	{
		uint32 lSize = (uint32)pSize;
		lOk = (mFile->Write(lSize) == IO_OK);
	}
	if (lOk)
	{
		pChunkEndPosition = mFile->Tell()+((pSize+3)&(~3));
	}
	return (lOk);
}

bool ChunkyLoader::RewriteChunkSize(int64 pChunkStartPosition)
{
	uint32 lSize = (uint32)(mFile->Tell() - pChunkStartPosition);
	mFile->SeekSet(pChunkStartPosition-4);
	bool lOk = (mFile->Write(lSize) == IO_OK);
	mFile->SeekEnd(0);
	return (lOk);
}

uint32* ChunkyLoader::AllocInitBigEndian(const float* pData, unsigned pCount)
{
	if (!pData || !pCount)
	{
		return (0);
	}
	uint32* lData = new uint32[pCount];
	for (unsigned x = 0; x < pCount; ++x)
	{
		lData[x] = Endian::HostToBigF(pData[x]);
	}
	return (lData);
}

uint32* ChunkyLoader::AllocInitBigEndian(const uint32* pData, unsigned pCount)
{
	if (!pData || !pCount)
	{
		return (0);
	}
	uint32* lData = new uint32[pCount];
	for (unsigned x = 0; x < pCount; ++x)
	{
		lData[x] = Endian::HostToBig(pData[x]);
	}
	return (lData);
}

uint32* ChunkyLoader::AllocInitBigEndian(const uint16* pData, unsigned pCount)
{
	if (!pData || !pCount)
	{
		return (0);
	}
	uint32* lData = new uint32[pCount];
	for (unsigned x = 0; x < pCount; ++x)
	{
		lData[x] = Endian::HostToBig(pData[x]);
	}
	return (lData);
}



ChunkyAnimationLoader::ChunkyAnimationLoader(File* pFile, bool pIsFileOwner):
	Parent(pFile, pIsFileOwner)
{
}

ChunkyAnimationLoader::~ChunkyAnimationLoader()
{
}

bool ChunkyAnimationLoader::Load(BoneAnimation* pAnimation)
{
	bool lOk = true;
	if (lOk)
	{
		lOk = VerifyFileType(CHUNK_ANIMATION);
	}

	int32 lDefaultMode = -1;
	mKeyframeCount = -1;	// Using member variable for callback l8r on.
	mBoneCount = -1;	// Using member variable for callback l8r on.
	int32 lUseSplines = -1;
	float lAnimationTimeLength = 0;
	str lRootNodeName;
	if (lOk)
	{
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_MODE, &lDefaultMode));
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_FRAME_COUNT, &mKeyframeCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_BONE_COUNT, &mBoneCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_USE_SPLINES, &lUseSplines));
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_TIME, (int32*)&lAnimationTimeLength));
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_ROOT_NODE, &lRootNodeName));
		lOk = AllocLoadChunkyList(lLoadList, mFile->GetSize());
	}
	if (lOk)
	{
		// Check that all mandatories have been found.
		lOk = (lDefaultMode != -1 && mKeyframeCount != -1 && mBoneCount != -1 && lUseSplines != -1 &&
			lAnimationTimeLength > 0 && lAnimationTimeLength < 1e4 && lRootNodeName.length() > 0);
	}
	if (lOk)
	{
		pAnimation->SetDefaultMode((BoneAnimation::Mode)lDefaultMode);
		pAnimation->SetKeyframeCount(mKeyframeCount, lUseSplines != 0);
		pAnimation->SetBoneCount(mBoneCount);
		pAnimation->SetTimeTag(mBoneCount, lAnimationTimeLength);
		pAnimation->SetRootNodeName(lRootNodeName);
	}
	if (lOk)
	{
		FileElementList lLoadList;
		mCurrentKeyframe = 0;
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_KEYFRAME, (void*)pAnimation, mKeyframeCount));
		lOk = AllocLoadChunkyList(lLoadList, mFile->GetSize());
	}
	return (lOk);
}

bool ChunkyAnimationLoader::Save(const BoneAnimation* pAnimation)
{
	// Write file header. We will come back to it later to re-write the actual size.
	bool lOk = true;
	if (lOk)
	{
		lOk = WriteFileType(CHUNK_ANIMATION);
	}
	int64 lFileDataStart = mFile->Tell();

	// Write animation settings and similar stuff.
	int32 lDefaultMode = pAnimation->GetDefaultMode();
	int32 lKeyframeCount = pAnimation->GetKeyframeCount();
	int32 lBoneCount = pAnimation->GetBoneCount();
	int32 lUseSplines = pAnimation->GetUseSplines();
	float lAnimationTimeLength = pAnimation->GetTimeTag(lKeyframeCount);
	str lRootNodeName = pAnimation->GetRootNodeName();
	if (lOk)
	{
		lOk = (lDefaultMode >= BoneAnimation::MODE_PLAY_ONCE && lKeyframeCount > 0 &&
			lBoneCount > 0 && lAnimationTimeLength > 0 && lRootNodeName.length() > 0);
	}
	if (lOk)
	{
		FileElementList lSaveList;
		lSaveList.push_back(ChunkyFileElement(CHUNK_ANIMATION_MODE, &lDefaultMode));
		lSaveList.push_back(ChunkyFileElement(CHUNK_ANIMATION_FRAME_COUNT, &lKeyframeCount));
		lSaveList.push_back(ChunkyFileElement(CHUNK_ANIMATION_BONE_COUNT, &lBoneCount));
		lSaveList.push_back(ChunkyFileElement(CHUNK_ANIMATION_USE_SPLINES, &lUseSplines));
		lSaveList.push_back(ChunkyFileElement(CHUNK_ANIMATION_TIME, (int32*)&lAnimationTimeLength));
		lSaveList.push_back(ChunkyFileElement(CHUNK_ANIMATION_ROOT_NODE, &lRootNodeName));
		lOk = SaveChunkyList(lSaveList);
	}

	// Write actual keyframes.
	const unsigned lTransformFloatCount = 3+4;
	if (lOk)
	{
		// Write all keyframes into a single chunk (multiple element chunk).
		const uint32 lFrameSize = (sizeof(uint32)*5 + lBoneCount*lTransformFloatCount*sizeof(float)) * lKeyframeCount;
		int64 lChunkEndPosition = 0;
		lOk = SaveHead(CHUNK_ANIMATION_KEYFRAME, lFrameSize, lChunkEndPosition);
	}
	if (lOk)
	{
		for (int kf = 0; lOk && kf < lKeyframeCount; ++kf)
		{
			// Save time as sub-chunk to the keyframe chunk.
			float lAnimationTime = pAnimation->GetTimeTag(kf);
			lOk = SaveInt(CHUNK_ANIMATION_KEYFRAME_TIME, *(int32*)&lAnimationTime);

			// Save transformations for all bones as sub-chunk to the keyframe chunk.
			const uint32 lTransformSize = lBoneCount*lTransformFloatCount*sizeof(float);
			int64 lChunkEndPosition = 0;
			lOk = SaveHead(CHUNK_ANIMATION_KEYFRAME_TRANSFORM, lTransformSize, lChunkEndPosition);
			for (int bc = 0; lOk && bc < lBoneCount; ++bc)
			{
				float lTransform[lTransformFloatCount];
				pAnimation->GetBoneTransformation(kf, bc).Get(lTransform);
				lOk = (mFile->WriteData(lTransform, sizeof(lTransform)) == IO_OK);
			}
		}
	}

	// Re-write file header size.
	if (lOk)
	{
		lOk = RewriteChunkSize(lFileDataStart);
	}

	return (lOk);
}



bool ChunkyAnimationLoader::LoadElementCallback(ChunkyType pType, uint32 pSize, int64 pChunkEndPosition, void* pStorage)
{
	bool lOk = false;
	if (pType == CHUNK_ANIMATION_KEYFRAME)
	{
		// Setup pointers and counters for list loading.
		float lKeyframeTime = 0;
		float* lTransformArray = 0;
		uint32 lFloatByteSize = 0;
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_KEYFRAME_TIME, (int32*)&lKeyframeTime));
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_KEYFRAME_TRANSFORM, (void**)&lTransformArray, &lFloatByteSize));
		lOk = AllocLoadChunkyList(lLoadList, pChunkEndPosition);

		const unsigned lTransformFloatCount = 3+4;
		if (lOk)
		{
			const unsigned lFloatCount = lTransformFloatCount*mBoneCount;
			lOk = (lFloatByteSize == lFloatCount*sizeof(float));
		}

		if (lOk)
		{
			BoneAnimation* lAnimation = (BoneAnimation*)pStorage;
			lAnimation->SetTimeTag(mCurrentKeyframe, lKeyframeTime);
			for (int lBone = 0; lBone < mBoneCount; ++lBone)
			{
				TransformationF lTransform(&lTransformArray[lBone*lTransformFloatCount]);
				lAnimation->SetBoneTransformation(mCurrentKeyframe, lBone, lTransform);
			}
			++mCurrentKeyframe;
		}

		delete[] (lTransformArray);
	}
	else
	{
		lOk = Parent::LoadElementCallback(pType, pSize, pChunkEndPosition, pStorage);
	}
	return (lOk);
}



ChunkyPhysicsLoader::ChunkyPhysicsLoader(File* pFile, bool pIsFileOwner):
	Parent(pFile, pIsFileOwner)
{
}

ChunkyPhysicsLoader::~ChunkyPhysicsLoader()
{
}

bool ChunkyPhysicsLoader::Load(ChunkyPhysics* pPhysics)
{
	bool lOk = true;
	if (lOk)
	{
		lOk = VerifyFileType(CHUNK_PHYSICS);
	}

	int32 lBoneCount = -1;
	int32 lPhysicsType = -1;
	int32 lGuideMode = 1;
	int32 lEngineCount = -1;
	int32 lTriggerCount = -1;
	int32 lSpawnerCount = -1;
	if (lOk)
	{
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_BONE_COUNT, &lBoneCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_PHYSICS_TYPE, &lPhysicsType));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_PHYSICS_GUIDE_MODE, &lGuideMode));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_ENGINE_COUNT, &lEngineCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_TRIGGER_COUNT, &lTriggerCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_SPAWNER_COUNT, &lSpawnerCount));
		lOk = AllocLoadChunkyList(lLoadList, mFile->GetSize());
		assert(lOk);
	}
	if (lOk)
	{
		// Check that all mandatories have been found.
		lOk = (lBoneCount >= 1 && lBoneCount < 10000 &&
			(lPhysicsType == ChunkyPhysics::STATIC || lPhysicsType == ChunkyPhysics::DYNAMIC) &&
			(lGuideMode >= ChunkyPhysics::GUIDE_NEVER && lGuideMode <= ChunkyPhysics::GUIDE_ALWAYS) &&
			lEngineCount >= 0 && lEngineCount < 1000 &&
			lTriggerCount >= 0 && lTriggerCount < 1000);
	}
	if (lOk)
	{
		pPhysics->SetBoneCount(lBoneCount);
		pPhysics->SetPhysicsType((ChunkyPhysics::PhysicsType)lPhysicsType);
		pPhysics->SetGuideMode((ChunkyPhysics::GuideMode)lGuideMode);

		FileElementList lLoadList;
		mCurrentBoneIndex = 0;
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_BONE_CONTAINER, (void*)pPhysics, lBoneCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_ENGINE_CONTAINER, (void*)pPhysics, lEngineCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_TRIGGER_CONTAINER, (void*)pPhysics, lTriggerCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_SPAWNER_CONTAINER, (void*)pPhysics, lSpawnerCount));
		lOk = AllocLoadChunkyList(lLoadList, mFile->GetSize());
		assert(lOk);
	}
	if (lOk)
	{
		lOk = (lEngineCount == pPhysics->GetEngineCount());
		assert(lOk);
	}
	if (lOk)
	{
		lOk = (lTriggerCount == pPhysics->GetTriggerCount());
		assert(lOk);
	}

	return (lOk);
}

bool ChunkyPhysicsLoader::Save(const ChunkyPhysics* pPhysics)
{
	bool lOk = (pPhysics->GetBoneCount() > 0);

	// Write file header. We will come back to it later to re-write the actual size.
	if (lOk)
	{
		lOk = WriteFileType(CHUNK_PHYSICS);
	}
	const int64 lFileDataStart = mFile->Tell();

	// Write physics modes and similar stuff.
	int32 lBoneCount = pPhysics->GetBoneCount();
	int32 lPhysicsType = pPhysics->GetPhysicsType();
	int32 lEngineCount = pPhysics->GetEngineCount();
	int32 lTriggerCount = pPhysics->GetTriggerCount();
	int32 lSpawnerCount = pPhysics->GetSpawnerCount();
	if (lOk)
	{
		lOk = (lBoneCount > 0 && lBoneCount < 10000 &&
			lPhysicsType >= ChunkyPhysics::STATIC &&
			lEngineCount >= 0 && lEngineCount < 1000 &&
			lTriggerCount >= 0 && lTriggerCount < 1000);
	}
	if (lOk)
	{
		FileElementList lSaveList;
		lSaveList.push_back(ChunkyFileElement(CHUNK_PHYSICS_BONE_COUNT, &lBoneCount));
		lSaveList.push_back(ChunkyFileElement(CHUNK_PHYSICS_PHYSICS_TYPE, &lPhysicsType));
		lSaveList.push_back(ChunkyFileElement(CHUNK_PHYSICS_ENGINE_COUNT, &lEngineCount));
		lSaveList.push_back(ChunkyFileElement(CHUNK_PHYSICS_TRIGGER_COUNT, &lTriggerCount));
		lSaveList.push_back(ChunkyFileElement(CHUNK_PHYSICS_SPAWNER_COUNT, &lSpawnerCount));
		lOk = SaveChunkyList(lSaveList);
	}

	// Write actual bone positioning and geometries.
	if (lOk)
	{
		// Write all bone stuff into a single chunk (multiple element chunk).
		int64 lChunkEndPosition = 0;
		lOk = SaveHead(CHUNK_PHYSICS_BONE_CONTAINER, 0, lChunkEndPosition);
	}
	const int64 lBoneChunkStart = mFile->Tell();
	for (int b = 0; lOk && b < lBoneCount; ++b)
	{
		// Save children.
		const int cc = pPhysics->GetBoneChildCount(b);
		if (cc > 0)
		{
			int64 lChunkEndPosition = 0;
			lOk = SaveHead(CHUNK_PHYSICS_BONE_CHILD_LIST, cc*sizeof(int32), lChunkEndPosition);
			for (int c = 0; lOk && c < cc; ++c)
			{
				int32 lChildIndex = pPhysics->GetChildIndex(b, c);
				mFile->Write(lChildIndex);
			}
		}

		// Save bone's original transformation.
		if (lOk)
		{
			const unsigned lTransformFloatCount = 3+4;
			const uint32 lTransformSize = lTransformFloatCount*sizeof(float);
			int64 lChunkEndPosition = 0;
			lOk = SaveHead(CHUNK_PHYSICS_BONE_TRANSFORM, lTransformSize, lChunkEndPosition);
			uint32 lTransform[lTransformFloatCount];
			pPhysics->GetBoneTransformation(b).Get((float*)lTransform);
			// Convert to network byte order.
			for (unsigned x = 0; x < lTransformFloatCount; ++x)
			{
				lTransform[x] = Endian::HostToBig(lTransform[x]);
			}
			lOk = (mFile->WriteData(lTransform, sizeof(lTransform)) == IO_OK);
		}

		// Save geometries.
		if (lOk)
		{
			ChunkyBoneGeometry* lGeometry = pPhysics->GetBoneGeometry(b);
			if (lGeometry)
			{
				int64 lChunkEndPosition = 0;
				unsigned lSize = lGeometry->GetChunkySize();
				lOk = SaveHead(CHUNK_PHYSICS_BONE_SHAPE, lSize, lChunkEndPosition);
				if (lOk)
				{
					char* lData = new char[lSize];
					lGeometry->SaveChunkyData(pPhysics, lData);
					assert(lGeometry->GetChunkySize(lData) == lSize);
					lOk = (mFile->WriteData(lData, lSize) == IO_OK);
					delete (lData);
				}
			}
		}
	}
	if (lOk)
	{
		lOk = RewriteChunkSize(lBoneChunkStart);
	}

	// Write engine header.
	if (lOk)
	{
		// Write all bone stuff into a single chunk (multiple element chunk).
		int64 lChunkEndPosition = 0;
		lOk = SaveHead(CHUNK_PHYSICS_ENGINE_CONTAINER, 0, lChunkEndPosition);
	}
	const int64 lEngineChunkStart = mFile->Tell();
	// Write engines.
	for (int e = 0; lOk && e < lEngineCount; ++e)
	{
		PhysicsEngine* lEngine = pPhysics->GetEngine(e);
		assert(lEngine);
		int64 lChunkEndPosition = 0;
		unsigned lSize = lEngine->GetChunkySize();
		lOk = SaveHead(CHUNK_PHYSICS_ENGINE, lSize, lChunkEndPosition);
		if (lOk)
		{
			char* lData = new char[lSize];
			lEngine->SaveChunkyData(pPhysics, lData);
			lOk = (mFile->WriteData(lData, lSize) == IO_OK);
			delete (lData);
		}
	}
	if (lOk)
	{
		lOk = RewriteChunkSize(lEngineChunkStart);
	}

	// Write trigger header.
	if (lOk)
	{
		// Write all bone stuff into a single chunk (multiple element chunk).
		int64 lChunkEndPosition = 0;
		lOk = SaveHead(CHUNK_PHYSICS_TRIGGER_CONTAINER, 0, lChunkEndPosition);
	}
	const int64 lTriggerChunkStart = mFile->Tell();
	// Write triggers.
	for (int t = 0; lOk && t < lTriggerCount; ++t)
	{
		const PhysicsTrigger* lTrigger = pPhysics->GetTrigger(t);
		assert(lTrigger);
		int64 lChunkEndPosition = 0;
		unsigned lSize = lTrigger->GetChunkySize();
		lOk = SaveHead(CHUNK_PHYSICS_TRIGGER, lSize, lChunkEndPosition);
		if (lOk)
		{
			char* lData = new char[lSize];
			lTrigger->SaveChunkyData(pPhysics, lData);
			lOk = (mFile->WriteData(lData, lSize) == IO_OK);
			delete (lData);
		}
	}
	if (lOk)
	{
		lOk = RewriteChunkSize(lTriggerChunkStart);
	}

	// Re-write file header size.
	if (lOk)
	{
		lOk = RewriteChunkSize(lFileDataStart);
	}

	return (lOk);
}

bool ChunkyPhysicsLoader::LoadElementCallback(ChunkyType pType, uint32 pSize, int64 pChunkEndPosition, void* pStorage)
{
	ChunkyPhysics* lPhysics = (ChunkyPhysics*)pStorage;
	bool lOk = false;
	if (pType == CHUNK_PHYSICS_BONE_CONTAINER)
	{
		// Setup pointers and counters for list loading.
		const int MAXIMUM_CHILD_BONES = 256;
		int32 lChildArray[MAXIMUM_CHILD_BONES];
		::memset(lChildArray, -1, sizeof(lChildArray));
		unsigned lChildByteSize = 0;
		uint32* lTransformArray = 0;
		unsigned lFloatByteSize = 0;
		uint32* lGeometryArray = 0;
		unsigned lGeometryByteSize = 0;
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_BONE_CHILD_LIST, lChildArray, &lChildByteSize, -MAXIMUM_CHILD_BONES));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_BONE_TRANSFORM, (void**)&lTransformArray, (unsigned*)&lFloatByteSize));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_BONE_SHAPE, (void**)&lGeometryArray, (unsigned*)&lGeometryByteSize));
		lOk = AllocLoadChunkyList(lLoadList, pChunkEndPosition);

		// Check child array byte size.
		int lChildCount = 0;
		if (lOk)
		{
			lOk = ((lChildByteSize&3) == 0);
			lChildCount = lChildByteSize/sizeof(int32);
		}
		// Check bone transformation byte size.
		const unsigned lTransformFloatCount = 3+4;
		if (lOk)
		{
			const unsigned lFloatCount = lTransformFloatCount;
			lOk = (lFloatByteSize == lFloatCount*sizeof(float));
		}

		if (lOk)
		{
			log_volatile(mLog.Tracef(_T("Current bone index is %i."),
				mCurrentBoneIndex));
			lPhysics->SetBoneChildCount(mCurrentBoneIndex, lChildCount);
			for (int x = 0; lOk && x < lChildCount; ++x)
			{
				lOk = (lChildArray[x] >= 0 && lChildArray[x] < lPhysics->GetBoneCount());
				if (lOk)
				{
					lPhysics->SetChildIndex(mCurrentBoneIndex, x, lChildArray[x]);
				}
			}
			// Convert to host endian.
			for (unsigned x = 0; x < lTransformFloatCount; ++x)
			{
				lTransformArray[x] = Endian::BigToHost(lTransformArray[x]);
			}
			TransformationF lTransform((const float*)lTransformArray);
			lPhysics->SetOriginalBoneTransformation(mCurrentBoneIndex, lTransform);
		}

		if (lOk && lGeometryArray)
		{
			ChunkyBoneGeometry* lGeometry = ChunkyBoneGeometry::Load(lPhysics,
				lGeometryArray, lGeometryByteSize);
			lOk = (lGeometry != 0);
			if (lOk)
			{
				lPhysics->AddBoneGeometry(lGeometry);
			}
		}

		assert(lOk);

		delete[] (lGeometryArray);
		delete[] (lTransformArray);
		++mCurrentBoneIndex;
	}
	else if (pType == CHUNK_PHYSICS_ENGINE_CONTAINER)
	{
		uint32* lEngineArray = 0;
		unsigned lEngineByteSize = 0;
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_ENGINE, (void**)&lEngineArray, (unsigned*)&lEngineByteSize));
		lOk = AllocLoadChunkyList(lLoadList, pChunkEndPosition);

		PhysicsEngine* lEngine = 0;
		if (lOk)
		{
			lEngine = PhysicsEngine::Load(lPhysics, lEngineArray, lEngineByteSize);
			lOk = (lEngine != 0);
		}
		if (lOk)
		{
			lPhysics->AddEngine(lEngine);
		}

		delete[] (lEngineArray);
	}
	else if (pType == CHUNK_PHYSICS_TRIGGER_CONTAINER)
	{
		uint32* lTriggerArray = 0;
		unsigned lTriggerByteSize = 0;
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_TRIGGER, (void**)&lTriggerArray, (unsigned*)&lTriggerByteSize));
		lOk = AllocLoadChunkyList(lLoadList, pChunkEndPosition);

		PhysicsTrigger* lTrigger = 0;
		if (lOk)
		{
			lTrigger = PhysicsTrigger::Load(lPhysics, lTriggerArray, lTriggerByteSize);
			lOk = (lTrigger != 0);
			assert(lOk);
		}
		if (lOk)
		{
			lPhysics->AddTrigger(lTrigger);
		}

		delete[] (lTriggerArray);
	}
	else if (pType == CHUNK_PHYSICS_SPAWNER_CONTAINER)
	{
		uint32* lSpawnerArray = 0;
		unsigned lSpawnerByteSize = 0;
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_SPAWNER, (void**)&lSpawnerArray, (unsigned*)&lSpawnerByteSize));
		lOk = AllocLoadChunkyList(lLoadList, pChunkEndPosition);

		PhysicsSpawner* lSpawner = 0;
		if (lOk)
		{
			lSpawner = PhysicsSpawner::Load(lPhysics, lSpawnerArray, lSpawnerByteSize);
			lOk = (lSpawner != 0);
			assert(lOk);
		}
		if (lOk)
		{
			lPhysics->AddSpawner(lSpawner);
		}

		delete[] (lSpawnerArray);
	}
	else
	{
		lOk = Parent::LoadElementCallback(pType, pSize, pChunkEndPosition, pStorage);
	}
	assert(lOk);
	return (lOk);
}



ChunkyClassLoader::ChunkyClassLoader(File* pFile, bool pIsFileOwner):
	Parent(pFile, pIsFileOwner)
{
}

ChunkyClassLoader::~ChunkyClassLoader()
{
}

bool ChunkyClassLoader::Load(ChunkyClass* pData)
{
	bool lOk = true;
	if (lOk)
	{
		lOk = VerifyFileType(CHUNK_CLASS);
	}

	if (lOk)
	{
		FileElementList lLoadList;
		AddLoadElements(lLoadList, pData);
		lOk = AllocLoadChunkyList(lLoadList, mFile->GetSize());
	}
	if (lOk)
	{
		lOk = (!pData->GetPhysicsBaseName().empty() &&
			true);	// TODO: check other tags (e.g. settings).
		if (!lOk)
		{
			mLog.Errorf(_T("Could not load contents of class file %s!"), pData->GetPhysicsBaseName().c_str());
		}
	}

	return (lOk);
}

void ChunkyClassLoader::AddLoadElements(FileElementList& pElementList, ChunkyClass* pData)
{
	pElementList.push_back(ChunkyFileElement(CHUNK_CLASS_PHYSICS, &pData->GetPhysicsBaseName()));
	pElementList.push_back(ChunkyFileElement(CHUNK_CLASS_TAG_LIST, (void*)pData, -1000));
}

bool ChunkyClassLoader::LoadElementCallback(ChunkyType pType, uint32 pSize, int64 pChunkEndPosition, void* pStorage)
{
	ChunkyClass* lClass = (ChunkyClass*)pStorage;
	bool lOk = false;
	if (pType == CHUNK_CLASS_TAG_LIST)
	{
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_CLASS_TAG, (void*)lClass));
		lOk = AllocLoadChunkyList(lLoadList, pChunkEndPosition);
		assert(lOk);
	}
	else if (pType == CHUNK_CLASS_TAG)
	{
		uint8* lBuffer = 0;
		lOk = (mFile->AllocReadData((void**)&lBuffer, pSize) == IO_OK);
		assert(lOk);
		if (lOk)
		{
			lOk = lClass->UnpackTag(lBuffer, pSize);
			assert(lOk);
		}
		delete[] (lBuffer);
	}
	else
	{
		lOk = Parent::LoadElementCallback(pType, pSize, pChunkEndPosition, pStorage);
		assert(lOk);
	}
	return (lOk);
}



LOG_CLASS_DEFINE(GENERAL_RESOURCES, ChunkyLoader);
LOG_CLASS_DEFINE(GENERAL_RESOURCES, ChunkyAnimationLoader);
LOG_CLASS_DEFINE(GENERAL_RESOURCES, ChunkyPhysicsLoader);
LOG_CLASS_DEFINE(GENERAL_RESOURCES, ChunkyClassLoader);



}
