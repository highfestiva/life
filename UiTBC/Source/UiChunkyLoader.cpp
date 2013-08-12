
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Include/UiChunkyLoader.h"
#include "../../Lepra/Include/Packer.h"
#include "../Include/UiAnimatedGeometry.h"
#include "../Include/UiChunkyClass.h"
#include "../Include/UiTriangleBasedGeometry.h"



namespace UiTbc
{



ChunkyMeshLoader::ChunkyMeshLoader(File* pFile, bool pIsFileOwner):
	TBC::ChunkyLoader(pFile, pIsFileOwner)
{
}

ChunkyMeshLoader::~ChunkyMeshLoader()
{
}

bool ChunkyMeshLoader::Load(TriangleBasedGeometry* pMeshData, int& pCastsShadows)
{
	bool lOk = true;
	if (lOk)
	{
		lOk = VerifyFileType(TBC::CHUNK_MESH);
	}

	uint32* lLoadVertices = 0;
	unsigned lVerticesSize = 0;
	uint32* lLoadNormals = 0;
	unsigned lNormalsSize = 0;
	uint32* lTriangleIndices = 0;
	unsigned lTriangleIndicesSize = 0;
	uint32* lStripsIndices = 0;
	unsigned lStripsIndicesSize = 0;
	const int lUvCount = 8;
	uint32* lLoadUvs[lUvCount] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned lUvsSize[lUvCount] = {0, 0, 0, 0, 0, 0, 0, 0};
	uint8* lColors = 0;
	unsigned lColorsSize = 0;
	int32 lColorFormat = 0x7FFFFFFD;
	int32 lGeometryVolatility = 0x7FFFFFFD;
	int32 lCastsShadows = 0;
	int32 lIsTwoSided = 0;
	if (lOk)
	{
		TBC::ChunkyLoader::FileElementList lLoadList;
		// TRICKY: these have to be in the exact same order as when saved.
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_VOLATILITY, &lGeometryVolatility));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_CASTS_SHADOWS, &lCastsShadows));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_TWO_SIDED, &lIsTwoSided));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_VERTICES, (void**)&lLoadVertices, &lVerticesSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_TRIANGLES, (void**)&lTriangleIndices, &lTriangleIndicesSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_STRIPS, (void**)&lStripsIndices, &lStripsIndicesSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_NORMALS, (void**)&lLoadNormals, &lNormalsSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_UV, (void**)lLoadUvs, lUvsSize, -lUvCount));	// Specialcasing for array loading.
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_COLOR, (void**)&lColors, &lColorsSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_COLOR_FORMAT, &lColorFormat));
		lOk = AllocLoadChunkyList(lLoadList, mFile->GetSize());
	}
	if (lOk)
	{
		lOk = (lLoadVertices != 0);
	}
	if (lOk)
	{
		lOk = ((!lTriangleIndices)^(!lStripsIndices));	// One and only one must be there.
	}
	if (lOk)
	{
		if (lColors)
		{
			lOk = (lColorFormat == TBC::GeometryBase::COLOR_RGB || lColorFormat == TBC::GeometryBase::COLOR_RGBA);
		}
	}
	if (lOk)
	{
		lOk = (lGeometryVolatility == TBC::GeometryBase::GEOM_STATIC ||
			lGeometryVolatility == TBC::GeometryBase::GEOM_SEMI_STATIC ||
			lGeometryVolatility == TBC::GeometryBase::GEOM_DYNAMIC ||
			lGeometryVolatility == TBC::GeometryBase::GEOM_VOLATILE);
	}
	deb_assert(lOk);
	if (lOk)
	{
		lOk = (lTriangleIndicesSize%(sizeof(uint32)*3) == 0 &&
			lVerticesSize%(sizeof(float)*3) == 0 &&
			lUvCount >= 0 &&
			lUvCount <= 8);
	}
	deb_assert(lOk);
	for (unsigned x = 0; lOk && lUvsSize[x] && x < lUvCount; ++x)
	{
		lOk = (lUvsSize[x] == lVerticesSize*2/3);
	}
	deb_assert(lOk);
	if (lOk)
	{
		// TODO: add checks on normal and color sizes, so that we don't overrun buffers.
	}
	const unsigned lIndexCount = (lTriangleIndices? lTriangleIndicesSize : lStripsIndicesSize) / sizeof(uint32);
	const unsigned lVertexCount = lVerticesSize / (sizeof(float)*3);
	uint32* lIndices = lTriangleIndices? lTriangleIndices : lStripsIndices;
	float* lVertices = (float*)lLoadVertices;
	float* lNormals = (float*)lLoadNormals;
	float* lUvs[lUvCount] = {0, 0, 0, 0, 0, 0, 0, 0};
	if (lOk)
	{
		// Convert to host endian.
		unsigned x;
		for (x = 0; x < lIndexCount; ++x)
		{
			lIndices[x] = Endian::BigToHost(lIndices[x]);
		}
		for (x = 0; x < lVertexCount*3; ++x)
		{
			lVertices[x] = Endian::BigToHostF(lLoadVertices[x]);
		}
		if (lLoadNormals)
		{
			for (x = 0; x < lVertexCount*3; ++x)
			{
				lNormals[x] = Endian::BigToHostF(lLoadNormals[x]);
			}
		}
		for (x = 0; lLoadUvs[x] && (int)x < lUvCount; ++x)
		{
			lUvs[x] = (float*)lLoadUvs[x];
			for (unsigned y = 0; y < lVertexCount*2; ++y)
			{
				lUvs[x][y] = Endian::BigToHostF(lLoadUvs[x][y]);
			}
		}
	}
	if (lOk)
	{
		if (lStripsIndices)
		{
			deb_assert(false);	// Currently not supported.
		}
		const TBC::GeometryBase::PrimitiveType lType = lTriangleIndices? TBC::GeometryBase::TRIANGLES : TBC::GeometryBase::TRIANGLE_STRIP;
		// Alex/TODO: Volatility of TriangleBasedGeometry should always be GEOM_STATIC.
		pMeshData->Set(lVertices, lNormals, lUvs[0], lColors,
			(TBC::GeometryBase::ColorFormat)lColorFormat, lIndices,
			lVertexCount, lIndexCount, lType,
			(TBC::GeometryBase::GeometryVolatility)lGeometryVolatility);

		for (int x = 1; lUvs[x] && x < lUvCount; ++x)
		{
			pMeshData->AddUVSet(lUvs[x]);
		}

		if (lIsTwoSided)
		{
			pMeshData->SetTwoSided(true);
		}

		pCastsShadows = lCastsShadows;
	}
	// TODO: reuse memory, don't new/delete constantly!
	delete[] (lLoadVertices);
	delete[] (lLoadNormals);
	delete[] (lTriangleIndices);
	delete[] (lStripsIndices);
	for (int x = 0; x < lUvCount; ++x)
	{
		delete[] (lLoadUvs[x]);
	}
	delete[] (lColors);
	deb_assert(lOk);
	return (lOk);
}

