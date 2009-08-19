
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/ChunkyLoader.h"
#include "../../Lepra/Include/Packer.h"
#include "../Include/Bones.h"
#include "../Include/ChunkyBoneGeometry.h"
#include "../Include/ChunkyClass.h"
#include "../Include/ChunkyPhysics.h"
#include "../Include/PhysicsEngine.h"



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

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType pType, void** pPointer, Lepra::uint32* pFieldSize, int pElementCount):
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

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType pType, Lepra::int32* pInt, Lepra::uint32* pFieldSize, int pElementCount):
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

ChunkyLoader::ChunkyFileElement::ChunkyFileElement(ChunkyType pType, Lepra::String* pString, int pElementCount):
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



bool ChunkyLoader::AllocLoadChunkyList(FileElementList& pLoadList, Lepra::int64 pChunkEnd)
{
	bool lLoadedAllElements = false;
	bool lOk = true;
	bool lAlreadyLoaded = false;
	Lepra::int64 lHeadPosition = mFile->Tell();
	while (!lLoadedAllElements && lOk && mFile->Tell() < pChunkEnd)
	{
		ChunkyType lType = (ChunkyType)0;
		Lepra::uint32 lSize = 0;
		Lepra::int64 lChunkEndPosition = 0;

		// Load element head.
		if (lOk)
		{
			lHeadPosition = mFile->Tell();
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
						Lepra::tchar c[4];
						c[0] = (Lepra::tchar)((lType>>24)&0x7F);
						c[1] = (Lepra::tchar)((lType>>16)&0x7F);
						c[2] = (Lepra::tchar)((lType>>8)&0x7F);
						c[3] = (Lepra::tchar)((lType>>0)&0x7F);
						log_volatile(mLog.Debugf(_T("Loading chunk '%c%c%c%c'."),
							c[0], c[1], c[2], c[3]));
					}
					lElement.mIsElementLoaded = true;
					const int lMaxElementCount = ::abs(lElement.mElementCount);
					for (int y = 0; lOk && y < lMaxElementCount; ++y)
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
							Lepra::uint8* lString = 0;
							lOk = (lSize >= 4 && (lSize&1) == 0);
							if (lOk)
							{
								lOk = (mFile->AllocReadData((void**)&lString, lSize) == Lepra::IO_OK);
							}
							if (lOk)
							{
								Lepra::UnicodeString lUnicodeString;
								lOk = (Lepra::PackerUnicodeString::Unpack(&lUnicodeString, lString, lSize) == (int)lSize);
								if (lOk)
								{
									lElement.mString[y] = Lepra::UnicodeStringUtility::ToCurrentCode(lUnicodeString);
								}
							}
							delete[] (lString);
						}

						if (mFile->Tell() >= lChunkEndPosition)
						{
							lOk = (y == lElement.mElementCount-1 || lElement.mElementCount <= 0);
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



bool ChunkyLoader::SaveSingleString(ChunkyType pType, const Lepra::String& pString)
{
	// Write padding at once. No CPU safety border (a.k.a. page boundary) crosses the 4-byte alignment boundary.
	const Lepra::uint32 lSize = (Lepra::uint32)((2 + pString.length()*2+2 + 3)&(~3));
	Lepra::int64 lChunkEndPosition = 0;
	bool lOk = true;
	if (lOk)
	{
		lOk = SaveHead(pType, lSize, lChunkEndPosition);
	}
	if (lOk)
	{
		Lepra::uint8* lData = new Lepra::uint8[lSize];
		::memset(lData, 0, lSize);
		Lepra::PackerUnicodeString::Pack(lData, Lepra::UnicodeStringUtility::ToOwnCode(pString));
		lOk = (mFile->WriteData(lData, lSize) == Lepra::IO_OK);
		delete[] (lData);
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
		for (int y = 0; lOk && y < lElement.mElementCount; ++y)
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



bool ChunkyLoader::LoadElementCallback(ChunkyType, Lepra::uint32, Lepra::int64 pChunkEndPosition, void*)
{
	// Default behavior is to simply skip the chunk.
	bool lOk = (mFile->SeekSet(pChunkEndPosition) == pChunkEndPosition);
	return (lOk);
}



bool ChunkyLoader::VerifyFileType(ChunkyType pType)
{
	Lepra::uint32 lSize = 0;
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
	Lepra::uint32 lSize = 0;
	Lepra::int64 lChunkEndPosition;
	bool lOk = SaveHead(pType, lSize, lChunkEndPosition);
	return (lOk);
}



bool ChunkyLoader::LoadHead(ChunkyType& pType, Lepra::uint32& pSize, Lepra::int64& pChunkEndPosition)
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
		Lepra::uint32 lSize;
		lOk = (mFile->Read(lSize) == Lepra::IO_OK);
		pSize = lSize;
	}
	if (lOk)
	{
		pChunkEndPosition = mFile->Tell()+((pSize+3)&(~3));
	}
	return (lOk);
}

bool ChunkyLoader::LoadRequiredHead(ChunkyType pType, Lepra::uint32& pSize, Lepra::int64& pChunkEndPosition)
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

bool ChunkyLoader::SaveHead(ChunkyType pType, Lepra::uint32 pSize, Lepra::int64& pChunkEndPosition)
{
	bool lOk = true;
	if (lOk)
	{
		Lepra::int32 lTempType = (Lepra::int32)pType;
		lOk = (mFile->Write(lTempType) == Lepra::IO_OK);
	}
	if (lOk)
	{
		Lepra::uint32 lSize = (Lepra::uint32)pSize;
		lOk = (mFile->Write(lSize) == Lepra::IO_OK);
	}
	if (lOk)
	{
		pChunkEndPosition = mFile->Tell()+((pSize+3)&(~3));
	}
	return (lOk);
}

bool ChunkyLoader::RewriteChunkSize(Lepra::int64 pChunkStartPosition)
{
	Lepra::uint32 lSize = (Lepra::uint32)(mFile->Tell() - pChunkStartPosition);
	mFile->SeekSet(pChunkStartPosition-4);
	bool lOk = (mFile->Write(lSize) == Lepra::IO_OK);
	mFile->SeekEnd(0);
	return (lOk);
}

Lepra::uint32* ChunkyLoader::AllocInitBigEndian(const float* pData, unsigned pCount)
{
	if (!pData || !pCount)
	{
		return (0);
	}
	Lepra::uint32* lData = new Lepra::uint32[pCount];
	for (unsigned x = 0; x < pCount; ++x)
	{
		lData[x] = Lepra::Endian::HostToBigF(pData[x]);
	}
	return (lData);
}

Lepra::uint32* ChunkyLoader::AllocInitBigEndian(const Lepra::uint32* pData, unsigned pCount)
{
	if (!pData || !pCount)
	{
		return (0);
	}
	Lepra::uint32* lData = new Lepra::uint32[pCount];
	for (unsigned x = 0; x < pCount; ++x)
	{
		lData[x] = Lepra::Endian::HostToBig(pData[x]);
	}
	return (lData);
}



ChunkyAnimationLoader::ChunkyAnimationLoader(Lepra::File* pFile, bool pIsFileOwner):
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
		lOk = RewriteChunkSize(lFileDataStart);
	}

	return (lOk);
}



