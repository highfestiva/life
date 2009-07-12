
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/UiAnimatedGeometry.h"
#include "../Include/UiChunkyLoader.h"
#include "../Include/UiTriangleBasedGeometry.h"



namespace UiTbc
{



/*ChunkyClassLoader::ChunkyClassLoader(Lepra::File* pFile):
      TBC::ChunkyClassLoader(pFile)
{
}

ChunkyClassLoader::~ChunkyClassLoader()
{
}

bool ChunkyClassLoader::Load(ChunkyClass* pMeshData)
{
	pData;
	return (true);
}

bool ChunkyClassLoader::Save(const ChunkyClass* pData)
{
	pData;
	return (true);
}*/



ChunkyMeshLoader::ChunkyMeshLoader(Lepra::File* pFile, bool pIsFileOwner):
	TBC::ChunkyLoader(pFile, pIsFileOwner)
{
}

ChunkyMeshLoader::~ChunkyMeshLoader()
{
}

bool ChunkyMeshLoader::Load(TriangleBasedGeometry* pMeshData)
{
	bool lOk = true;
	if (lOk)
	{
		lOk = VerifyFileType(TBC::CHUNK_MESH);
	}

	float* lVertices = 0;
	unsigned lVerticesSize = 0;
	float* lNormals = 0;
	unsigned lNormalsSize = 0;
	Lepra::uint32* lTriangleIndices = 0;
	unsigned lTriangleIndicesSize = 0;
	Lepra::uint32* lStripsIndices = 0;
	unsigned lStripsIndicesSize = 0;
	const int lUvCount = 8;
	float* lUvs[lUvCount] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned lUvsSize[lUvCount] = {0, 0, 0, 0, 0, 0, 0, 0};
	Lepra::uint8* lColors = 0;
	unsigned lColorsSize = 0;
	Lepra::int32 lColorFormat = 0x7FFFFFFD;
	Lepra::int32 lGeometryVolatility = 0x7FFFFFFD;
	if (lOk)
	{
		TBC::ChunkyLoader::FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_VERTICES, (void**)&lVertices, &lVerticesSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_NORMALS, (void**)&lNormals, &lNormalsSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_TRIANGLES, (void**)&lTriangleIndices, &lTriangleIndicesSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_STRIPS, (void**)&lStripsIndices, &lStripsIndicesSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_UV, (void**)lUvs, lUvsSize, lUvCount));	// Specialcasing for array loading.
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_COLOR, (void**)&lColors, &lColorsSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_COLOR_FORMAT, &lColorFormat));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_VOLATILITY, &lGeometryVolatility));
		lOk = AllocLoadChunkyList(lLoadList, mFile->GetSize());
	}
	if (lOk)
	{
		lOk = (lVertices != 0);
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
			lGeometryVolatility == TBC::GeometryBase::GEOM_DYNAMIC ||
			lGeometryVolatility == TBC::GeometryBase::GEOM_VOLATILE);
	}
	if (lOk)
	{
		// TODO: add checks of all loaded field sizes, so that we don't overrun buffers.
	}
	if (lOk)
	{
		if (lStripsIndices)
		{
			// TODO: add strips when supported.
			// Alex/TODO: Volatility of TriangleBasedGeometry should always be GEOM_STATIC.
			//pMeshData->Set(lVertices, lNormals, lUvs, lColors, (TBC::GeometryBase::ColorFormat)lColorFormat, lStripsIndices, lVerticesSize/12, lStripsIndicesSize/4, TBC::GeometryBase::TRIANGLE_STRIP, (TBC::GeometryBase::GeometryVolatility)lGeometryVolatility);
		}
		else
		{
			// Alex/TODO: Volatility of TriangleBasedGeometry should always be GEOM_STATIC.
			pMeshData->Set(lVertices, lNormals, lUvs[0], lColors, (TBC::GeometryBase::ColorFormat)lColorFormat, lTriangleIndices, lVerticesSize/12, lTriangleIndicesSize/4, TBC::GeometryBase::TRIANGLES, (TBC::GeometryBase::GeometryVolatility)lGeometryVolatility);
		}
		for (int x = 1; lUvs[x] && x < lUvCount; ++x)
		{
			pMeshData->AddUVSet(lUvs[x]);
		}
	}
	// TODO: reuse memory, don't new/delete constantly!
	delete[] (lVertices);
	delete[] (lNormals);
	delete[] (lTriangleIndices);
	delete[] (lStripsIndices);
	for (int x = 0; lUvs[x] && x < lUvCount; ++x)
	{
		delete[] (lUvs[x]);
	}
	delete[] (lColors);
	return (lOk);
}