bool ChunkyMeshLoader::Save(const TriangleBasedGeometry* pMeshData, int pCastsShadows)
{
	// Write file header. We will come back to it later to re-write the actual size.
	bool lOk = true;
	if (lOk)
	{
		lOk = WriteFileType(TBC::CHUNK_MESH);
	}
	int64 lFileDataStart = mFile->Tell();

	// Initialize data and write the mesh itself, exluding vertex weights.
	unsigned lVerticesSize = pMeshData->GetVertexCount()*3*sizeof(float);
	const uint32* lVertices = AllocInitBigEndian(pMeshData->GetVertexData(), lVerticesSize/sizeof(float));
	const float* lSaveNormals = pMeshData->GetNormalData();
	unsigned lNormalsSize = (lSaveNormals? lVerticesSize : 0);
	const uint32* lNormals = AllocInitBigEndian(lSaveNormals, lNormalsSize/sizeof(float));
	unsigned lTriangleIndicesSize = pMeshData->GetIndexCount()*sizeof(uint32);
	const uint32* lTriangleIndices = AllocInitBigEndian(pMeshData->GetIndexData(), lTriangleIndicesSize/sizeof(uint32));
	uint32* lStripsIndices = 0;	// TODO: add strips when supported.
	unsigned lStripsIndicesSize = 0;	// TODO: add strips when supported.
	const int lUvCount = pMeshData->GetUVSetCount();
	const uint32* lUvs[32];
	unsigned lUvsSize[32];
	const unsigned lFixedUvByteSize = pMeshData->GetVertexCount()*2*sizeof(float);
	for (int x = 0; x < lUvCount; ++x)
	{
		lUvsSize[x] = lFixedUvByteSize;
		lUvs[x] = AllocInitBigEndian(pMeshData->GetUVData(x), lUvsSize[x]/sizeof(float));
	}
	const unsigned char* lColors = pMeshData->GetColorData();
	unsigned lColorsSize = 0;
	if (lColors)
	{
		if (pMeshData->GetColorFormat() == TBC::GeometryBase::COLOR_RGB)
		{
			lColorsSize = (pMeshData->GetVertexCount()*3+3)&(~3);
		}
		else	// RGBA.
		{
			lColorsSize = pMeshData->GetVertexCount()*4;
		}
	}
	int32 lColorFormat = pMeshData->GetColorFormat();
	int32 lGeometryVolatility = pMeshData->GetGeometryVolatility();
	int32 lCastsShadows = pCastsShadows;
	int32 lIsTwoSided = pMeshData->IsTwoSided()? 1 : 0;
	if (lOk)
	{
		TBC::ChunkyLoader::FileElementList lSaveList;
		lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_VOLATILITY, &lGeometryVolatility));
		lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_CASTS_SHADOWS, &lCastsShadows));
		lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_TWO_SIDED, &lIsTwoSided));
		lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_VERTICES, (void**)&lVertices, &lVerticesSize));
		if (lTriangleIndices)
		{
			lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_TRIANGLES, (void**)&lTriangleIndices, &lTriangleIndicesSize));
		}
		if (lStripsIndices)
		{
			lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_STRIPS, (void**)&lStripsIndices, &lStripsIndicesSize));
		}
		if (lNormals)
		{
			lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_NORMALS, (void**)&lNormals, &lNormalsSize));
		}
		if (lUvCount)
		{
			lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_UV, (void**)lUvs, lUvsSize, lUvCount));
		}
		if (lColors)
		{
			lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_COLOR, (void**)&lColors, &lColorsSize));
			lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_COLOR_FORMAT, &lColorFormat));
		}
		lOk = SaveChunkyList(lSaveList);
	}

	// Re-write file header size.
	if (lOk)
	{
		uint32 lSize = (uint32)(mFile->Tell()-lFileDataStart);
		mFile->SeekSet(lFileDataStart-4);
		lOk = (mFile->Write(lSize) == IO_OK);
	}

	delete[] (lVertices);
	delete[] (lNormals);
	delete[] (lTriangleIndices);
	delete[] (lStripsIndices);
	for (int x = 0; x < lUvCount; ++x)
	{
		delete[] (lUvs[x]);
	}

	return (lOk);
}



