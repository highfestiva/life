
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/Bones.h"
#include "../Include/ChunkyLoader.h"
#include "../Include/ChunkyStructure.h"



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

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType pType, void** pPointer, unsigned* pFieldSize, unsigned pElementCount):
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

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType pType, Lepra::int32* pInt, unsigned* pFieldSize, unsigned pElementCount):
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

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType pType, Lepra::String* pString, unsigned pElementCount):
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

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType pType, void* pPointer, unsigned pElementCount):
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



ChunkyLoader::ChunkyLoader(Lepra::File* pFile, bool pIsFileOwner):
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



bool ChunkyLoader::AllocLoadChunkyList(FileElementList& pLoadList)
{
	bool lLoadedAllElements = false;
	bool lOk = true;
	while (!lLoadedAllElements && lOk && mFile->Tell() < mFile->GetSize())
	{
		ChunkyType lType = (ChunkyType)0;
		size_t lSize = 0;
		Lepra::int64 lChunkEndPosition = 0;

		// Load element head.
		if (lOk)
		{
			lOk = LoadHead(lType, lSize, lChunkEndPosition);
		}

		// Load element contents, or skip it if the supplied list does not want it.
		bool lElementFound = false;
		if (lOk)
		{
			FileElementList::iterator x = pLoadList.begin();
			for (; lOk && !lElementFound && x != pLoadList.end(); ++x)
			{
				ChunkyFileElement& lElement = *x;
				if (lElement.mType == lType)
				{
					lElementFound = true;
					lOk = (!lElement.mIsElementLoaded);
					lElement.mIsElementLoaded = true;
					for (unsigned y = 0; lOk && y < lElement.mElementCount; ++y)
					{
						if (lElement.mLoadCallback)
						{
							lOk = LoadElementCallback(lType, lSize, lChunkEndPosition, lElement.mPointer);
						}
						else if (lElement.mIntPointer)
						{
							lOk = (mFile->Read(lElement.mIntPointer[y]) == Lepra::IO_OK);
							if (lElement.mFieldSize)
							{
								*lElement.mFieldSize += sizeof(Lepra::int32);
							}
						}
						else if (lElement.mPointer)
						{
							lOk = (mFile->AllocReadData(&lElement.mPointer[y], lSize) == Lepra::IO_OK);
							if (lOk)
							{
								lElement.mFieldSize[y] = lSize;
							}
						}
						else if (lElement.mString)
						{
							char* lString = 0;
							lOk = (mFile->AllocReadData((void**)&lString, lSize) == Lepra::IO_OK);
							if (lOk)
							{
								Lepra::AnsiString lAnsiString;
								lAnsiString.assign(lString, lSize);
								lElement.mString[y] = Lepra::AnsiStringUtility::ToCurrentCode(lAnsiString);
							}
							delete[] (lString);
						}

						if (mFile->Tell() >= lChunkEndPosition)
						{
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
				}
				else
				{
					// Unknown element, try to load it separately.
					lOk = (mFile->Skip(lSize) == Lepra::IO_OK);
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
	return (lOk);
}



bool ChunkyLoader::SaveSingleString(ChunkyType pType, const Lepra::String& pString)
{
	// Write padding at once. No CPU safety border (a.k.a. page boundary) crosses the 4-byte alignment boundary.
	const size_t lSize = ((pString.length()+1+3)&(~3));
	Lepra::int64 lChunkEndPosition = 0;
	bool lOk = true;
	if (lOk)
	{
		lOk = SaveHead(pType, lSize, lChunkEndPosition);
	}
	if (lOk)
	{
		lOk = (mFile->WriteData(Lepra::AnsiStringUtility::ToOwnCode(pString).c_str(), lSize) == Lepra::IO_OK);
	}
	if (lOk)
	{
		lOk = (mFile->Tell() == lChunkEndPosition);
	}
	return (lOk);
}

/*bool ChunkyLoader::SaveStringList(const std::list<Lepra::String>& pStringList)
{
	// Compute size of all strings.
	size_t lSize = 0;
	std::list<Lepra::String>::const_iterator x = pStringList.begin();
	for (; x != pStringList.end(); ++x)
	{
		const Lepra::String& lString = *x;
		const int HEAD_SIZE = 4*2;
		lSize += HEAD_SIZE + ((lString.length()+1+3)&(~3));
	}

	Lepra::int64 lChunkEndPosition = 0;
	bool lOk = true;
	if (lOk)
	{
		lOk = SaveHead(CHUNK_STRING_LIST, lSize, lChunkEndPosition);
	}
	x = pStringList.begin();
	for (; lOk && x != pStringList.end(); ++x)
	{
		const Lepra::String& lString = *x;
		lOk = SaveSingleString(lString);
	}
	if (lOk)
	{
		lOk = (mFile->Tell() == lChunkEndPosition);
	}
	return (lOk);
}*/

bool ChunkyLoader::SaveInt(ChunkyType pType, Lepra::int32 pInt)
{
	Lepra::int64 lChunkEndPosition = 0;
	bool lOk = true;
	if (lOk)
	{
		lOk = SaveHead(pType, 4, lChunkEndPosition);
	}
	if (lOk)
	{
		lOk = (mFile->Write(pInt) == Lepra::IO_OK);
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
		for (unsigned y = 0; lOk && y < lElement.mElementCount; ++y)
		{
			Lepra::int64 lChunkEndPosition = 0;
			if (lElement.mIntPointer)
			{
				lOk = SaveInt(lElement.mType, lElement.mIntPointer[y]);
			}
			else if (lElement.mPointer)
			{
				lOk = SaveHead(lElement.mType, lElement.mFieldSize[y], lChunkEndPosition);
				if (lOk)
				{
					lOk = (mFile->WriteData(lElement.mPointer[y], lElement.mFieldSize[y]) == Lepra::IO_OK);
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



bool ChunkyLoader::LoadElementCallback(ChunkyType, size_t, Lepra::int64 pChunkEndPosition, void*)
{
	// Default behavior is to simply skip the chunk.
	bool lOk = (mFile->SeekSet(pChunkEndPosition) == pChunkEndPosition);
	return (lOk);
}



bool ChunkyLoader::VerifyFileType(ChunkyType pType)
{
	size_t lSize = 0;
	Lepra::int64 lChunkEndPosition = 0;
	bool lOk = LoadRequiredHead(pType, lSize, lChunkEndPosition);
	if (lOk && lChunkEndPosition != mFile->GetSize())
	{
		lOk = false;
	}
	return (lOk);
}

bool ChunkyLoader::WriteFileType(ChunkyType pType)
{
	size_t lSize = 0;
	Lepra::int64 lChunkEndPosition;
	bool lOk = SaveHead(pType, lSize, lChunkEndPosition);
	return (lOk);
}



bool ChunkyLoader::LoadHead(ChunkyType& pType, size_t& pSize, Lepra::int64& pChunkEndPosition)
{
	bool lOk = true;
	Lepra::int32 lTempType = 0;
	if (lOk)
	{
		lOk = (mFile->Read(lTempType) == Lepra::IO_OK);
		pType = (ChunkyType)lTempType;
	}
	if (lOk)
	{
		lOk = (mFile->Read(pSize) == Lepra::IO_OK);
	}
	if (lOk)
	{
		pChunkEndPosition = mFile->Tell()+((pSize+3)&(~3));
	}
	return (lOk);
}

bool ChunkyLoader::LoadRequiredHead(ChunkyType pType, size_t& pSize, Lepra::int64& pChunkEndPosition)
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

bool ChunkyLoader::SaveHead(ChunkyType pType, size_t pSize, Lepra::int64& pChunkEndPosition)
{
	bool lOk = true;
	if (lOk)
	{
		Lepra::int32 lTempType = (Lepra::int32)pType;
		lOk = (mFile->Write(lTempType) == Lepra::IO_OK);
	}
	if (lOk)
	{
		lOk = (mFile->Write(pSize) == Lepra::IO_OK);
	}
	if (lOk)
	{
		pChunkEndPosition = mFile->Tell()+((pSize+3)&(~3));
	}
	return (lOk);
}



ChunkyAnimationLoader::ChunkyAnimationLoader(Lepra::File* pFile, bool pIsFileOwner):
	ChunkyLoader(pFile, pIsFileOwner)
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

	Lepra::int32 lDefaultMode = -1;
	mKeyframeCount = -1;	// Using member variable for callback l8r on.
	mBoneCount = -1;	// Using member variable for callback l8r on.
	Lepra::int32 lUseSplines = -1;
	float lAnimationTimeLength = 0;
	Lepra::String lRootNodeName;
	if (lOk)
	{
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_MODE, &lDefaultMode));
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_FRAME_COUNT, &mKeyframeCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_BONE_COUNT, &mBoneCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_USE_SPLINES, &lUseSplines));
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_TIME, (Lepra::int32*)&lAnimationTimeLength));
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_ROOT_NODE, &lRootNodeName));
		lOk = AllocLoadChunkyList(lLoadList);
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
		lOk = AllocLoadChunkyList(lLoadList);
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
	Lepra::int64 lFileDataStart = mFile->Tell();

	// Write animation settings and similar stuff.
	Lepra::int32 lDefaultMode = pAnimation->GetDefaultMode();
	Lepra::int32 lKeyframeCount = pAnimation->GetKeyframeCount();
	Lepra::int32 lBoneCount = pAnimation->GetBoneCount();
	Lepra::int32 lUseSplines = pAnimation->GetUseSplines();
	float lAnimationTimeLength = pAnimation->GetTimeTag(lKeyframeCount);
	Lepra::String lRootNodeName = pAnimation->GetRootNodeName();
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
		lSaveList.push_back(ChunkyFileElement(CHUNK_ANIMATION_TIME, (Lepra::int32*)&lAnimationTimeLength));
		lSaveList.push_back(ChunkyFileElement(CHUNK_ANIMATION_ROOT_NODE, &lRootNodeName));
		lOk = SaveChunkyList(lSaveList);
	}

	// Write actual keyframes.
	const unsigned lTransformFloatCount = 3+4;
	if (lOk)
	{
		// Write all keyframes into a single chunk (multiple element chunk).
		const Lepra::uint32 lFrameSize = (sizeof(Lepra::uint32)*5 + lBoneCount*lTransformFloatCount*sizeof(float)) * lKeyframeCount;
		Lepra::int64 lChunkEndPosition = 0;
		lOk = SaveHead(CHUNK_ANIMATION_KEYFRAME, lFrameSize, lChunkEndPosition);
	}
	if (lOk)
	{
		for (int kf = 0; lOk && kf < lKeyframeCount; ++kf)
		{
			// Save time as sub-chunk to the keyframe chunk.
			float lAnimationTime = pAnimation->GetTimeTag(kf);
			lOk = SaveInt(CHUNK_ANIMATION_KEYFRAME_TIME, *(Lepra::int32*)&lAnimationTime);

			// Save transformations for all bones as sub-chunk to the keyframe chunk.
			const Lepra::uint32 lTransformSize = lBoneCount*lTransformFloatCount*sizeof(float);
			Lepra::int64 lChunkEndPosition = 0;
			lOk = SaveHead(CHUNK_ANIMATION_KEYFRAME_TRANSFORM, lTransformSize, lChunkEndPosition);
			for (int bc = 0; lOk && bc < lBoneCount; ++bc)
			{
				float lTransform[lTransformFloatCount];
				pAnimation->GetBoneTransformation(kf, bc).Get(lTransform);
				lOk = (mFile->WriteData(lTransform, sizeof(lTransform)) == Lepra::IO_OK);
			}
		}
	}

	// Re-write file header size.
	if (lOk)
	{
		size_t lSize = (size_t)(mFile->Tell()-lFileDataStart);
		mFile->SeekSet(lFileDataStart-4);
		lOk = (mFile->Write(lSize) == Lepra::IO_OK);
	}

	return (lOk);
}



bool ChunkyAnimationLoader::LoadElementCallback(ChunkyType pType, size_t pSize, Lepra::int64 pChunkEndPosition, void* pStorage)
{
	bool lOk = false;
	if (pType == CHUNK_ANIMATION_KEYFRAME)
	{
		// Setup pointers and counters for list loading.
		float lKeyframeTime = 0;
		float* lTransformArray = 0;
		unsigned lFloatByteSize = 0;
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_KEYFRAME_TIME, (Lepra::int32*)&lKeyframeTime));
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_KEYFRAME_TRANSFORM, (void**)&lTransformArray, (unsigned*)&lFloatByteSize));
		lOk = AllocLoadChunkyList(lLoadList);

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
				Lepra::TransformationF lTransform(&lTransformArray[lBone*lTransformFloatCount]);
				lAnimation->SetBoneTransformation(mCurrentKeyframe, lBone, lTransform);
			}
			++mCurrentKeyframe;
		}

		delete[] (lTransformArray);
	}
	else
	{
		lOk = ChunkyLoader::LoadElementCallback(pType, pSize, pChunkEndPosition, pStorage);
	}
	return (lOk);
}



