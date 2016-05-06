
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/uichunkyloader.h"
#include "../../lepra/include/packer.h"
#include "../include/uianimatedgeometry.h"
#include "../include/uichunkyclass.h"
#include "../include/uitrianglebasedgeometry.h"



namespace uitbc {



ChunkyMeshLoader::ChunkyMeshLoader(File* file, bool is_file_owner):
	tbc::ChunkyLoader(file, is_file_owner) {
}

ChunkyMeshLoader::~ChunkyMeshLoader() {
}

bool ChunkyMeshLoader::Load(TriangleBasedGeometry* mesh_data, int& casts_shadows) {
	bool ok = true;
	if (ok) {
		ok = VerifyFileType(tbc::kChunkMesh);
	}

	uint32* load_vertices = 0;
	unsigned vertices_size = 0;
	uint32* load_normals = 0;
	unsigned normals_size = 0;
	uint32* triangle_indices = 0;
	unsigned triangle_indices_size = 0;
	const int uv_count = 8;
	uint32* load_uvs[uv_count] = {0, 0, 0, 0, 0, 0, 0, 0};
	unsigned uvs_size[uv_count] = {0, 0, 0, 0, 0, 0, 0, 0};
	int32 uvs_per_vertex = 2;
	uint8* colors = 0;
	unsigned colors_size = 0;
	int32 color_format = 0x7FFFFFFD;
	int32 geometry_primitive = 0x7FFFFFFD;
	int32 geometry_volatility = 0x7FFFFFFD;
	int32 _casts_shadows = 0;
	int32 shadow_deviation_int = -1;
	int32 is_two_sided = 0;
	int32 recv_no_shadows = 0;
	if (ok) {
		tbc::ChunkyLoader::FileElementList load_list;
		// TRICKY: these have to be in the exact same order as when saved.
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshPrimitive, &geometry_primitive));
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshVolatility, &geometry_volatility));
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshCastsShadows, &_casts_shadows));
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshShadowDeviation, &shadow_deviation_int));
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshTwoSided, &is_two_sided));
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshRecvNoShadows, &recv_no_shadows));
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshVertices, (void**)&load_vertices, &vertices_size));
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshTriangles, (void**)&triangle_indices, &triangle_indices_size));
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshNormals, (void**)&load_normals, &normals_size));
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshUv, (void**)load_uvs, uvs_size, -uv_count));	// Specialcasing for array loading.
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshUvsPerVertex, &uvs_per_vertex));
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshColor, (void**)&colors, &colors_size));
		load_list.push_back(ChunkyFileElement(tbc::kChunkMeshColorFormat, &color_format));
		ok = AllocLoadChunkyList(load_list, file_->GetSize());
	}
	if (ok) {
		ok = (load_vertices != 0);
	}
	if (ok) {
		ok = !!triangle_indices;	// One and only one must be there.
	}
	if (ok) {
		if (colors) {
			ok = (color_format == tbc::GeometryBase::kColorRgb || color_format == tbc::GeometryBase::kColorRgba);
		}
	}
	if (ok) {
		ok = (geometry_primitive == tbc::GeometryBase::kTriangles ||
			geometry_primitive == tbc::GeometryBase::kQuads);
	}
	if (ok) {
		ok = (geometry_volatility == tbc::GeometryBase::kGeomStatic ||
			geometry_volatility == tbc::GeometryBase::kGeomSemiStatic ||
			geometry_volatility == tbc::GeometryBase::kGeomDynamic ||
			geometry_volatility == tbc::GeometryBase::kGeomVolatile);
	}
	deb_assert(ok);
	if (ok) {
		if (geometry_primitive == tbc::GeometryBase::kTriangles) {
			ok = (triangle_indices_size%(sizeof(uint32)*3) == 0);
		} else if (geometry_primitive == tbc::GeometryBase::kQuads) {
			ok = (triangle_indices_size%(sizeof(uint32)*4) == 0);
		} else {
			ok = false;
		}
	}
	if (ok) {
		ok = (vertices_size%(sizeof(float)*3) == 0 &&
			uv_count >= 0 &&
			uv_count <= 8);
	}
	if (ok) {
		ok = (uvs_per_vertex == 2 || uvs_per_vertex == 4);
	}
	deb_assert(ok);
	for (unsigned x = 0; ok && uvs_size[x] && x < uv_count; ++x) {
		ok = (uvs_size[x] == vertices_size*uvs_per_vertex/3);
	}
	deb_assert(ok);
	if (ok) {
		// TODO: add checks on normal and color sizes, so that we don't overrun buffers.
	}
	const unsigned index_count = triangle_indices_size / sizeof(uint32);
	const unsigned vertex_count = vertices_size / (sizeof(float)*3);
	uint32* indices = triangle_indices;
	float* vertices = (float*)load_vertices;
	float* normals = (float*)load_normals;
	float* uvs[uv_count] = {0, 0, 0, 0, 0, 0, 0, 0};
	if (ok) {
		// Convert to host endian.
		unsigned x;
		for (x = 0; x < index_count; ++x) {
			indices[x] = Endian::BigToHost(indices[x]);
		}
		for (x = 0; x < vertex_count*3; ++x) {
			vertices[x] = Endian::BigToHostF(load_vertices[x]);
		}
		if (load_normals) {
			for (x = 0; x < vertex_count*3; ++x) {
				normals[x] = Endian::BigToHostF(load_normals[x]);
			}
		}
		for (x = 0; load_uvs[x] && (int)x < uv_count; ++x) {
			uvs[x] = (float*)load_uvs[x];
			for (unsigned y = 0; y < vertex_count*uvs_per_vertex; ++y) {
				uvs[x][y] = Endian::BigToHostF(load_uvs[x][y]);
			}
		}
	}
	if (ok) {
		const tbc::GeometryBase::PrimitiveType _type = (tbc::GeometryBase::PrimitiveType)geometry_primitive;
		mesh_data->SetUVCountPerVertex(uvs_per_vertex);
		mesh_data->Set(vertices, normals, uvs[0], colors,
			(tbc::GeometryBase::ColorFormat)color_format, indices,
			vertex_count, index_count, _type,
			(tbc::GeometryBase::GeometryVolatility)geometry_volatility);

		for (int x = 1; uvs[x] && x < uv_count; ++x) {
			mesh_data->AddUVSet(uvs[x]);
		}

		casts_shadows = _casts_shadows;
		if (shadow_deviation_int >= 0) {
			float32 shadow_deviation = *(float*)&shadow_deviation_int;	// Already big2host'ed, as it izz single value.
			mesh_data->SetBigOrientationThreshold(shadow_deviation);
		}
		mesh_data->SetTwoSided(is_two_sided > 0);
		mesh_data->SetRecvNoShadows(recv_no_shadows > 0);
	}
	// TODO: reuse memory, don't new/delete constantly!
	delete[] (load_vertices);
	delete[] (load_normals);
	delete[] (triangle_indices);
	for (int x = 0; x < uv_count; ++x) {
		delete[] (load_uvs[x]);
	}
	delete[] (colors);
	deb_assert(ok);
	return (ok);
}