ChunkySkinLoader::ChunkySkinLoader(File* pFile, bool pIsFileOwner):
	TBC::ChunkyLoader(pFile, pIsFileOwner)
{
}

ChunkySkinLoader::~ChunkySkinLoader()
{
}

bool ChunkySkinLoader::Load(AnimatedGeometry* pSkinData)
{
	bool lOk = true;
	if (lOk)
	{
		lOk = VerifyFileType(TBC::CHUNK_SKIN);
	}
	if (lOk)
	{
		TBC::ChunkyLoader::FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_SKIN_BONE_WEIGHT_GROUP, (void*)pSkinData, -1000000));
		lOk = AllocLoadChunkyList(lLoadList, mFile->GetSize());
	}
	return (lOk);
}

bool ChunkySkinLoader::Save(const AnimatedGeometry* pSkinData)
{
	// Write file header. We will come back to it later to re-write the actual size.
	bool lOk = true;
	if (lOk)
	{
		lOk = WriteFileType(TBC::CHUNK_SKIN);
	}
	int64 lFileDataStart = mFile->Tell();
	if (lOk)
	{
		lOk = SaveBoneWeightChunkArray(pSkinData);
	}
	// Re-write file header size.
	if (lOk)
	{
		uint32 lSize = (uint32)(mFile->Tell()-lFileDataStart);
		mFile->SeekSet(lFileDataStart-4);
		lOk = (mFile->Write(lSize) == IO_OK);
	}
	return (lOk);
}