ChunkyStructureLoader::ChunkyStructureLoader(Lepra::File* pFile, bool pIsFileOwner):
	ChunkyLoader(pFile, pIsFileOwner)
{
}

ChunkyStructureLoader::~ChunkyStructureLoader()
{
}

bool ChunkyStructureLoader::Load(ChunkyStructure* pStructure)
{
	bool lOk = true;
	if (lOk)
	{
		lOk = VerifyFileType(CHUNK_STRUCTURE);
	}

	Lepra::int32 lBoneCount = -1;
	Lepra::int32 lRootBoneIndex = -1;
	Lepra::int32 lPhysicsType = -1;
	if (lOk)
	{
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_STRUCTURE_PART_COUNT, &lBoneCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_STRUCTURE_ROOT_BONE_INDEX, &lRootBoneIndex));
		lLoadList.push_back(ChunkyFileElement(CHUNK_STRUCTURE_PHYSICS_TYPE, &lPhysicsType));
		lOk = AllocLoadChunkyList(lLoadList);
	}
	if (lOk)
	{
		// Check that all mandatories have been found.
		lOk = (lRootBoneIndex >= 0 && lRootBoneIndex < lBoneCount &&
			lBoneCount >= 1 && lBoneCount < 10000 &&
			(lPhysicsType == ChunkyStructure::STATIC ||
			lPhysicsType == ChunkyStructure::DYNAMIC ||
			lPhysicsType == ChunkyStructure::COLLISION_DETECT_ONLY));
	}
	if (lOk)
	{
		pStructure->SetBoneCount(lBoneCount);
		pStructure->SetRootBone(lRootBoneIndex);
		pStructure->SetPhysicsType((ChunkyStructure::PhysicsType)lPhysicsType);

		FileElementList lLoadList;
		mCurrentBoneIndex = 0;
		lLoadList.push_back(ChunkyFileElement(CHUNK_STRUCTURE_BONE, (void*)pStructure, lBoneCount));
		lOk = AllocLoadChunkyList(lLoadList);
	}
	if (lOk)
	{
		pStructure->FinalizeInit();
	}

	return (lOk);
}