bool ChunkyMeshLoader::Save(const TriangleBasedGeometry* mesh_data, int casts_shadows) {
	// Write file header. We will come back to it later to re-write the actual size.
	bool ok = true;
	if (ok) {
		ok = WriteFileType(tbc::kChunkMesh);
	}
	int64 file_data_start = file_->Tell();

	// Initialize data and write the mesh itself, exluding vertex weights.
	unsigned vertices_size = mesh_data->GetVertexCount()*3*sizeof(float);
	const uint32* vertices = AllocInitBigEndian(mesh_data->GetVertexData(), vertices_size/sizeof(float));
	const float* save_normals = mesh_data->GetNormalData();
	unsigned normals_size = (save_normals? vertices_size : 0);
	const uint32* normals = AllocInitBigEndian(save_normals, normals_size/sizeof(float));
	unsigned triangle_indices_size = mesh_data->GetIndexCount()*sizeof(uint32);
	const uint32* triangle_indices = AllocInitBigEndian(mesh_data->GetIndexData(), triangle_indices_size/sizeof(uint32));
	const int uvs_per_vertex = mesh_data->GetUVCountPerVertex();
	const int uv_count = mesh_data->GetUVSetCount();
	const uint32* uvs[32];
	unsigned uvs_size[32];
	const unsigned fixed_uv_byte_size = mesh_data->GetVertexCount()*uvs_per_vertex*sizeof(float);
	for (int x = 0; x < uv_count; ++x) {
		uvs_size[x] = fixed_uv_byte_size;
		uvs[x] = AllocInitBigEndian(mesh_data->GetUVData(x), uvs_size[x]/sizeof(float));
	}
	const unsigned char* colors = mesh_data->GetColorData();
	unsigned colors_size = 0;
	if (colors) {
		if (mesh_data->GetColorFormat() == tbc::GeometryBase::kColorRgb) {
			colors_size = (mesh_data->GetVertexCount()*3+3)&(~3);
		} else {	// RGBA.
			colors_size = mesh_data->GetVertexCount()*4;
		}
	}
	int32 color_format = mesh_data->GetColorFormat();
	int32 geometry_primitive = mesh_data->GetPrimitiveType();
	int32 geometry_volatility = mesh_data->GetGeometryVolatility();
	int32 _casts_shadows = casts_shadows;
	int32 shadow_deviation = Endian::HostToBigF(mesh_data->GetBigOrientationThreshold());
	int32 is_two_sided = mesh_data->IsTwoSided()? 1 : 0;
	int32 recv_no_shadows = mesh_data->IsRecvNoShadows()? 1 : 0;
	if (ok) {
		tbc::ChunkyLoader::FileElementList save_list;
		save_list.push_back(ChunkyFileElement(tbc::kChunkMeshPrimitive, &geometry_primitive));
		save_list.push_back(ChunkyFileElement(tbc::kChunkMeshVolatility, &geometry_volatility));
		save_list.push_back(ChunkyFileElement(tbc::kChunkMeshCastsShadows, &_casts_shadows));
		save_list.push_back(ChunkyFileElement(tbc::kChunkMeshShadowDeviation, &shadow_deviation));
		save_list.push_back(ChunkyFileElement(tbc::kChunkMeshTwoSided, &is_two_sided));
		save_list.push_back(ChunkyFileElement(tbc::kChunkMeshRecvNoShadows, &recv_no_shadows));
		save_list.push_back(ChunkyFileElement(tbc::kChunkMeshVertices, (void**)&vertices, &vertices_size));
		if (triangle_indices) {
			save_list.push_back(ChunkyFileElement(tbc::kChunkMeshTriangles, (void**)&triangle_indices, &triangle_indices_size));
		}
		if (normals) {
			save_list.push_back(ChunkyFileElement(tbc::kChunkMeshNormals, (void**)&normals, &normals_size));
		}
		if (uv_count) {
			save_list.push_back(ChunkyFileElement(tbc::kChunkMeshUv, (void**)uvs, uvs_size, uv_count));
		}
		if (colors) {
			save_list.push_back(ChunkyFileElement(tbc::kChunkMeshColor, (void**)&colors, &colors_size));
			save_list.push_back(ChunkyFileElement(tbc::kChunkMeshColorFormat, &color_format));
		}
		ok = SaveChunkyList(save_list);
	}

	// Re-write file header size.
	if (ok) {
		uint32 _size = (uint32)(file_->Tell()-file_data_start);
		file_->SeekSet(file_data_start-4);
		ok = (file_->Write(_size) == kIoOk);
	}

	delete[] (vertices);
	delete[] (normals);
	delete[] (triangle_indices);
	for (int x = 0; x < uv_count; ++x) {
		delete[] (uvs[x]);
	}

	return (ok);
}