bool ChunkySkinLoader::LoadElementCallback(TBC::ChunkyType pType, uint32 pSize, int64 pChunkEndPosition, void* pStorage)
{
	bool lOk = false;
	if (pType == TBC::CHUNK_SKIN_BONE_WEIGHT_GROUP)
	{
		// Setup pointers and counters for list loading.
		AnimatedGeometry::BoneWeightGroup lWeights;
		lWeights.mBoneCount = 0;
		int* lBoneIndices = 0;
		lWeights.mVectorIndexCount = 0;
		lWeights.mVectorIndexArray = 0;
		unsigned lWeightCount = 0;
		lWeights.mVectorWeightArray = 0;
		TBC::ChunkyLoader::FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_SKIN_BWG_BONES, (void**)&lBoneIndices, (unsigned*)&lWeights.mBoneCount));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_SKIN_BWG_VERTICES, (void**)&lWeights.mVectorIndexArray, (unsigned*)&lWeights.mVectorIndexCount));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_SKIN_BWG_WEIGHTS, (void**)&lWeights.mVectorWeightArray, &lWeightCount));
		lOk = AllocLoadChunkyList(lLoadList, pChunkEndPosition);

		if (lOk)
		{
			// Assert that number of index/weight bytes loaded are a multiple of 4 (i.e. uint32/float).
			lOk = ((lWeights.mBoneCount & 3) == 0 &&
				(lWeights.mVectorIndexCount & 3) == 0 &&
				(lWeightCount & 3) == 0);
			lWeights.mBoneCount /= sizeof(lBoneIndices[0]);
			lWeights.mVectorIndexCount /= sizeof(lWeights.mVectorIndexArray[0]);
			lWeightCount /= sizeof(lWeights.mVectorWeightArray[0]);
		}

		if (lOk)
		{
			// Assert that the number of bones are in range and that the proportion between weights and vertices is correct.
			lOk = (lWeights.mBoneCount >= 1 && lWeights.mBoneCount <= AnimatedGeometry::BoneWeightGroup::MAX_BONES_PER_VERTEX &&
				lWeights.mVectorIndexCount >= 1 && ((int)lWeightCount) == lWeights.mVectorIndexCount*lWeights.mBoneCount);
		}

		if (lOk)
		{
			::memcpy(lWeights.mBoneIndexArray, lBoneIndices, lWeights.mBoneCount*sizeof(lWeights.mBoneIndexArray[0]));
			AnimatedGeometry* lAnimatedData = (AnimatedGeometry*)pStorage;
			lAnimatedData->AddBoneWeights(lWeights);
		}

		if (!lOk)
		{
			delete[] (lWeights.mVectorIndexArray);
			delete[] (lWeights.mVectorWeightArray);
		}
		delete[] (lBoneIndices);	// Always goes, uses BoneWeightGroup uses aggregate array.
	}
	else
	{
		lOk = ChunkyLoader::LoadElementCallback(pType, pSize, pChunkEndPosition, pStorage);
	}
	return (lOk);
}