bool ChunkyStructureLoader::Save(const ChunkyStructure* pStructure)
{
	bool lOk = (pStructure->GetBoneCount() > 0);

	// Write file header. We will come back to it later to re-write the actual size.
	if (lOk)
	{
		lOk = WriteFileType(CHUNK_STRUCTURE);
	}
	const Lepra::int64 lFileDataStart = mFile->Tell();

	// Write structure modes and similar stuff.
	Lepra::int32 lBoneCount = pStructure->GetBoneCount();
	Lepra::int32 lRootBoneIndex = pStructure->GetRootBone();
	Lepra::int32 lPhysicsType = pStructure->GetPhysicsType();
	if (lOk)
	{
		lOk = (lBoneCount > 0 && lBoneCount < 10000 &&
			lRootBoneIndex >= 0 && lRootBoneIndex < lBoneCount &&
			lPhysicsType >= ChunkyStructure::STATIC);
	}
	if (lOk)
	{
		FileElementList lSaveList;
		lSaveList.push_back(ChunkyFileElement(CHUNK_STRUCTURE_PART_COUNT, &lBoneCount));
		lSaveList.push_back(ChunkyFileElement(CHUNK_STRUCTURE_ROOT_BONE_INDEX, &lRootBoneIndex));
		lSaveList.push_back(ChunkyFileElement(CHUNK_STRUCTURE_PHYSICS_TYPE, &lPhysicsType));
		lOk = SaveChunkyList(lSaveList);
	}

	// Write actual bone positioning and geometries.
	if (lOk)
	{
		// Write all bone stuff into a single chunk (multiple element chunk).
		Lepra::int64 lChunkEndPosition = 0;
		lOk = SaveHead(CHUNK_STRUCTURE_BONE, 0, lChunkEndPosition);
	}
	const Lepra::int64 lBoneChunkStart = mFile->Tell();
	if (lOk)
	{
		for (int b = 0; lOk && b < lBoneCount; ++b)
		{
			// Save children.
			const int cc = pStructure->GetBoneChildCount(b);
			if (cc > 0)
			{
				Lepra::int64 lChunkEndPosition = 0;
				lOk = SaveHead(CHUNK_STRUCTURE_BONE_CHILD_LIST, cc*sizeof(Lepra::int32), lChunkEndPosition);
				for (int c = 0; lOk && c < cc; ++c)
				{
					Lepra::int32 lChildIndex = pStructure->GetChildIndex(b, c);
					mFile->Write(lChildIndex);
				}
			}

			// Save bone's original transformation.
			if (lOk)
			{
				const unsigned lTransformFloatCount = 3+4;
				const Lepra::uint32 lTransformSize = lTransformFloatCount*sizeof(float);
				Lepra::int64 lChunkEndPosition = 0;
				lOk = SaveHead(CHUNK_STRUCTURE_BONE_TRANSFORM, lTransformSize, lChunkEndPosition);
				float lTransform[lTransformFloatCount];
				pStructure->GetOriginalBoneTransformation(b).Get(lTransform);
				lOk = (mFile->WriteData(lTransform, sizeof(lTransform)) == Lepra::IO_OK);
			}

			// TODO: save structure's geometry lists.
			if (lOk)
			{
			}
		}
	}
	// Re-write bone chunk header size.
	if (lOk)
	{
		Lepra::int64 lFileEnd = mFile->Tell();
		size_t lSize = (size_t)(lFileEnd-lBoneChunkStart);
		mFile->SeekSet(lBoneChunkStart-4);
		lOk = (mFile->Write(lSize) == Lepra::IO_OK);
		mFile->SeekSet(lFileEnd);
	}

	// Re-write file header size.
	if (lOk)
	{
		size_t lSize = (size_t)(mFile->Tell()-lFileDataStart);
		mFile->SeekSet(lFileDataStart-4);
		lOk = (mFile->Write(lSize) == Lepra::IO_OK);
	}

	return (lOk);
}