ChunkySkinLoader::ChunkySkinLoader(File* file, bool is_file_owner):
	tbc::ChunkyLoader(file, is_file_owner) {
}

ChunkySkinLoader::~ChunkySkinLoader() {
}

bool ChunkySkinLoader::Load(AnimatedGeometry* skin_data) {
	bool ok = true;
	if (ok) {
		ok = VerifyFileType(tbc::kChunkSkin);
	}
	if (ok) {
		tbc::ChunkyLoader::FileElementList load_list;
		load_list.push_back(ChunkyFileElement(tbc::kChunkSkinBoneWeightGroup, (void*)skin_data, -1000000));
		ok = AllocLoadChunkyList(load_list, file_->GetSize());
	}
	return (ok);
}

bool ChunkySkinLoader::Save(const AnimatedGeometry* skin_data) {
	// Write file header. We will come back to it later to re-write the actual size.
	bool ok = true;
	if (ok) {
		ok = WriteFileType(tbc::kChunkSkin);
	}
	int64 file_data_start = file_->Tell();
	if (ok) {
		ok = SaveBoneWeightChunkArray(skin_data);
	}
	// Re-write file header size.
	if (ok) {
		uint32 _size = (uint32)(file_->Tell()-file_data_start);
		file_->SeekSet(file_data_start-4);
		ok = (file_->Write(_size) == kIoOk);
	}
	return (ok);
}



