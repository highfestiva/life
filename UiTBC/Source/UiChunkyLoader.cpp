
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#include "../Include/UiChunkyLoader.h"
#include "../../Lepra/Include/Packer.h"
#include "../Include/UiAnimatedGeometry.h"
#include "../Include/UiChunkyClass.h"
#include "../Include/UiTriangleBasedGeometry.h"



namespace UiTbc
{



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

	Lepra::uint32* lLoadVertices = 0;
	unsigned lVerticesSize = 0;
	Lepra::uint32* lLoadNormals = 0;
	unsigned lNormalsSize = 0;
	Lepra::uint32* lTriangleIndices = 0;
	unsigned lTriangleIndicesSize = 0;
	Lepra::uint32* lStripsIndices = 0;
	unsigned lStripsIndicesSize = 0;
	const int lUvCount = 8;
	Lepra::uint32* lLoadUvs[lUvCount] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned lUvsSize[lUvCount] = {0, 0, 0, 0, 0, 0, 0, 0};
	Lepra::uint8* lColors = 0;
	unsigned lColorsSize = 0;
	Lepra::int32 lColorFormat = 0x7FFFFFFD;
	Lepra::int32 lGeometryVolatility = 0x7FFFFFFD;
	if (lOk)
	{
		TBC::ChunkyLoader::FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_VERTICES, (void**)&lLoadVertices, &lVerticesSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_NORMALS, (void**)&lLoadNormals, &lNormalsSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_TRIANGLES, (void**)&lTriangleIndices, &lTriangleIndicesSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_STRIPS, (void**)&lStripsIndices, &lStripsIndicesSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_UV, (void**)lLoadUvs, lUvsSize, lUvCount));	// Specialcasing for array loading.
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_COLOR, (void**)&lColors, &lColorsSize));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_COLOR_FORMAT, &lColorFormat));
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_MESH_VOLATILITY, &lGeometryVolatility));
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
			lGeometryVolatility == TBC::GeometryBase::GEOM_DYNAMIC ||
			lGeometryVolatility == TBC::GeometryBase::GEOM_VOLATILE);
	}
	if (lOk)
	{
		lOk = (lTriangleIndicesSize%(sizeof(Lepra::uint32)*3) == 0 &&
			lVerticesSize%(sizeof(float)*3) == 0);
	}
	if (lOk)
	{
		// TODO: add checks on normal, uv and color sizes, so that we don't overrun buffers.
	}
	const unsigned lIndexCount = (lTriangleIndices? lTriangleIndicesSize : lStripsIndicesSize) / sizeof(Lepra::uint32);
	const unsigned lVertexCount = lVerticesSize / (sizeof(float)*3);
	Lepra::uint32* lIndices = lTriangleIndices? lTriangleIndices : lStripsIndices;
	float* lVertices = (float*)lLoadVertices;
	float* lNormals = (float*)lLoadNormals;
	float* lUvs[lUvCount] = {0, 0, 0, 0, 0, 0, 0, 0};
	if (lOk)
	{
		// Convert to host endian.
		unsigned x;
		for (x = 0; x < lIndexCount; ++x)
		{
			lIndices[x] = Lepra::Endian::BigToHost(lIndices[x]);
		}
		for (x = 0; x < lVertexCount*3; ++x)
		{
			lVertices[x] = Lepra::Endian::BigToHostF(lLoadVertices[x]);
		}
		if (lLoadNormals)
		{
			for (x = 0; x < lVertexCount*3; ++x)
			{
				lNormals[x] = Lepra::Endian::BigToHostF(lLoadNormals[x]);
			}
		}
		for (x = 0; lLoadUvs[x] && x < lUvCount; ++x)
		{
			lUvs[x] = (float*)lLoadUvs[x];
			for (unsigned y = 0; y < lVertexCount*2; ++y)
			{
				lUvs[x][y] = Lepra::Endian::BigToHostF(lLoadUvs[x][y]);
			}
		}
	}
	if (lOk)
	{
		if (lStripsIndices)
		{
			assert(false);	// Currently not supported.
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
	unsigned lVerticesSize = pMeshData->GetVertexCount()*3*sizeof(float);
	const Lepra::uint32* lVertices = AllocInitBigEndian(pMeshData->GetVertexData(), lVerticesSize/sizeof(float));
	const float* lSaveNormals = pMeshData->GetNormalData();
	unsigned lNormalsSize = (lSaveNormals? lVerticesSize : 0);
	const Lepra::uint32* lNormals = AllocInitBigEndian(lSaveNormals, lNormalsSize/sizeof(float));
	unsigned lTriangleIndicesSize = pMeshData->GetIndexCount()*sizeof(Lepra::uint32);
	const Lepra::uint32* lTriangleIndices = AllocInitBigEndian(pMeshData->GetIndexData(), lTriangleIndicesSize/sizeof(Lepra::uint32));
	Lepra::uint32* lStripsIndices = 0;	// TODO: add strips when supported.
	unsigned lStripsIndicesSize = 0;	// TODO: add strips when supported.
	const int lUvCount = pMeshData->GetUVSetCount();
	const Lepra::uint32* lUvs[32];
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



ChunkyClassLoader::ChunkyClassLoader(Lepra::File* pFile, bool pIsFileOwner):
	Parent(pFile, pIsFileOwner)
{
}

ChunkyClassLoader::~ChunkyClassLoader()
{
}

void ChunkyClassLoader::AddLoadElements(Parent::FileElementList& pElementList, TBC::ChunkyClass* pData)
{
	Parent::AddLoadElements(pElementList, pData);
	pElementList.push_back(ChunkyFileElement(TBC::CHUNK_CLASS_MESH_LIST, (void*)pData, -1000));
}

bool ChunkyClassLoader::LoadElementCallback(TBC::ChunkyType pType, Lepra::uint32 pSize, Lepra::int64 pChunkEndPosition, void* pStorage)
{
	ChunkyClass* lClass = (ChunkyClass*)pStorage;
	bool lOk = false;
	if (pType == TBC::CHUNK_CLASS_MESH_LIST)
	{
		FileElementList lLoadList;
		lLoadList.push_back(ChunkyFileElement(TBC::CHUNK_CLASS_PHYS_MESH, (void*)lClass));
		lOk = AllocLoadChunkyList(lLoadList, pChunkEndPosition);
	}
	else if (pType == TBC::CHUNK_CLASS_PHYS_MESH)
	{
		Lepra::uint8* lBuffer = 0;
		lOk = (mFile->AllocReadData((void**)&lBuffer, pSize) == Lepra::IO_OK);
		Lepra::int32 lPhysicsIndex = -1;
		Lepra::String lMeshBaseName;
		int lIndex = 0;
		if (lOk)
		{
			lPhysicsIndex = Lepra::Endian::BigToHost(*(Lepra::int32*)&lBuffer[lIndex]);
			lIndex += sizeof(lPhysicsIndex);
			Lepra::UnicodeString lUnicodeMeshName;
			const size_t lExcludeByteCount = (1+7+4*3)*4;	// Index+transform.
			int lStrSize = Lepra::PackerUnicodeString::Unpack(&lUnicodeMeshName, &lBuffer[lIndex], pSize-lExcludeByteCount);
			lStrSize = (lStrSize+3)&(~3);
			lOk = (lStrSize < (int)(pSize-lExcludeByteCount));
			lIndex += lStrSize;
			lMeshBaseName = Lepra::UnicodeStringUtility::ToCurrentCode(lUnicodeMeshName);
		}
		if (lOk)
		{
			const int lTransformFloatCount = 7;
			float lTransformArray[lTransformFloatCount];
			for (int x = 0; x < lTransformFloatCount; ++x)
			{
				lTransformArray[x] = Lepra::Endian::BigToHostF(*(Lepra::uint32*)&lBuffer[lIndex+x*sizeof(float)]);
			}
			lIndex += lTransformFloatCount * sizeof(float);
			lOk = (lIndex < (int)(pSize - 4*3*sizeof(float) - 4));
			lClass->AddMesh(lPhysicsIndex, lMeshBaseName, Lepra::TransformationF(lTransformArray));
		}
		UiTbc::ChunkyClass::Material lMaterial;
		if (lOk)
		{
			const int lMaterialFloatCount = 4*3;
			float lMaterialArray[lMaterialFloatCount];
			for (int x = 0; x < lMaterialFloatCount; ++x)
			{
				lMaterialArray[x] = Lepra::Endian::BigToHostF(*(Lepra::uint32*)&lBuffer[lIndex+x*sizeof(float)]);
			}
			lIndex += lMaterialFloatCount * sizeof(float);
			lOk = (lIndex <= (int)(pSize-4-4));
			lMaterial.mAmbient.Set(lMaterialArray[0], lMaterialArray[1], lMaterialArray[2], lMaterialArray[3]);
			lMaterial.mDiffuse.Set(lMaterialArray[4], lMaterialArray[5], lMaterialArray[6], lMaterialArray[7]);
			lMaterial.mSpecular.Set(lMaterialArray[8], lMaterialArray[9], lMaterialArray[10], lMaterialArray[11]);
		}
		if (lOk)
		{
			Lepra::int32 lTextureCount = Lepra::Endian::BigToHost(*(Lepra::int32*)&lBuffer[lIndex]);
			lIndex += sizeof(lTextureCount);
			lOk = (lIndex <= (int)(pSize-2*lTextureCount-2));
			for (int x = 0; lOk && x < lTextureCount; ++x)
			{
				Lepra::UnicodeString lTextureName;
				int lStrSize = Lepra::PackerUnicodeString::Unpack(&lTextureName, &lBuffer[lIndex], pSize-lIndex);
				lStrSize = (lStrSize+3)&(~3);
				lOk = (lStrSize <= (int)(pSize-2));
				lIndex += lStrSize;
				lMaterial.mTextureList.push_back(lTextureName);
			}
		}
		if (lOk)
		{
			int lStrSize = Lepra::PackerUnicodeString::Unpack(&lMaterial.mShaderName, &lBuffer[lIndex], pSize-lIndex);
			lStrSize = (lStrSize+3)&(~3);
			lOk = (lStrSize == (int)(pSize-lIndex));
			lIndex += lStrSize;
		}
		if (lOk)
		{
			lClass->SetLastMeshMaterial(lMaterial);
		}
		assert(lOk);
	}
	else
	{
		lOk = Parent::LoadElementCallback(pType, pSize, pChunkEndPosition, pStorage);
	}
	return (lOk);
}



}