bool ChunkyStructureLoader::LoadElementCallback(ChunkyType pType, size_t pSize, Lepra::int64 pChunkEndPosition, void* pStorage)
{
	bool lOk = false;
	if (pType == CHUNK_STRUCTURE_BONE)
	{
		// Setup pointers and counters for list loading.
		const int MAXIMUM_CHILD_BONES = 64;
		Lepra::int32 lChildArray[MAXIMUM_CHILD_BONES];
		::memset(lChildArray, -1, sizeof(lChildArray));
		unsigned lChildByteSize = 0;
		float* lTransformArray = 0;
		unsigned lFloatByteSize = 0;
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_STRUCTURE_BONE_CHILD_LIST, lChildArray, &lChildByteSize, MAXIMUM_CHILD_BONES));
		lLoadList.push_back(ChunkyFileElement(CHUNK_STRUCTURE_BONE_TRANSFORM, (void**)&lTransformArray, (unsigned*)&lFloatByteSize));
		// TODO: load physics shapes!
		// lLoadList.push_back(ChunkyFileElement(CHUNK_STRUCTURE_BONE_SHAPE_CAPSULE, ...));
		lOk = AllocLoadChunkyList(lLoadList);

		// Check child array byte size.
		int lChildCount = 0;
		if (lOk)
		{
			lOk = ((lChildByteSize&3) == 0);
			lChildCount = lChildByteSize/sizeof(Lepra::int32);
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
			ChunkyStructure* lStructure = (ChunkyStructure*)pStorage;
			lStructure->SetBoneChildCount(mCurrentBoneIndex, lChildCount);
			for (int x = 0; lOk && x < lChildCount; ++x)
			{
				lOk = (lChildArray[x] >= 0 && lChildArray[x] < lStructure->GetBoneCount());
				if (lOk)
				{
					lStructure->SetChildIndex(mCurrentBoneIndex, x, lChildArray[x]);
				}
			}
			Lepra::TransformationF lTransform(lTransformArray);
			lStructure->SetOriginalBoneTransformation(mCurrentBoneIndex, lTransform);

			++mCurrentBoneIndex;
		}

		delete[] (lTransformArray);
	}
	else
	{
		lOk = ChunkyLoader::LoadElementCallback(pType, pSize, pChunkEndPosition, pStorage);
	}
	return (lOk);
}



}