bool ChunkySkinLoader::SaveBoneWeightChunkArray(const AnimatedGeometry* pSkinData)
{
	// We don't need to write file header or any of that junk, just straight to business.

	bool lOk = (pSkinData->GetBoneWeightGroupCount() >= 1);
	int64 lChunkStartPosition = mFile->Tell();
	if (lOk)
	{
		int64 lChunkEndPosition = 0;
		lOk = SaveHead(TBC::CHUNK_SKIN_BONE_WEIGHT_GROUP, 0, lChunkEndPosition);
	}
	for (int x = 0; lOk && x < pSkinData->GetBoneWeightGroupCount(); ++x)
	{
		const AnimatedGeometry::BoneWeightGroup* lBoneWeightData = &pSkinData->GetBoneWeightGroup(x);
		uint32 bs = lBoneWeightData->mBoneCount*sizeof(uint32);
		uint32 vs = lBoneWeightData->mVectorIndexCount*sizeof(uint32);
		uint32 ws = lBoneWeightData->mBoneCount*lBoneWeightData->mVectorIndexCount*sizeof(uint32);
		TBC::ChunkyLoader::FileElementList lSaveList;
		const int* lBoneIndexArray = lBoneWeightData->mBoneIndexArray;	// TRICKY: intermediate pointer workaround for MSVC8 compiler bug.
		lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_SKIN_BWG_BONES, (void**)&lBoneIndexArray, &bs));
		lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_SKIN_BWG_VERTICES, (void**)&lBoneWeightData->mVectorIndexArray, &vs));
		lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_SKIN_BWG_WEIGHTS, (void**)&lBoneWeightData->mVectorWeightArray, &ws));
		lOk = SaveChunkyList(lSaveList);
	}
	// Re-write chunk size.
	if (lOk)
	{
		int64 lChunkEndPosition = mFile->Tell();
		lOk = (mFile->SeekSet(lChunkStartPosition+4) == lChunkStartPosition+4);
		if (lOk)
		{
			uint32 lSize = (uint32)(lChunkEndPosition-lChunkStartPosition)-8;
			lOk = (mFile->Write(lSize) == IO_OK);
		}
		if (lOk)
		{
			lOk = (mFile->SeekSet(lChunkEndPosition) == lChunkEndPosition);
		}
	}
	return (lOk);
}



ChunkyClassLoader::ChunkyClassLoader(File* pFile, bool pIsFileOwner):
	Parent(pFile, pIsFileOwner)
{
}

ChunkyClassLoader::~ChunkyClassLoader()
{
}

void ChunkyClassLoader::AddLoadElements(Parent::FileElementList& pElementList, TBC::ChunkyClass* pData)
{
	Parent::AddLoadElements(pElementList, pData);
	pElementList.insert(--pElementList.end(), ChunkyFileElement(TBC::CHUNK_CLASS_MESH_LIST, (void*)pData, -1000));
}