bool ChunkySkinLoader::LoadElementCallback(tbc::ChunkyType type, uint32 size, int64 chunk_end_position, void* storage) {
	bool ok = false;
	if (type == tbc::kChunkSkinBoneWeightGroup) {
		// Setup pointers and counters for list loading.
		AnimatedGeometry::BoneWeightGroup weights;
		weights.bone_count_ = 0;
		int* bone_indices = 0;
		weights.vector_index_count_ = 0;
		weights.vector_index_array_ = 0;
		unsigned weight_count = 0;
		weights.vector_weight_array_ = 0;
		tbc::ChunkyLoader::FileElementList load_list;
		load_list.push_back(ChunkyFileElement(tbc::kChunkSkinBwgBones, (void**)&bone_indices, (unsigned*)&weights.bone_count_));
		load_list.push_back(ChunkyFileElement(tbc::kChunkSkinBwgVertices, (void**)&weights.vector_index_array_, (unsigned*)&weights.vector_index_count_));
		load_list.push_back(ChunkyFileElement(tbc::kChunkSkinBwgWeights, (void**)&weights.vector_weight_array_, &weight_count));
		ok = AllocLoadChunkyList(load_list, chunk_end_position);

		if (ok) {
			// Assert that number of index/weight bytes loaded are a multiple of 4 (i.e. uint32/float).
			ok = ((weights.bone_count_ & 3) == 0 &&
				(weights.vector_index_count_ & 3) == 0 &&
				(weight_count & 3) == 0);
			weights.bone_count_ /= sizeof(bone_indices[0]);
			weights.vector_index_count_ /= sizeof(weights.vector_index_array_[0]);
			weight_count /= sizeof(weights.vector_weight_array_[0]);
		}

		if (ok) {
			// Assert that the number of bones are in range and that the proportion between weights and vertices is correct.
			ok = (weights.bone_count_ >= 1 && weights.bone_count_ <= AnimatedGeometry::BoneWeightGroup::MAX_BONES_PER_VERTEX &&
				weights.vector_index_count_ >= 1 && ((int)weight_count) == weights.vector_index_count_*weights.bone_count_);
		}

		if (ok) {
			::memcpy(weights.bone_index_array_, bone_indices, weights.bone_count_*sizeof(weights.bone_index_array_[0]));
			AnimatedGeometry* animated_data = (AnimatedGeometry*)storage;
			animated_data->AddBoneWeights(weights);
		}

		if (!ok) {
			delete[] (weights.vector_index_array_);
			delete[] (weights.vector_weight_array_);
		}
		delete[] (bone_indices);	// Always goes, uses BoneWeightGroup uses aggregate array.
	} else {
		ok = ChunkyLoader::LoadElementCallback(type, size, chunk_end_position, storage);
	}
	return (ok);
}

bool ChunkySkinLoader::SaveBoneWeightChunkArray(const AnimatedGeometry* skin_data) {
	// We don't need to write file header or any of that junk, just straight to business.

	bool ok = (skin_data->GetBoneWeightGroupCount() >= 1);
	int64 chunk_start_position = file_->Tell();
	if (ok) {
		int64 _chunk_end_position = 0;
		ok = SaveHead(tbc::kChunkSkinBoneWeightGroup, 0, _chunk_end_position);
	}
	for (int x = 0; ok && x < skin_data->GetBoneWeightGroupCount(); ++x) {
		const AnimatedGeometry::BoneWeightGroup* bone_weight_data = &skin_data->GetBoneWeightGroup(x);
		uint32 bs = bone_weight_data->bone_count_*sizeof(uint32);
		uint32 vs = bone_weight_data->vector_index_count_*sizeof(uint32);
		uint32 ws = bone_weight_data->bone_count_*bone_weight_data->vector_index_count_*sizeof(uint32);
		tbc::ChunkyLoader::FileElementList save_list;
		const int* bone_index_array = bone_weight_data->bone_index_array_;	// TRICKY: intermediate pointer workaround for MSVC8 compiler bug.
		save_list.push_back(ChunkyFileElement(tbc::kChunkSkinBwgBones, (void**)&bone_index_array, &bs));
		save_list.push_back(ChunkyFileElement(tbc::kChunkSkinBwgVertices, (void**)&bone_weight_data->vector_index_array_, &vs));
		save_list.push_back(ChunkyFileElement(tbc::kChunkSkinBwgWeights, (void**)&bone_weight_data->vector_weight_array_, &ws));
		ok = SaveChunkyList(save_list);
	}
	// Re-write chunk size.
	if (ok) {
		int64 _chunk_end_position = file_->Tell();
		ok = (file_->SeekSet(chunk_start_position+4) == chunk_start_position+4);
		if (ok) {
			uint32 _size = (uint32)(_chunk_end_position-chunk_start_position)-8;
			ok = (file_->Write(_size) == kIoOk);
		}
		if (ok) {
			ok = (file_->SeekSet(_chunk_end_position) == _chunk_end_position);
		}
	}
	return (ok);
}



ChunkyClassLoader::ChunkyClassLoader(File* file, bool is_file_owner):
	Parent(file, is_file_owner) {
}

ChunkyClassLoader::~ChunkyClassLoader() {
}