bool ChunkyAnimationLoader::LoadElementCallback(ChunkyType pType, Lepra::uint32 pSize, Lepra::int64 pChunkEndPosition, void* pStorage)
{
	bool lOk = false;
	if (pType == CHUNK_ANIMATION_KEYFRAME)
	{
		// Setup pointers and counters for list loading.
		float lKeyframeTime = 0;
		float* lTransformArray = 0;
		Lepra::uint32 lFloatByteSize = 0;
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_ANIMATION_KEYFRAME_TIME, (Lepra::int32*)&lKeyframeTime));
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
				Lepra::TransformationF lTransform(&lTransformArray[lBone*lTransformFloatCount]);
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



ChunkyPhysicsLoader::ChunkyPhysicsLoader(Lepra::File* pFile, bool pIsFileOwner):
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

	Lepra::int32 lBoneCount = -1;
	Lepra::int32 lPhysicsType = -1;
	Lepra::int32 lEngineCount = -1;
	if (lOk)
	{
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_BONE_COUNT, &lBoneCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_PHYSICS_TYPE, &lPhysicsType));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_ENGINE_COUNT, &lEngineCount));
		lOk = AllocLoadChunkyList(lLoadList, mFile->GetSize());
	}
	if (lOk)
	{
		// Check that all mandatories have been found.
		lOk = (lBoneCount >= 1 && lBoneCount < 10000 &&
			(lPhysicsType == ChunkyPhysics::STATIC ||
			lPhysicsType == ChunkyPhysics::DYNAMIC ||
			lPhysicsType == ChunkyPhysics::COLLISION_DETECT_ONLY) &&
			lEngineCount >= 0 && lEngineCount < 1000);
	}
	if (lOk)
	{
		pPhysics->SetBoneCount(lBoneCount);
		pPhysics->SetPhysicsType((ChunkyPhysics::PhysicsType)lPhysicsType);

		FileElementList lLoadList;
		mCurrentBoneIndex = 0;
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_BONE_CONTAINER, (void*)pPhysics, lBoneCount));
		lLoadList.push_back(ChunkyFileElement(CHUNK_PHYSICS_ENGINE_CONTAINER, (void*)pPhysics, lEngineCount));
		lOk = AllocLoadChunkyList(lLoadList, mFile->GetSize());
	}
	if (lOk)
	{
		lOk = (lEngineCount == pPhysics->GetEngineCount());
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
	const Lepra::int64 lFileDataStart = mFile->Tell();

	// Write physics modes and similar stuff.
	Lepra::int32 lBoneCount = pPhysics->GetBoneCount();
	Lepra::int32 lPhysicsType = pPhysics->GetPhysicsType();
	Lepra::int32 lEngineCount = pPhysics->GetEngineCount();
	if (lOk)
	{
		lOk = (lBoneCount > 0 && lBoneCount < 10000 &&
			lPhysicsType >= ChunkyPhysics::STATIC &&
			lEngineCount >= 0 && lEngineCount < 1000);
	}
	if (lOk)
	{
		FileElementList lSaveList;
		lSaveList.push_back(ChunkyFileElement(CHUNK_PHYSICS_BONE_COUNT, &lBoneCount));
		lSaveList.push_back(ChunkyFileElement(CHUNK_PHYSICS_PHYSICS_TYPE, &lPhysicsType));
		lSaveList.push_back(ChunkyFileElement(CHUNK_PHYSICS_ENGINE_COUNT, &lEngineCount));
		lOk = SaveChunkyList(lSaveList);
	}

	// Write actual bone positioning and geometries.
	if (lOk)
	{
		// Write all bone stuff into a single chunk (multiple element chunk).
		Lepra::int64 lChunkEndPosition = 0;
		lOk = SaveHead(CHUNK_PHYSICS_BONE_CONTAINER, 0, lChunkEndPosition);
	}
	const Lepra::int64 lBoneChunkStart = mFile->Tell();
	for (int b = 0; lOk && b < lBoneCount; ++b)
	{
		// Save children.
		const int cc = pPhysics->GetBoneChildCount(b);
		if (cc > 0)
		{
			Lepra::int64 lChunkEndPosition = 0;
			lOk = SaveHead(CHUNK_PHYSICS_BONE_CHILD_LIST, cc*sizeof(Lepra::int32), lChunkEndPosition);
			for (int c = 0; lOk && c < cc; ++c)
			{
				Lepra::int32 lChildIndex = pPhysics->GetChildIndex(b, c);
				mFile->Write(lChildIndex);
			}
		}

		// Save bone's original transformation.
		if (lOk)
		{
			const unsigned lTransformFloatCount = 3+4;
			const Lepra::uint32 lTransformSize = lTransformFloatCount*sizeof(float);
			Lepra::int64 lChunkEndPosition = 0;
			lOk = SaveHead(CHUNK_PHYSICS_BONE_TRANSFORM, lTransformSize, lChunkEndPosition);
			Lepra::uint32 lTransform[lTransformFloatCount];
			pPhysics->GetBoneTransformation(b).Get((float*)lTransform);
			// Convert to network byte order.
			for (unsigned x = 0; x < lTransformFloatCount; ++x)
			{
				lTransform[x] = Lepra::Endian::HostToBig(lTransform[x]);
			}
			lOk = (mFile->WriteData(lTransform, sizeof(lTransform)) == Lepra::IO_OK);
		}

		// Save geometries.
		if (lOk)
		{
			ChunkyBoneGeometry* lGeometry = pPhysics->GetBoneGeometry(b);
			if (lGeometry)
			{
				Lepra::int64 lChunkEndPosition = 0;
				unsigned lSize = lGeometry->GetChunkySize();
				lOk = SaveHead(CHUNK_PHYSICS_BONE_SHAPE, lSize, lChunkEndPosition);
				if (lOk)
				{
					char* lData = new char[lSize];
					lGeometry->SaveChunkyData(pPhysics, lData);
					assert(lGeometry->GetChunkySize(lData) == lSize);
					lOk = (mFile->WriteData(lData, lSize) == Lepra::IO_OK);
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
		Lepra::int64 lChunkEndPosition = 0;
		lOk = SaveHead(CHUNK_PHYSICS_ENGINE_CONTAINER, 0, lChunkEndPosition);
	}
	const Lepra::int64 lEngineChunkStart = mFile->Tell();
	// Write engines.
	for (int e = 0; lOk && e < lEngineCount; ++e)
	{
		PhysicsEngine* lEngine = pPhysics->GetEngine(e);
		assert(lEngine);
		Lepra::int64 lChunkEndPosition = 0;
		unsigned lSize = lEngine->GetChunkySize();
		lOk = SaveHead(CHUNK_PHYSICS_ENGINE, lSize, lChunkEndPosition);
		if (lOk)
		{
			char* lData = new char[lSize];
			lEngine->SaveChunkyData(pPhysics, lData);
			lOk = (mFile->WriteData(lData, lSize) == Lepra::IO_OK);
			delete (lData);
		}
	}
	if (lOk)
	{
		lOk = RewriteChunkSize(lEngineChunkStart);
	}

	// Re-write file header size.
	if (lOk)
	{
		lOk = RewriteChunkSize(lFileDataStart);
	}

	return (lOk);
}

bool ChunkyPhysicsLoader::LoadElementCallback(ChunkyType pType, Lepra::uint32 pSize, Lepra::int64 pChunkEndPosition, void* pStorage)
{
	ChunkyPhysics* lPhysics = (ChunkyPhysics*)pStorage;
	bool lOk = false;
	if (pType == CHUNK_PHYSICS_BONE_CONTAINER)
	{
		// Setup pointers and counters for list loading.
		const int MAXIMUM_CHILD_BONES = 64;
		Lepra::int32 lChildArray[MAXIMUM_CHILD_BONES];
		::memset(lChildArray, -1, sizeof(lChildArray));
		unsigned lChildByteSize = 0;
		Lepra::uint32* lTransformArray = 0;
		unsigned lFloatByteSize = 0;
		Lepra::uint32* lGeometryArray = 0;
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
			log_volatile(mLog.Debugf(_T("Current bone index is %i."),
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
				lTransformArray[x] = Lepra::Endian::BigToHost(lTransformArray[x]);
			}
			Lepra::TransformationF lTransform((const float*)lTransformArray);
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
		Lepra::uint32* lEngineArray = 0;
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
	else
	{
		lOk = Parent::LoadElementCallback(pType, pSize, pChunkEndPosition, pStorage);
	}
	return (lOk);
}



ChunkyClassLoader::ChunkyClassLoader(Lepra::File* pFile, bool pIsFileOwner):
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
	}

	return (lOk);
}

void ChunkyClassLoader::AddLoadElements(FileElementList& pElementList, ChunkyClass* pData)
{
	pElementList.push_back(ChunkyFileElement(CHUNK_CLASS_PHYSICS, &pData->GetPhysicsBaseName()));
}



LOG_CLASS_DEFINE(GENERAL_RESOURCES, ChunkyLoader);
LOG_CLASS_DEFINE(GENERAL_RESOURCES, ChunkyAnimationLoader);
LOG_CLASS_DEFINE(GENERAL_RESOURCES, ChunkyPhysicsLoader);
LOG_CLASS_DEFINE(GENERAL_RESOURCES, ChunkyClassLoader);



}