bool ChunkyClassLoader::LoadElementCallback(TBC::ChunkyType pType, uint32 pSize, int64 pChunkEndPosition, void* pStorage)
{
	ChunkyClass* lClass = (ChunkyClass*)pStorage;
	bool lOk = false;
	if (pType == TBC::CHUNK_CLASS_MESH_LIST)
	{
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_CLASS_PHYS_MESH, (void*)lClass));
		lOk = AllocLoadChunkyList(lLoadList, pChunkEndPosition);
		deb_assert(lOk);
	}
	else if (pType == TBC::CHUNK_CLASS_PHYS_MESH)
	{
		uint8* lBuffer = 0;
		lOk = (mFile->AllocReadData((void**)&lBuffer, pSize) == IO_OK);
		deb_assert(lOk);
		int32 lPhysicsIndex = -1;
		str lMeshBaseName;
		int lIndex = 0;
		if (lOk)
		{
			lPhysicsIndex = Endian::BigToHost(*(int32*)&lBuffer[lIndex]);
			lIndex += sizeof(lPhysicsIndex);
			const size_t lExcludeByteCount = (1+7+11)*4;	// Index+transform+colors.
			int lStrSize = PackerUnicodeString::Unpack(lMeshBaseName, &lBuffer[lIndex], pSize-lExcludeByteCount);
			lStrSize = (lStrSize+3)&(~3);
			lOk = (lStrSize < (int)(pSize-lExcludeByteCount));
			deb_assert(lOk);
			lIndex += lStrSize;
		}
		if (lOk)
		{
			const int lTransformFloatCount = 7;
			float lTransformArray[lTransformFloatCount];
			for (int x = 0; x < lTransformFloatCount; ++x)
			{
				lTransformArray[x] = Endian::BigToHostF(*(uint32*)&lBuffer[lIndex+x*sizeof(float)]);
			}
			lIndex += lTransformFloatCount * sizeof(float);
			lOk = (lIndex < (int)(pSize - 11*sizeof(float) - 4));
			deb_assert(lOk);
			lClass->AddMesh(lPhysicsIndex, lMeshBaseName, TransformationF(lTransformArray));
			lClass->AddPhysRoot(lPhysicsIndex);
		}
		UiTbc::ChunkyClass::Material lMaterial;
		if (lOk)
		{
			const int lMaterialFloatCount = 12;
			float lMaterialArray[lMaterialFloatCount];
			for (int x = 0; x < lMaterialFloatCount; ++x)
			{
				lMaterialArray[x] = Endian::BigToHostF(*(uint32*)&lBuffer[lIndex+x*sizeof(float)]);
			}
			lIndex += lMaterialFloatCount * sizeof(float);
			lOk = (lIndex <= (int)(pSize-4-4));
			deb_assert(lOk);
			lMaterial.mAmbient.Set(lMaterialArray[0], lMaterialArray[1], lMaterialArray[2]);
			lMaterial.mDiffuse.Set(lMaterialArray[3], lMaterialArray[4], lMaterialArray[5]);
			lMaterial.mSpecular.Set(lMaterialArray[6], lMaterialArray[7], lMaterialArray[8]);
			lMaterial.mShininess = lMaterialArray[9];
			lMaterial.mAlpha = lMaterialArray[10];
			lMaterial.mSmooth = (lMaterialArray[11] > 0.5f);
		}
		if (lOk)
		{
			int32 lTextureCount = Endian::BigToHost(*(int32*)&lBuffer[lIndex]);
			lIndex += sizeof(lTextureCount);
			lOk = (lIndex <= (int)(pSize-2*lTextureCount-2));
			deb_assert(lOk);
			for (int x = 0; lOk && x < lTextureCount; ++x)
			{
				str lTextureName;
				int lStrSize = PackerUnicodeString::Unpack(lTextureName, &lBuffer[lIndex], pSize-lIndex);
				lStrSize = (lStrSize+3)&(~3);
				lOk = (lStrSize <= (int)(pSize-2));
				deb_assert(lOk);
				lIndex += lStrSize;
				lMaterial.mTextureList.push_back(lTextureName);
			}
		}
		if (lOk)
		{
			int lStrSize = PackerUnicodeString::Unpack(lMaterial.mShaderName, &lBuffer[lIndex], pSize-lIndex);
			lStrSize = (lStrSize+3)&(~3);
			lOk = (lStrSize == (int)(pSize-lIndex));
			deb_assert(lOk);
			lIndex += lStrSize;
		}
		if (lOk)
		{
			lClass->SetLastMeshMaterial(lMaterial);
		}
		deb_assert(lOk);
		delete[] (lBuffer);
	}
	else
	{
		lOk = Parent::LoadElementCallback(pType, pSize, pChunkEndPosition, pStorage);
		deb_assert(lOk);
	}
	return (lOk);
}



}