void ChunkyClassLoader::AddLoadElements(Parent::FileElementList& element_list, tbc::ChunkyClass* data) {
	Parent::AddLoadElements(element_list, data);
	element_list.insert(--element_list.end(), ChunkyFileElement(tbc::kChunkClassMeshList, (void*)data, -1000));
}

bool ChunkyClassLoader::LoadElementCallback(tbc::ChunkyType type, uint32 size, int64 chunk_end_position, void* storage) {
	ChunkyClass* clazz = (ChunkyClass*)storage;
	bool ok = false;
	if (type == tbc::kChunkClassMeshList) {
		FileElementList load_list;
		load_list.push_back(ChunkyFileElement(tbc::kChunkClassPhysMesh, (void*)clazz));
		ok = AllocLoadChunkyList(load_list, chunk_end_position);
		deb_assert(ok);
	} else if (type == tbc::kChunkClassPhysMesh) {
		uint8* buffer = 0;
		ok = (file_->AllocReadData((void**)&buffer, size) == kIoOk);
		deb_assert(ok);
		int32 physics_index = -1;
		str mesh_base_name;
		int index = 0;
		if (ok) {
			physics_index = Endian::BigToHost(*(int32*)&buffer[index]);
			index += sizeof(physics_index);
			const size_t exclude_byte_count = (1+7+11)*4;	// Index+transform+colors.
			int str_size = PackerUnicodeString::Unpack(mesh_base_name, &buffer[index], size-exclude_byte_count);
			str_size = (str_size+3)&(~3);
			ok = (str_size < (int)(size-exclude_byte_count));
			deb_assert(ok);
			index += str_size;
		}
		if (ok) {
			const int transform_float_count = 7;
			float transform_array[transform_float_count];
			int x = 0;
			for (; x < transform_float_count; ++x) {
				transform_array[x] = Endian::BigToHostF(*(uint32*)&buffer[index+x*sizeof(float)]);
			}
			const float scale = Endian::BigToHostF(*(uint32*)&buffer[index+x*sizeof(float)]);
			index += (transform_float_count + 1) * sizeof(float);
			ok = (index < (int)(size - 12*sizeof(float) - 4));
			deb_assert(ok);
			clazz->AddMesh(physics_index, mesh_base_name, xform(transform_array), scale);
			clazz->AddPhysRoot(physics_index);
		}
		uitbc::ChunkyClass::Material material;
		if (ok) {
			const int material_float_count = 12;
			float material_array[material_float_count];
			for (int x = 0; x < material_float_count; ++x) {
				material_array[x] = Endian::BigToHostF(*(uint32*)&buffer[index+x*sizeof(float)]);
			}
			index += material_float_count * sizeof(float);
			ok = (index <= (int)(size-4-4-4));
			deb_assert(ok);
			material.ambient_.Set(material_array[0], material_array[1], material_array[2]);
			material.diffuse_.Set(material_array[3], material_array[4], material_array[5]);
			material.specular_.Set(material_array[6], material_array[7], material_array[8]);
			material.shininess_ = material_array[9];
			material.alpha_ = material_array[10];
			material.smooth_ = (material_array[11] > 0.5f);
		}
		if (ok) {
			Canvas::ResizeHint resize_hint = (Canvas::ResizeHint)Endian::BigToHost(*(int32*)&buffer[index]);
			index += sizeof(int32);
			ok = (index <= (int)(size-4-4));
			deb_assert(ok);
			material.resize_hint_ = resize_hint;
		}
		if (ok) {
			int32 texture_count = Endian::BigToHost(*(int32*)&buffer[index]);
			index += sizeof(texture_count);
			ok = (index <= (int)(size-2*texture_count-2));
			deb_assert(ok);
			for (int x = 0; ok && x < texture_count; ++x) {
				str texture_name;
				int str_size = PackerUnicodeString::Unpack(texture_name, &buffer[index], size-index);
				str_size = (str_size+3)&(~3);
				ok = (str_size <= (int)(size-2));
				deb_assert(ok);
				index += str_size;
				material.texture_list_.push_back(texture_name);
			}
		}
		if (ok) {
			int str_size = PackerUnicodeString::Unpack(material.shader_name_, &buffer[index], size-index);
			str_size = (str_size+3)&(~3);
			ok = (str_size == (int)(size-index));
			deb_assert(ok);
			index += str_size;
		}
		if (ok) {
			clazz->SetLastMeshMaterial(material);
		}
		deb_assert(ok);
		delete[] (buffer);
	} else {
		ok = Parent::LoadElementCallback(type, size, chunk_end_position, storage);
		deb_assert(ok);
	}
	return (ok);
}



}