bool ChunkyMeshLoader::Save(const TriangleBasedGeometry* pMeshData)
{
	// Write file header. We will come back to it later to re-write the actual size.
	bool lOk = true;
	if (lOk)
	{
		lOk = WriteFileType(TBC::CHUNK_MESH);
	}
	Lepra::int64 lFileDataStart = mFile->Tell();

	// Initialize data and write the mesh itself, exluding vertex weights.
	const float* lVertices = pMeshData->GetVertexData();
	unsigned lVerticesSize = pMeshData->GetVertexCount()*3*sizeof(float);
	const float* lNormals = pMeshData->GetNormalData();
	unsigned lNormalsSize = (lNormals? lVerticesSize : 0);
	const Lepra::uint32* lTriangleIndices = pMeshData->GetIndexData();
	unsigned lTriangleIndicesSize = pMeshData->GetIndexCount()*sizeof(long);
	unsigned long* lStripsIndices = 0;	// TODO: add strips when supported.
	unsigned lStripsIndicesSize = 0;	// TODO: add strips when supported.
	const int lUvCount = pMeshData->GetUVSetCount();
	const float* lUvs[32];
	unsigned lUvsSize[32];
	const unsigned lFixedUvByteSize = pMeshData->GetVertexCount()*2*sizeof(float);
	for (int x = 0; x < lUvCount; ++x)
	{
		lUvs[x] = pMeshData->GetUVData(x);
		lUvsSize[x] = lFixedUvByteSize;
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
	Lepra::int32 lColorFormat = pMeshData->GetColorFormat();
	Lepra::int32 lGeometryVolatility = pMeshData->GetGeometryVolatility();
	if (lOk)
	{
		TBC::ChunkyLoader::FileElementList lSaveList;
		lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_VERTICES, (void**)&lVertices, &lVerticesSize));
		if (lNormals)
		{
			lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_NORMALS, (void**)&lNormals, &lNormalsSize));
		}
		if (lTriangleIndices)
		{
			lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_TRIANGLES, (void**)&lTriangleIndices, &lTriangleIndicesSize));
		}
		if (lStripsIndices)
		{
			lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_STRIPS, (void**)&lStripsIndices, &lStripsIndicesSize));
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
		lSaveList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_VOLATILITY, &lGeometryVolatility));
		lOk = SaveChunkyList(lSaveList);
	}

	// Re-write file header size.
	if (lOk)
	{
		Lepra::uint32 lSize = (Lepra::uint32)(mFile->Tell()-lFileDataStart);
		mFile->SeekSet(lFileDataStart-4);
		lOk = (mFile->Write(lSize) == Lepra::IO_OK);
	}

	return (lOk);
}



ChunkySkinLoader::ChunkySkinLoader(Lepra::File* pFile, bool pIsFileOwner):
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
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_SKIN_BONE_WEIGHT_GROUP, (void*)pSkinData, 10000));
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
	Lepra::int64 lFileDataStart = mFile->Tell();
	if (lOk)
	{
		lOk = SaveBoneWeightChunkArray(pSkinData);
	}
	// Re-write file header size.
	if (lOk)
	{
		Lepra::uint32 lSize = (Lepra::uint32)(mFile->Tell()-lFileDataStart);
		mFile->SeekSet(lFileDataStart-4);
		lOk = (mFile->Write(lSize) == Lepra::IO_OK);
	}
	return (lOk);
}



bool ChunkySkinLoader::LoadElementCallback(TBC::ChunkyType pType, Lepra::uint32 pSize, Lepra::int64 pChunkEndPosition, void* pStorage)
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
	Lepra::int64 lChunkStartPosition = mFile->Tell();
	if (lOk)
	{
		Lepra::int64 lChunkEndPosition = 0;
		lOk = SaveHead(TBC::CHUNK_SKIN_BONE_WEIGHT_GROUP, 0, lChunkEndPosition);
	}
	for (int x = 0; lOk && x < pSkinData->GetBoneWeightGroupCount(); ++x)
	{
		const AnimatedGeometry::BoneWeightGroup* lBoneWeightData = &pSkinData->GetBoneWeightGroup(x);
		Lepra::uint32 bs = lBoneWeightData->mBoneCount*sizeof(Lepra::uint32);
		Lepra::uint32 vs = lBoneWeightData->mVectorIndexCount*sizeof(Lepra::uint32);
		Lepra::uint32 ws = lBoneWeightData->mBoneCount*lBoneWeightData->mVectorIndexCount*sizeof(Lepra::uint32);
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
		Lepra::int64 lChunkEndPosition = mFile->Tell();
		lOk = (mFile->SeekSet(lChunkStartPosition+4) == lChunkStartPosition+4);
		if (lOk)
		{
			Lepra::uint32 lSize = (Lepra::uint32)(lChunkEndPosition-lChunkStartPosition)-8;
			lOk = (mFile->Write(lSize) == Lepra::IO_OK);
		}
		if (lOk)
		{
			lOk = (mFile->SeekSet(lChunkEndPosition) == lChunkEndPosition);
		}
	}
	return (lOk);
}



}
