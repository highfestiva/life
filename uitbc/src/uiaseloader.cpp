/*
	Class:  ASELoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "pch.h"
#include "../../lepra/include/imageloader.h"
#include "../../lepra/include/canvas.h"
#include "../../lepra/include/diskfile.h"
#include "../../lepra/include/Params.h"
#include "../../lepra/include/hashtable.h"
#include "../../lepra/include/vector2d.h"
#include "../../lepra/include/vector3d.h"
#include "../include/uiaseloader.h"
#include "../include/uitexture.h"
#include "../include/uitrianglebasedgeometry.h"
#include <list>

namespace uitbc {

ASELoader::ASELoader() {
}


bool ASELoader::Load(ASEData& ase_data, const lepra::String& file_name) {
	bool ok = false;
	lepra::DiskFile file;
	if (file.Open(file_name, lepra::DiskFile::kModeRead) == true) {
		ok = Load(ase_data, file);
		file.Close();
	}

	return ok;
}

bool ASELoader::Load(ASEData& ase_data, lepra::Reader& reader) {
	ase_data.Init();

	//
	// Read the file into an AnsiString.
	//

	bool status_ok = true;
	unsigned int size = (unsigned int)reader.GetAvailable();
	char* data = new char[size + 1];
	status_ok = reader.ReadData(data, (unsigned int)reader.GetAvailable()) == lepra::kIoOk;
	data[size] = 0;

	lepra::AnsiString _data_string;
	if (status_ok == true) {
		_data_string = data;
	}
	delete[] data;

	//
	// Parse the file.
	//

	if (status_ok) {
		status_ok = Parse(ase_data, _data_string);
	}

	return status_ok;
}

bool ASELoader::Parse(ASEData& ase_data, const lepra::AnsiString& data_string) {
	//
	// Setup special tokens.
	//

	lepra::AnsiString _special_tokens;
	_special_tokens.AddToken("*");
	_special_tokens.AddToken("{");
	_special_tokens.AddToken("}");
	_special_tokens.AddToken("\"");
	_special_tokens.AddToken(",");

	static char* delimitors_ = " \t\v\b\r\n";

	int _index = 0;
	bool status_ok = true;
	bool done = false;
	lepra::AnsiString token;
	while (status_ok && !done) {
		_index = data_string.GetToken(token, _index, " \t\v\b\r\n", _special_tokens);
		done = _index < 0;
		status_ok = (token == "*") && _index > 0 || done;

		if (status_ok && !done) {
			_index = data_string.GetToken(token, _index, " \t\v\b\r\n", _special_tokens);
			if (token == "3DSMAX_ASCIIEXPORT")
				status_ok = ReadInt(ase_data.m3DSMaxAsciiExport_, data_string, _index, _special_tokens);
			else if(token == "COMMENT")
				status_ok = ReadQuotedString(ase_data.comment_, data_string, _index, _special_tokens);
			else if(token == "SCENE")
				status_ok = ReadSCENE(ase_data, data_string, _index, _special_tokens);
			else if(token == "MATERIAL_LIST")
				status_ok = ReadMATERIAL_LIST(ase_data, data_string, _index, _special_tokens);
			else if(token == "GEOMOBJECT") {
				GeomObject* _geom_obj = new GeomObject;
				status_ok = ReadGEOMOBJECT(_geom_obj, data_string, _index, _special_tokens);

				if (status_ok) {
					ase_data.geom_list_.PushBack(_geom_obj);
				} else {
					delete _geom_obj;
				}
			} else if(token == "CAMERAOBJECT") {
				CameraObject* _cam_obj = new CameraObject;
				status_ok = ReadCAMERAOBJECT(_cam_obj, data_string, _index, _special_tokens);

				if (status_ok) {
					ase_data.cam_list_.PushBack(_cam_obj);
				} else {
					delete _cam_obj;
				}
			} else if(token == "LIGHTOBJECT") {
				LightObject* _light_obj = new LightObject;
				status_ok = ReadLIGHTOBJECT(_light_obj, data_string, _index, _special_tokens);

				if (status_ok) {
					ase_data.light_list_.PushBack(_light_obj);
				} else {
					delete _light_obj;
				}
			} else
				status_ok = false;
		}
	}

	return status_ok;
}

bool ASELoader::ReadQuotedString(lepra::AnsiString& s, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the first quote.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "\"" && index > 0;
	}
	if (status_ok) {
		index = data_string.GetToken(token, index, "", special_tokens);
	}

	if (token == "\"") {
		s = "";
	} else {
		s = token;

		status_ok = index > 0;
		if (status_ok) {
			// Read the final quote.
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			status_ok = token == "\"";
		}
	}

	return status_ok;
}

bool ASELoader::ReadUnquotedString(lepra::AnsiString& s, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	if (status_ok) {
		index = data_string.GetToken(s, index, " \t\v\b\r\n:", special_tokens);
	}

	return status_ok;
}

bool ASELoader::ReadInt(int& _i, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;
	if (status_ok) {
		// Read the integer.
		lepra::AnsiString token;
		index = data_string.GetToken(token, index, " \t\v\b\r\n:", special_tokens);
		status_ok = lepra::AnsiStringUtility::StringToInt(token, _i);
	}
	return status_ok;
}

bool ASELoader::ReadIntVec(int* _i, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;
	if (status_ok)
		status_ok = ReadInt(_i[0], data_string, index, special_tokens);
	if (status_ok)
		status_ok = ReadInt(_i[1], data_string, index, special_tokens);
	if (status_ok)
		status_ok = ReadInt(_i[2], data_string, index, special_tokens);

	return status_ok;
}

bool ASELoader::ReadFloat(double& _f, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = (index > 0);
	if (status_ok) {
		// Read the float value.
		lepra::AnsiString token;
		index = data_string.GetToken(token, index, " \t\v\b\r\n:", special_tokens);
		status_ok = lepra::AnsiStringUtility::StringToDouble(token, _f);
	}
	return status_ok;
}

bool ASELoader::ReadFloatVec(double* _f, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;
	if (status_ok)
		status_ok = ReadFloat(_f[0], data_string, index, special_tokens);
	if (status_ok)
		status_ok = ReadFloat(_f[1], data_string, index, special_tokens);
	if (status_ok)
		status_ok = ReadFloat(_f[2], data_string, index, special_tokens);

	return status_ok;
}

bool ASELoader::ReadBool(bool& _bool, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	int __i = -1;
	bool status_ok = ReadInt(__i, data_string, index, special_tokens);
	if (status_ok) {
		switch(__i) {
		case 0:
			_bool = false;
			break;
		case 1:
			_bool = true;
			break;
		default:
			status_ok = false;
		}
	}

	return status_ok;
}

bool ASELoader::ReadVertexList(float* vertex, int num_vertex, char* vertex_identifier, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	if (status_ok) {
		int i;
		for (i = 0; status_ok && i < num_vertex; i++) {
			if (status_ok) {
				// Read the '*'.
				index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
				status_ok = token == "*" && index > 0;
			}

			if (status_ok) {
				index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
				status_ok = token == vertex_identifier && index > 0;
			}

			// Read vertex index. Must equal 'i'.
			int v_index = -1;
			if (status_ok) {
				status_ok = ReadInt(v_index, data_string, index, special_tokens);
			}

			if (status_ok) {
				status_ok = (v_index == i);
			}

			double _vertex[3];
			if (status_ok) {
				// Read the vector...
				status_ok = ReadFloatVec(_vertex, data_string, index, special_tokens);
			}

			if (status_ok) {
				// Type cast.
				vertex[v_index * 3 + 0] = (float)_vertex[0];
				vertex[v_index * 3 + 1] = (float)_vertex[1];
				vertex[v_index * 3 + 2] = (float)_vertex[2];
			}
		}
	}

	if (status_ok) {
		// Read the end brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "}" && index > 0;
	}

	return status_ok;
}

bool ASELoader::ReadFaceList(int* face, int num_faces, char* face_identifier, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	if (status_ok) {
		int i;
		for (i = 0; status_ok && i < num_faces; i++) {
			if (status_ok) {
				// Read the '*'.
				index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
				status_ok = token == "*" && index > 0;
			}

			if (status_ok) {
				index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
				status_ok = token == face_identifier && index > 0;
			}

			// Read face index. Must equal 'i'.
			int f_index = -1;
			if (status_ok) {
				status_ok = ReadInt(f_index, data_string, index, special_tokens);
			}

			if (status_ok) {
				status_ok = (f_index == i);
			}

			if (status_ok) {
				// Read the face...
				status_ok = ReadIntVec(&face[f_index * 3], data_string, index, special_tokens);
			}
		}
	}

	if (status_ok) {
		// Read the end brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "}" && index > 0;
	}

	return status_ok;
}

bool ASELoader::ReadSCENE(ASEData& ase_data, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	if (ase_data.scene_ == 0) {
		ase_data.scene_ = new Scene;
	} else {
		ase_data.scene_->Init();
	}

	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "SCENE_FILENAME")
				status_ok = ReadQuotedString(ase_data.scene_->filename_, data_string, index, special_tokens);
			else if(token == "COMMENT")
				status_ok = ReadQuotedString(ase_data.scene_->comment_, data_string, index, special_tokens);
			else if(token == "SCENE_FIRSTFRAME")
				status_ok = ReadInt(ase_data.scene_->first_frame_, data_string, index, special_tokens);
			else if(token == "SCENE_LASTFRAME")
				status_ok = ReadInt(ase_data.scene_->last_frame_, data_string, index, special_tokens);
			else if(token == "SCENE_FRAMESPEED")
				status_ok = ReadInt(ase_data.scene_->frame_speed_, data_string, index, special_tokens);
			else if(token == "SCENE_TICKSPERFRAME")
				status_ok = ReadInt(ase_data.scene_->ticks_per_frame_, data_string, index, special_tokens);
			else if(token == "SCENE_BACKGROUND_STATIC")
				status_ok = ReadFloatVec(ase_data.scene_->background_static_, data_string, index, special_tokens);
			else if(token == "SCENE_AMBIENT_STATIC")
				status_ok = ReadFloatVec(ase_data.scene_->ambient_static_, data_string, index, special_tokens);
			else
				status_ok = false;
		}
	}

	return status_ok;
}

bool ASELoader::ReadMATERIAL_LIST(ASEData& ase_data, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	if (status_ok) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;
	}
	if (status_ok && !done) {
		// Read tag name. Should be MATERIAL_COUNT.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "MATERIAL_COUNT" && index > 0;
	}

	int material_count_ = 0;
	if (status_ok && !done) {
		// Read the material count.
		status_ok = ReadInt(material_count_, data_string, index, special_tokens);
	}

	if (status_ok && !done) {
		for (int i = 0; i < material_count_ && status_ok; i++) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			status_ok = (token == "*") && index > 0;

			if (status_ok) {
				index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
				status_ok = (token == "MATERIAL") && index > 0;
			}

			int material_index = -1;
			if (status_ok) {
				status_ok = ReadInt(material_index, data_string, index, special_tokens);
			}
			if (status_ok) {
				status_ok = (material_index == i);
			}

			Material* _material = 0;
			if (status_ok) {
				_material = new Material;
				status_ok = ReadMATERIAL(_material, data_string, index, special_tokens);

				if (status_ok)
					ase_data.material_list_.PushBack(_material);
				else
					delete _material;
			}
		}
	}

	if (status_ok && !done) {
		// The block must end with a brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = (token == "}") && index > 0;
	}

	return status_ok;
}

bool ASELoader::ReadMATERIAL(Material* material, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "MATERIAL_NAME")
				status_ok = ReadQuotedString(material->name_, data_string, index, special_tokens);
			else if(token == "COMMENT")
				status_ok = ReadQuotedString(material->comment_, data_string, index, special_tokens);
			else if(token == "MATERIAL_CLASS")
				status_ok = ReadQuotedString(material->clazz_, data_string, index, special_tokens);
			else if(token == "MATERIAL_AMBIENT")
				status_ok = ReadFloatVec(material->ambient_, data_string, index, special_tokens);
			else if(token == "MATERIAL_DIFFUSE")
				status_ok = ReadFloatVec(material->diffuse_, data_string, index, special_tokens);
			else if(token == "MATERIAL_SPECULAR")
				status_ok = ReadFloatVec(material->specular_, data_string, index, special_tokens);
			else if(token == "MATERIAL_SHINE")
				status_ok = ReadFloat(material->shine_, data_string, index, special_tokens);
			else if(token == "MATERIAL_SHINESTRENGTH")
				status_ok = ReadFloat(material->shine_strength_, data_string, index, special_tokens);
			else if(token == "MATERIAL_TRANSPARENCY")
				status_ok = ReadFloat(material->transparency_, data_string, index, special_tokens);
			else if(token == "MATERIAL_WIRESIZE")
				status_ok = ReadFloat(material->wire_size_, data_string, index, special_tokens);
			else if(token == "MATERIAL_SHADING")
				status_ok = ReadUnquotedString(material->shading_, data_string, index, special_tokens);
			else if(token == "MATERIAL_XP_FALLOFF")
				status_ok = ReadFloat(material->xp_falloff_, data_string, index, special_tokens);
			else if(token == "MATERIAL_SELFILLUM")
				status_ok = ReadFloat(material->self_illum_, data_string, index, special_tokens);
			else if(token == "MATERIAL_FALLOFF")
				status_ok = ReadUnquotedString(material->falloff_, data_string, index, special_tokens);
			else if(token == "MATERIAL_XP_TYPE")
				status_ok = ReadUnquotedString(material->xp_type_, data_string, index, special_tokens);
			else if(token == "MAP_AMBIENT")
				status_ok = ReadMAP(material->ambient_map_list_, data_string, index, special_tokens);
			else if(token == "MAP_DIFFUSE")
				status_ok = ReadMAP(material->diffuse_map_list_, data_string, index, special_tokens);
			else if(token == "MAP_SPECULAR")
				status_ok = ReadMAP(material->specular_map_list_, data_string, index, special_tokens);
			else if(token == "MAP_SHINE")
				status_ok = ReadMAP(material->shine_map_list_, data_string, index, special_tokens);
			else if(token == "MAP_SHINESTRENGTH")
				status_ok = ReadMAP(material->shine_strength_map_list_, data_string, index, special_tokens);
			else if(token == "MAP_SELFILLUM")
				status_ok = ReadMAP(material->self_illum_map_list_, data_string, index, special_tokens);
			else if(token == "MAP_OPACITY")
				status_ok = ReadMAP(material->opacity_map_list_, data_string, index, special_tokens);
			else if(token == "MAP_FILTERCOLOR")
				status_ok = ReadMAP(material->filter_color_map_list_, data_string, index, special_tokens);
			else if(token == "MAP_BUMP")
				status_ok = ReadMAP(material->bump_map_list_, data_string, index, special_tokens);
			else if(token == "MAP_REFLECT")
				status_ok = ReadMAP(material->reflect_map_list_, data_string, index, special_tokens);
			else if(token == "MAP_REFRACT")
				status_ok = ReadMAP(material->refract_map_list_, data_string, index, special_tokens);
			else
				status_ok = false;
		}
	}
	return status_ok;
}

bool ASELoader::ReadMAP(std::list<Map*>& map_list, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	Map* map = new Map;

	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "MAP_NAME")
				status_ok = ReadQuotedString(map->name_, data_string, index, special_tokens);
			else if(token == "COMMENCT")
				status_ok = ReadQuotedString(map->comment_, data_string, index, special_tokens);
			else if(token == "MAP_CLASS")
				status_ok = ReadQuotedString(map->clazz_, data_string, index, special_tokens);
			else if(token == "MAP_SUBNO")
				status_ok = ReadInt(map->sub_no_, data_string, index, special_tokens);
			else if(token == "MAP_AMOUNT")
				status_ok = ReadFloat(map->amount_, data_string, index, special_tokens);
			else if(token == "MAP_TYPE")
				status_ok = ReadUnquotedString(map->type_, data_string, index, special_tokens);
			else if(token == "BITMAP")
				status_ok = ReadQuotedString(map->bitmap_path_, data_string, index, special_tokens);
			else if(token == "BITMAP_FILTER")
				status_ok = ReadUnquotedString(map->bitmap_filter_, data_string, index, special_tokens);
			else if(token == "UVW_U_OFFSET")
				status_ok = ReadFloat(map->u_offset_, data_string, index, special_tokens);
			else if(token == "UVW_V_OFFSET")
				status_ok = ReadFloat(map->v_offset_, data_string, index, special_tokens);
			else if(token == "UVW_U_TILING")
				status_ok = ReadFloat(map->u_tiling_, data_string, index, special_tokens);
			else if(token == "UVW_V_TILING")
				status_ok = ReadFloat(map->v_tiling_, data_string, index, special_tokens);
			else if(token == "UVW_ANGLE")
				status_ok = ReadFloat(map->uvw_angle_, data_string, index, special_tokens);
			else if(token == "UVW_BLUR")
				status_ok = ReadFloat(map->uvw_blur_, data_string, index, special_tokens);
			else if(token == "UVW_BLUR_OFFSET")
				status_ok = ReadFloat(map->uvw_blur_offset_, data_string, index, special_tokens);
			// Yeah, I know... "NOUSE" should be "NOISE". But don't correct this! It is correct.
			else if(token == "UVW_NOUSE_AMT")
				status_ok = ReadFloat(map->uvw_noise_amt_, data_string, index, special_tokens);
			else if(token == "UVW_NOISE_SIZE")
				status_ok = ReadFloat(map->uvw_noise_size_, data_string, index, special_tokens);
			else if(token == "UVW_NOISE_LEVEL")
				status_ok = ReadInt(map->uvw_noise_level_, data_string, index, special_tokens);
			else if(token == "UVW_NOISE_PHASE")
				status_ok = ReadFloat(map->uvw_noise_phase_, data_string, index, special_tokens);
			else
				status_ok = false;
		}
	}

	if (status_ok) {
		map_list.PushBack(map);
	} else {
		delete map;
	}

	return status_ok;
}

bool ASELoader::ReadGEOMOBJECT(GeomObject* geom_obj, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "NODE_NAME")
				status_ok = ReadQuotedString(geom_obj->node_name_, data_string, index, special_tokens);
			else if(token == "COMMENT")
				status_ok = ReadQuotedString(geom_obj->comment_, data_string, index, special_tokens);
			else if(token == "NODE_TM") {
				NodeTM* _node_tm = new NodeTM;
				status_ok = ReadNODE_TM(_node_tm, data_string, index, special_tokens);

				if (status_ok)
					geom_obj->node_tm_list_.PushBack(_node_tm);
				else
					delete _node_tm;
			} else if(token == "MESH") {
				Mesh* _mesh = new Mesh;
				status_ok = ReadMESH(_mesh, data_string, index, special_tokens);

				if (status_ok)
					geom_obj->mesh_list_.PushBack(_mesh);
				else
					delete _mesh;
			} else if(token == "PROP_MOTIONBLUR")
				status_ok = ReadBool(geom_obj->motion_blur_, data_string, index, special_tokens);
			else if(token == "PROP_CASTSHADOW")
				status_ok = ReadBool(geom_obj->cast_shadow_, data_string, index, special_tokens);
			else if(token == "PROP_RECVSHADOW")
				status_ok = ReadBool(geom_obj->recv_shadow_, data_string, index, special_tokens);
			else if(token == "TM_ANIMATION") {
				geom_obj->AllocTMAnimation();
				status_ok = ReadTM_ANIMATION(geom_obj->tm_animation_, data_string, index, special_tokens);
			} else if(token == "MATERIAL_REF")
				status_ok = ReadInt(geom_obj->material_ref_, data_string, index, special_tokens);
			else
				status_ok = false;
		}
	}
	return status_ok;
}

bool ASELoader::ReadCAMERAOBJECT(CameraObject* cam_obj, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "NODE_NAME")
				status_ok = ReadQuotedString(cam_obj->node_name_, data_string, index, special_tokens);
			else if(token == "COMMENT")
				status_ok = ReadQuotedString(cam_obj->comment_, data_string, index, special_tokens);
			else if(token == "CAMERA_TYPE")
				status_ok = ReadUnquotedString(cam_obj->camera_type_, data_string, index, special_tokens);
			else if(token == "NODE_TM") {
				NodeTM* _node_tm = new NodeTM;
				status_ok = ReadNODE_TM(_node_tm, data_string, index, special_tokens);
				if (status_ok)
					cam_obj->node_tm_list_.PushBack(_node_tm);
				else
					delete _node_tm;

			} else if(token == "CAMERA_SETTINGS") {
				cam_obj->AllocCameraSettings();
				status_ok = ReadCAMERA_SETTINGS(cam_obj->settings_, data_string, index, special_tokens);
			} else if(token == "TM_ANIMATION") {
				cam_obj->AllocTMAnimation();
				status_ok = ReadTM_ANIMATION(cam_obj->tm_animation_, data_string, index, special_tokens);
			} else
				status_ok = false;
		}
	}
	return status_ok;
}

bool ASELoader::ReadLIGHTOBJECT(LightObject* light_obj, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "NODE_NAME")
				status_ok = ReadQuotedString(light_obj->node_name_, data_string, index, special_tokens);
			else if(token == "COMMENT")
				status_ok = ReadQuotedString(light_obj->comment_, data_string, index, special_tokens);
			else if(token == "LIGHT_TYPE")
				status_ok = ReadUnquotedString(light_obj->light_type_, data_string, index, special_tokens);
			else if(token == "NODE_TM") {
				NodeTM* _node_tm = new NodeTM;
				status_ok = ReadNODE_TM(_node_tm, data_string, index, special_tokens);
				if (status_ok)
					light_obj->node_tm_list_.PushBack(_node_tm);
				else
					delete _node_tm;

			} else if(token == "LIGHT_SHADOWS")
				status_ok = ReadUnquotedString(light_obj->shadows_, data_string, index, special_tokens);
			else if(token == "LIGHT_USELIGHT")
				status_ok = ReadBool(light_obj->use_light_, data_string, index, special_tokens);
			else if(token == "LIGHT_SPOTSHAPE")
				status_ok = ReadUnquotedString(light_obj->spot_shape_, data_string, index, special_tokens);
			else if(token == "LIGHT_USEGLOBAL")
				status_ok = ReadBool(light_obj->use_global_, data_string, index, special_tokens);
			else if(token == "LIGHT_ABSMAPBIAS")
				status_ok = ReadBool(light_obj->abs_map_bias_, data_string, index, special_tokens);
			else if(token == "LIGHT_OVERSHOOT")
				status_ok = ReadBool(light_obj->over_shoot_, data_string, index, special_tokens);
			else if(token == "LIGHT_SETTINGS") {
				light_obj->AllocLightSettings();
				status_ok = ReadLIGHT_SETTINGS(light_obj->settings_, data_string, index, special_tokens);
			} else if(token == "TM_ANIMATION") {
				light_obj->AllocTMAnimation();
				status_ok = ReadTM_ANIMATION(light_obj->tm_animation_, data_string, index, special_tokens);
			} else
				status_ok = false;
		}
	}
	return status_ok;
}

bool ASELoader::ReadCAMERA_SETTINGS(CameraSettings* camera_settings, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "TIMEVALUE")
				status_ok = ReadInt(camera_settings->time_value_, data_string, index, special_tokens);
			else if(token == "CAMERA_NEAR")
				status_ok = ReadFloat(camera_settings->near_, data_string, index, special_tokens);
			else if(token == "CAMERA_FAR")
				status_ok = ReadFloat(camera_settings->far_, data_string, index, special_tokens);
			else if(token == "CAMERA_FOV")
				status_ok = ReadFloat(camera_settings->fov_, data_string, index, special_tokens);
			else if(token == "CAMERA_TDIST")
				status_ok = ReadFloat(camera_settings->t_dist_, data_string, index, special_tokens);
			else
				status_ok = false;
		}
	}
	return status_ok;
}

bool ASELoader::ReadLIGHT_SETTINGS(LightSettings* light_settings, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "TIMEVALUE")
				status_ok = ReadInt(light_settings->time_value_, data_string, index, special_tokens);
			else if(token == "LIGHT_COLOR")
				status_ok = ReadFloatVec(light_settings->color_, data_string, index, special_tokens);
			else if(token == "LIGHT_INTENS")
				status_ok = ReadFloat(light_settings->intens_, data_string, index, special_tokens);
			else if(token == "LIGHT_ASPECT")
				status_ok = ReadFloat(light_settings->aspect_, data_string, index, special_tokens);
			else if(token == "LIGHT_HOTSPOT")
				status_ok = ReadFloat(light_settings->hot_spot_, data_string, index, special_tokens);
			else if(token == "LIGHT_FALLOFF")
				status_ok = ReadFloat(light_settings->falloff_, data_string, index, special_tokens);
			else if(token == "LIGHT_TDIST")
				status_ok = ReadFloat(light_settings->t_dist_, data_string, index, special_tokens);
			else if(token == "LIGHT_MAPBIAS")
				status_ok = ReadFloat(light_settings->map_bias_, data_string, index, special_tokens);
			else if(token == "LIGHT_MAPRANGE")
				status_ok = ReadFloat(light_settings->map_range_, data_string, index, special_tokens);
			else if(token == "LIGHT_MAPSIZE")
				status_ok = ReadFloat(light_settings->map_size_, data_string, index, special_tokens);
			else if(token == "LIGHT_RAYBIAS")
				status_ok = ReadFloat(light_settings->ray_bias_, data_string, index, special_tokens);
			else
				status_ok = false;
		}
	}
	return status_ok;
}

bool ASELoader::ReadNODE_TM(NodeTM* node_tm, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "NODE_NAME")
				status_ok = ReadQuotedString(node_tm->node_name_, data_string, index, special_tokens);
			else if(token == "COMMENT")
				status_ok = ReadQuotedString(node_tm->comment_, data_string, index, special_tokens);
			else if(token == "INHERIT_POS")
				status_ok = ReadIntVec(node_tm->inherit_pos_, data_string, index, special_tokens);
			else if(token == "INHERIT_ROT")
				status_ok = ReadIntVec(node_tm->inherit_rot_, data_string, index, special_tokens);
			else if(token == "INHERIT_SCL")
				status_ok = ReadIntVec(node_tm->inherit_scl_, data_string, index, special_tokens);
			else if(token == "TM_ROW0")
				status_ok = ReadFloatVec(node_tm->tm_row0_, data_string, index, special_tokens);
			else if(token == "TM_ROW1")
				status_ok = ReadFloatVec(node_tm->tm_row1_, data_string, index, special_tokens);
			else if(token == "TM_ROW2")
				status_ok = ReadFloatVec(node_tm->tm_row2_, data_string, index, special_tokens);
			else if(token == "TM_ROW3")
				status_ok = ReadFloatVec(node_tm->tm_row3_, data_string, index, special_tokens);
			else if(token == "TM_POS")
				status_ok = ReadFloatVec(node_tm->tm_pos_, data_string, index, special_tokens);
			else if(token == "TM_ROTAXIS")
				status_ok = ReadFloatVec(node_tm->tm_rot_axis_, data_string, index, special_tokens);
			else if(token == "TM_ROTANGLE")
				status_ok = ReadFloat(node_tm->tm_rot_angle_, data_string, index, special_tokens);
			else if(token == "TM_SCALE")
				status_ok = ReadFloatVec(node_tm->tm_scale_, data_string, index, special_tokens);
			else if(token == "TM_SCALEAXIS")
				status_ok = ReadFloatVec(node_tm->tm_scale_axis_, data_string, index, special_tokens);
			else if(token == "TM_SCALEAXISANG")
				status_ok = ReadFloat(node_tm->tm_scale_axis_ang_, data_string, index, special_tokens);
			else
				status_ok = false;
		}
	}
	return status_ok;
}

bool ASELoader::ReadTM_ANIMATION(TMAnimation* tm_animation, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "NODE_NAME")
				status_ok = ReadQuotedString(tm_animation->node_name_, data_string, index, special_tokens);
			else if(token == "COMMENT")
				status_ok = ReadQuotedString(tm_animation->comment_, data_string, index, special_tokens);
			else if(token == "CONTROL_POS_TRACK")
				status_ok = ReadCONTROL_POS_TRACK(tm_animation, data_string, index, special_tokens);
			else if(token == "CONTROL_ROT_TRACK")
				status_ok = ReadCONTROL_ROT_TRACK(tm_animation, data_string, index, special_tokens);
			else
				status_ok = false;
		}
	}
	return status_ok;
}

bool ASELoader::ReadCONTROL_POS_TRACK(TMAnimation* tm_animation, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "CONTROL_POS_SAMPLE") {
				ControlPosSample* sample = new ControlPosSample;
				status_ok = ReadCONTROL_POS_SAMPLE(sample, data_string, index, special_tokens);
				if (status_ok) {
					tm_animation->control_pos_track_.PushBack(sample);
				} else {
					delete sample;
				}
			} else {
				status_ok = false;
			}
		}
	}
	return status_ok;
}

bool ASELoader::ReadCONTROL_ROT_TRACK(TMAnimation* tm_animation, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "CONTROL_ROT_SAMPLE") {
				ControlRotSample* sample = new ControlRotSample;
				status_ok = ReadCONTROL_ROT_SAMPLE(sample, data_string, index, special_tokens);
				if (status_ok) {
					tm_animation->control_rot_track_.PushBack(sample);
				} else {
					delete sample;
				}
			} else {
				status_ok = false;
			}
		}
	}
	return status_ok;
}

bool ASELoader::ReadCONTROL_POS_SAMPLE(ControlPosSample* control_pos_sample, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;
	if (status_ok)
		status_ok = ReadInt(control_pos_sample->time_value_, data_string, index, special_tokens);
	if (status_ok)
		status_ok = ReadFloat(control_pos_sample->pos_[0], data_string, index, special_tokens);
	if (status_ok)
		status_ok = ReadFloat(control_pos_sample->pos_[1], data_string, index, special_tokens);
	if (status_ok)
		status_ok = ReadFloat(control_pos_sample->pos_[2], data_string, index, special_tokens);
	return status_ok;
}

bool ASELoader::ReadCONTROL_ROT_SAMPLE(ControlRotSample* control_rot_sample, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;
	if (status_ok)
		status_ok = ReadInt(control_rot_sample->time_value_, data_string, index, special_tokens);
	if (status_ok)
		status_ok = ReadFloat(control_rot_sample->rot_[0], data_string, index, special_tokens);
	if (status_ok)
		status_ok = ReadFloat(control_rot_sample->rot_[1], data_string, index, special_tokens);
	if (status_ok)
		status_ok = ReadFloat(control_rot_sample->rot_[2], data_string, index, special_tokens);
	if (status_ok)
		status_ok = ReadFloat(control_rot_sample->rot_[3], data_string, index, special_tokens);
	return status_ok;
}

bool ASELoader::ReadMESH(Mesh* mesh, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "COMMENT")
				status_ok = ReadQuotedString(mesh->comment_, data_string, index, special_tokens);
			else if(token == "TIMEVALUE")
				status_ok = ReadInt(mesh->time_value_, data_string, index, special_tokens);
			else if(token == "MESH_NUMVERTEX")
				status_ok = ReadInt(mesh->num_vertex_, data_string, index, special_tokens);
			else if(token == "MESH_NUMFACES")
				status_ok = ReadInt(mesh->num_faces_, data_string, index, special_tokens);
			else if(token == "MESH_VERTEX_LIST") {
				mesh->AllocVertexArray();
				status_ok = ReadVertexList(mesh->vertex_, mesh->num_vertex_, "MESH_VERTEX", data_string, index, special_tokens);
			} else if(token == "MESH_FACE_LIST")
				status_ok = ReadMESH_FACE_LIST(mesh, data_string, index, special_tokens);
			else if(token == "MESH_NUMTVERTEX")
				status_ok = ReadInt(mesh->GetDefaultMappingChannel()->num_t_vertex_, data_string, index, special_tokens);
			else if(token == "MESH_TVERTLIST") {
				MappingChannel* mapping_channel = mesh->GetDefaultMappingChannel();
				mapping_channel->AllocVertexArray();
				status_ok = ReadVertexList(mapping_channel->t_vertex_, mapping_channel->num_t_vertex_, "MESH_TVERT", data_string, index, special_tokens);
			} else if(token == "MESH_NUMTVFACES")
				status_ok = ReadInt(mesh->GetDefaultMappingChannel()->num_tv_faces_, data_string, index, special_tokens);
			else if(token == "MESH_TFACELIST") {
				MappingChannel* mapping_channel = mesh->GetDefaultMappingChannel();
				mapping_channel->AllocFaceArray();
				status_ok = ReadFaceList(mapping_channel->tv_face_, mapping_channel->num_tv_faces_, "MESH_TFACE", data_string, index, special_tokens);
			} else if(token == "MESH_NUMCVERTEX")
				status_ok = ReadInt(mesh->num_c_vertex_, data_string, index, special_tokens);
			else if(token == "MESH_CVERTLIST") {
				mesh->AllocCVertexArray();
				status_ok = ReadVertexList(mesh->c_vertex_, mesh->num_c_vertex_, "MESH_VERTCOL", data_string, index, special_tokens);
			} else if(token == "MESH_NUMCVFACES")
				status_ok = ReadInt(mesh->num_cv_faces_, data_string, index, special_tokens);
			else if(token == "MESH_CFACELIST") {
				mesh->AllocCFaceArray();
				status_ok = ReadFaceList(mesh->c_face_, mesh->num_cv_faces_, "MESH_CFACE", data_string, index, special_tokens);
			} else if(token == "MESH_NORMALS")
				status_ok = ReadMESH_NORMALS(mesh, data_string, index, special_tokens);
			else if(token == "MESH_MAPPINGCHANNEL")
				status_ok = ReadMESH_MAPPINGCHANNEL(mesh, data_string, index, special_tokens);
			else
				status_ok = false;
		}
	}
	return status_ok;
}

bool ASELoader::ReadMESH_FACE_LIST(Mesh* mesh, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	if (status_ok) {
		mesh->AllocFaceArray();

		int i;
		for (i = 0; status_ok && i < mesh->num_faces_; i++) {
			if (status_ok) {
				// Read the '*'.
				index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
				status_ok = token == "*" && index > 0;
			}

			if (status_ok) {
				// Read 'MESH_FACE'.
				index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
				status_ok = token == "MESH_FACE" && index > 0;
			}

			// Read face index. Must equal 'i'.
			int f_index = -1;
			if (status_ok) {
				status_ok = ReadInt(f_index, data_string, index, special_tokens);
			}

			if (status_ok) {
				status_ok = (f_index == i);
			}

			if (status_ok) {
				// Read an 'A'.
				status_ok = ReadUnquotedString(token, data_string, index, special_tokens);
				if (status_ok) {
					status_ok = token == "A";
				}
			}

			if (status_ok) {
				// Read the first index.
				status_ok = ReadInt(mesh->face_[f_index * 3 + 0], data_string, index, special_tokens);
			}

			if (status_ok) {
				// Check constraints.
				status_ok = mesh->face_[f_index * 3 + 0] >= 0 &&
					      mesh->face_[f_index * 3 + 0] < mesh->num_faces_;
			}

			if (status_ok) {
				// Read a 'B'.
				status_ok = ReadUnquotedString(token, data_string, index, special_tokens);
				if (status_ok) {
					status_ok = token == "B";
				}
			}

			if (status_ok) {
				// Read the second index.
				status_ok = ReadInt(mesh->face_[f_index * 3 + 1], data_string, index, special_tokens);
			}

			if (status_ok) {
				// Check constraints.
				status_ok = mesh->face_[f_index * 3 + 1] >= 0 &&
					      mesh->face_[f_index * 3 + 1] < mesh->num_faces_;
			}

			if (status_ok) {
				// Read a 'C'.
				status_ok = ReadUnquotedString(token, data_string, index, special_tokens);
				if (status_ok) {
					status_ok = token == "C";
				}
			}

			if (status_ok) {
				// Read the third index.
				status_ok = ReadInt(mesh->face_[f_index * 3 + 2], data_string, index, special_tokens);
			}

			if (status_ok) {
				// Check constraints.
				status_ok = mesh->face_[f_index * 3 + 2] >= 0 &&
					      mesh->face_[f_index * 3 + 2] < mesh->num_faces_;
			}

			if (status_ok) {
				// Read 'AB'.
				status_ok = ReadUnquotedString(token, data_string, index, special_tokens);
				if (status_ok) {
					status_ok = token == "AB";
				}
			}

			if (status_ok) {
				status_ok = ReadBool(mesh->face_edge_visible_[f_index * 3 + 0], data_string, index, special_tokens);
			}

			if (status_ok) {
				// Read 'BC'.
				status_ok = ReadUnquotedString(token, data_string, index, special_tokens);
				if (status_ok) {
					status_ok = token == "BC";
				}
			}

			if (status_ok) {
				status_ok = ReadBool(mesh->face_edge_visible_[f_index * 3 + 1], data_string, index, special_tokens);
			}

			if (status_ok) {
				// Read 'CA'.
				status_ok = ReadUnquotedString(token, data_string, index, special_tokens);
				if (status_ok) {
					status_ok = token == "CA";
				}
			}

			if (status_ok) {
				status_ok = ReadBool(mesh->face_edge_visible_[f_index * 3 + 2], data_string, index, special_tokens);
			}

			if (status_ok) {
				// Read '*'.
				index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
				status_ok = token == "*" && index > 0;
			}

			if (status_ok) {
				// Read 'MESH_SMOOTHING'.
				index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
				status_ok = token == "MESH_SMOOTHING" && index > 0;
			}

			bool done = false;
			while (status_ok && !done) {
				int smoothing_group;
				status_ok = ReadInt(smoothing_group, data_string, index, special_tokens);

				if (status_ok) {
					mesh->smoothing_group_list_[i].PushBack(smoothing_group);

					// Read comma or '*'...
					index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
					done = token == "*";
					status_ok = (token == ",") || done && index > 0;
				}
			}

			if (status_ok) {
				// Read 'MESH_MTLID'.
				index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
				status_ok = token == "MESH_MTLID" && index > 0;
			}

			if (status_ok) {
				status_ok = ReadInt(mesh->sub_material_[i], data_string, index, special_tokens);
			}
		}
	}

	if (status_ok) {
		// Read the end brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "}" && index > 0;
	}

	return status_ok;
}

bool ASELoader::ReadMESH_NORMALS(Mesh* mesh, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	mesh->AllocNormalArrays();

	bool status_ok = index > 0;

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	if (status_ok) {
		// Normals are stored in a "per face"-order.
		for (int i = 0; status_ok && i < mesh->num_faces_; i++) {
			if (status_ok) {
				// Read '*'.
				index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
				status_ok = token == "*" && index > 0;
			}

			if (status_ok) {
				// Read 'MESH_FACENORMAL'.
				index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
				status_ok = token == "MESH_FACENORMAL" && index > 0;
			}

			int f_index = -1;
			if (status_ok) {
				// Read face index.
				status_ok = ReadInt(f_index, data_string, index, special_tokens);
			}

			if (status_ok) {
				// Check constraints.
				status_ok = f_index >= 0 && f_index < mesh->num_faces_;
			}

			double face_normal[3];
			if (status_ok) {
				// Read the face normal.
				status_ok = ReadFloatVec(face_normal, data_string, index, special_tokens);
			}

			if (status_ok) {
				// Type cast.
				mesh->face_normals_[f_index * 3 + 0] = (float)face_normal[0];
				mesh->face_normals_[f_index * 3 + 1] = (float)face_normal[1];
				mesh->face_normals_[f_index * 3 + 2] = (float)face_normal[2];
			}

			// Read the face's 3 vertex normals.
			// Alex: This redundancy is such a total waste of disk
			//       space. Well well... It's not my file format anyway.
			for (int j = 0; status_ok && j < 3; j++) {
				if (status_ok) {
					// Read '*'.
					index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
					status_ok = token == "*" && index > 0;
				}

				if (status_ok) {
					// Read 'MESH_VERTEXNORMAL'.
					index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
					status_ok = token == "MESH_VERTEXNORMAL" && index > 0;
				}

				int v_index = -1;
				if (status_ok) {
					// Read vertex index.
					status_ok = ReadInt(v_index, data_string, index, special_tokens);
				}

				if (status_ok) {
					// Check constraints.
					status_ok = v_index >= 0 && v_index < mesh->num_vertex_;
				}

				double vertex_normal[3];
				if (status_ok) {
					// Read the vertex normal.
					status_ok = ReadFloatVec(vertex_normal, data_string, index, special_tokens);
				}

				if (status_ok) {
					// Type cast.
					mesh->vertex_normals_[v_index * 3 + 0] = (float)vertex_normal[0];
					mesh->vertex_normals_[v_index * 3 + 1] = (float)vertex_normal[1];
					mesh->vertex_normals_[v_index * 3 + 2] = (float)vertex_normal[2];
				}
			}
		}
	}

	if (status_ok) {
		// Read the end brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "}" && index > 0;
	}

	return status_ok;
}

bool ASELoader::ReadMESH_MAPPINGCHANNEL(Mesh* mesh, const lepra::AnsiString& data_string, int& index, const lepra::AnsiString& special_tokens) {
	bool status_ok = index > 0;

	MappingChannel* mapping_channel = new MappingChannel;
	int mapping_channel_index;

	if (status_ok) {
		// Read the mapping channel index.
		status_ok = ReadInt(mapping_channel_index, data_string, index, special_tokens);
	}

	lepra::AnsiString token;
	if (status_ok) {
		// Read the start brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "{" && index > 0;
	}

	bool done = false;
	while (status_ok && !done) {
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		done = token == "}";
		status_ok = (token == "*") && index > 0 || done;

		if (status_ok && !done) {
			index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
			if (token == "COMMENT")
				status_ok = ReadQuotedString(mapping_channel->comment_, data_string, index, special_tokens);
			else if(token == "MESH_NUMTVERTEX")
				status_ok = ReadInt(mapping_channel->num_t_vertex_, data_string, index, special_tokens);
			else if(token == "MESH_TVERTLIST") {
				mapping_channel->AllocVertexArray();
				status_ok = ReadVertexList(mapping_channel->t_vertex_, mapping_channel->num_t_vertex_, "MESH_TVERT", data_string, index, special_tokens);
			} else if(token == "MESH_NUMTVFACES")
				status_ok = ReadInt(mapping_channel->num_tv_faces_, data_string, index, special_tokens);
			else if(token == "MESH_TFACELIST") {
				mapping_channel->AllocFaceArray();
				status_ok = ReadFaceList(mapping_channel->tv_face_, mapping_channel->num_tv_faces_, "MESH_TFACE", data_string, index, special_tokens);
			} else
				status_ok = false;
		}
	}

	if (status_ok) {
		// Read the end brace.
		index = data_string.GetToken(token, index, " \t\v\b\r\n", special_tokens);
		status_ok = token == "}" && index > 0;
	}

	if (status_ok) {
		mesh->mapping_channel_list_.PushBack(mapping_channel);
	} else {
		delete mapping_channel;
	}

	return status_ok;
}

// bool ASELoader::Save(const GeometryList& geometry_list, const lepra::AnsiString& file_name);






ASELoader::Scene::Scene() :
	first_frame_(0),
	last_frame_(0),
	frame_speed_(0),
	ticks_per_frame_(0) {
	background_static_[0] = 0;
	background_static_[1] = 0;
	background_static_[2] = 0;
	ambient_static_[0] = 0;
	ambient_static_[1] = 0;
	ambient_static_[2] = 0;
}

ASELoader::Scene::~Scene() {
}

void ASELoader::Scene::Init() {
	filename_ = "";
	comment_ = "";
	first_frame_ = 0;
	last_frame_ = 0;
	frame_speed_ = 0;
	ticks_per_frame_ = 0;
	background_static_[0] = 0;
	background_static_[1] = 0;
	background_static_[2] = 0;
	ambient_static_[0] = 0;
	ambient_static_[1] = 0;
	ambient_static_[2] = 0;
}



ASELoader::Map::Map() :
	sub_no_(0),
	amount_(0),
	u_offset_(0),
	v_offset_(0),
	u_tiling_(0),
	v_tiling_(0),
	uvw_angle_(0),
	uvw_blur_(0),
	uvw_blur_offset_(0),
	uvw_noise_amt_(0),
	uvw_noise_size_(0),
	uvw_noise_level_(0),
	uvw_noise_phase_(0) {
}

ASELoader::Map::~Map() {
}

void ASELoader::Map::Init() {
	name_ = "";
	clazz_ = "";
	comment_ = "";
	sub_no_ = 0;
	amount_ = 0;
	bitmap_path_ = "";
	type_ = "";
	u_offset_ = 0;
	v_offset_ = 0;
	u_tiling_ = 0;
	v_tiling_ = 0;
	uvw_angle_ = 0;
	uvw_blur_ = 0;
	uvw_blur_offset_ = 0;
	uvw_noise_amt_ = 0;
	uvw_noise_size_ = 0;
	uvw_noise_level_ = 0;
	uvw_noise_phase_ = 0;
	bitmap_filter_ = "";
}



ASELoader::Material::Material() :
	shine_(0),
	shine_strength_(0),
	transparency_(0),
	wire_size_(0),
	xp_falloff_(0),
	self_illum_(0) {
	ambient_[0] = 0;
	ambient_[1] = 0;
	ambient_[2] = 0;
	diffuse_[0] = 0;
	diffuse_[1] = 0;
	diffuse_[2] = 0;
	specular_[0] = 0;
	specular_[1] = 0;
	specular_[2] = 0;
}

ASELoader::Material::~Material() {
	Init();
}

void ASELoader::Material::Init() {
	name_ = "";
	clazz_ = "";
	comment_ = "";
	shine_ = 0;
	shine_strength_ = 0;
	transparency_ = 0;
	wire_size_ = 0;
	shading_ = "";
	xp_falloff_ = 0;
	self_illum_ = 0;
	falloff_ = "";
	xp_type_ = "";

	ambient_map_list_.DeleteAll();
	diffuse_map_list_.DeleteAll();
	specular_map_list_.DeleteAll();
	shine_map_list_.DeleteAll();
	shine_strength_map_list_.DeleteAll();
	self_illum_map_list_.DeleteAll();
	opacity_map_list_.DeleteAll();
	filter_color_map_list_.DeleteAll();
	bump_map_list_.DeleteAll();
	reflect_map_list_.DeleteAll();
	refract_map_list_.DeleteAll();

	sub_material_list_.DeleteAll();
}

bool ASELoader::Material::LoadAsTexture(Texture& texture) {
	lepra::ImageLoader loader;
	lepra::Canvas color_map;
	lepra::Canvas alpha_map;
	lepra::Canvas normal_map;
	lepra::Canvas specular_map;
	lepra::Canvas* alpha_map = 0;
	lepra::Canvas* normal_map = 0;
	lepra::Canvas* specular_map = 0;

	bool status_ok = !diffuse_map_list_.IsEmpty();

	if (status_ok) {
		Map* diffuse_map = *diffuse_map_list_.First();
		status_ok = loader.Load(diffuse_map->bitmap_path_.ToCurrentCode(), color_map);
	}

	if (status_ok && opacity_map_list_.IsEmpty() == false) {
		Map* opacity_map = *opacity_map_list_.First();
		status_ok = loader.Load(opacity_map->bitmap_path_.ToCurrentCode(), alpha_map);
		if (status_ok) {
			alpha_map.ConvertToGrayscale();
			alpha_map = &alpha_map;
		}
	}

	if (status_ok && bump_map_list_.IsEmpty() == false) {
		// TODO: Verify that the map is a normal map, and not a bump map.
		Map* bump_map = *bump_map_list_.First();
		status_ok = loader.Load(bump_map->bitmap_path_.ToCurrentCode(), normal_map);
		if (status_ok) {
			normal_map = &normal_map;
		}
	}

	if (status_ok && shine_map_list_.IsEmpty() == false) {
		Map* shine_map = *shine_map_list_.First();
		status_ok = loader.Load(shine_map->bitmap_path_.ToCurrentCode(), specular_map);
		if (status_ok) {
			specular_map.ConvertToGrayscale();
			specular_map = &specular_map;
		}
	}

	if (status_ok) {
		texture.Set(color_map, alpha_map, normal_map, specular_map);
	}

	return status_ok;
}

Renderer::MaterialType ASELoader::Material::GetRecommendedMaterial() {
	bool diffuse  = !diffuse_map_list_.IsEmpty();
	bool specular = !shine_map_list_.IsEmpty();
	bool alpha    = !opacity_map_list_.IsEmpty();
	bool bump     = !bump_map_list_.IsEmpty();
	bool light    = !self_illum_map_list_.IsEmpty() || !ambient_map_list_.IsEmpty();

	Renderer::MaterialType material_type = Renderer::kMatSingleColorSolid;

	if (!diffuse) {
		// Single color or vertex color.
	} else if(bump && specular) {
		material_type = Renderer::kMatTextureSbmapPxs;
	} else if(bump && !specular) {
		material_type = Renderer::kMatTextureAndDiffuseBumpmapPxs;
	} else if(alpha) {
		material_type = Renderer::kMatSingleTextureBlended;
	} else if(light) {
		if (shading_ == "Phong") {
			material_type = Renderer::kMatTextureAndLightmapPxs;
		} else {
			material_type = Renderer::kMatTextureAndLightmap;
		}
	} else {
		if (shading_ == "Phong") {
			material_type = Renderer::kMatSingleTextureSolidPxs;
		} else {
			material_type = Renderer::kMatSingleTextureSolid;
		}
	}

	return material_type;
}



ASELoader::NodeTM::NodeTM() {
	Init();
}

ASELoader::NodeTM::~NodeTM() {
}

void ASELoader::NodeTM::Init() {
	node_name_ = "";
	comment_ = "";
	tm_rot_angle_ = 0;
	inherit_pos_[0] = 0;
	inherit_pos_[1] = 0;
	inherit_pos_[2] = 0;
	inherit_rot_[0] = 0;
	inherit_rot_[1] = 0;
	inherit_rot_[2] = 0;
	inherit_scl_[0] = 0;
	inherit_scl_[1] = 0;
	inherit_scl_[2] = 0;
	tm_row0_[0] = 1;
	tm_row0_[1] = 0;
	tm_row0_[2] = 0;
	tm_row1_[0] = 0;
	tm_row1_[1] = 1;
	tm_row1_[2] = 0;
	tm_row2_[0] = 0;
	tm_row2_[1] = 0;
	tm_row2_[2] = 1;
	tm_row3_[0] = 0;
	tm_row3_[1] = 0;
	tm_row3_[2] = 0;
	tm_pos_[0] = 0;
	tm_pos_[1] = 0;
	tm_pos_[2] = 0;
	tm_rot_axis_[0] = 0;
	tm_rot_axis_[1] = 0;
	tm_rot_axis_[2] = 0;
	tm_scale_[0] = 1;
	tm_scale_[1] = 1;
	tm_scale_[2] = 1;
	tm_scale_axis_[0] = 0;
	tm_scale_axis_[1] = 0;
	tm_scale_axis_[2] = 0;
	tm_scale_axis_ang_ = 0;
}

void ASELoader::NodeTM::GetRotationMatrix(lepra::RotationMatrixF& rot_mtx) {
	// TODO: Verify this. We may need to take the transpose instead.
	rot_mtx.Set((float)tm_row0_[0], (float)tm_row0_[1], (float)tm_row0_[2],
		     (float)tm_row1_[0], (float)tm_row1_[1], (float)tm_row1_[2],
		     (float)tm_row2_[0], (float)tm_row2_[1], (float)tm_row2_[2]);
}

void ASELoader::NodeTM::GetRotationMatrix(lepra::RotationMatrixD& rot_mtx) {
	// TODO: Verify this. We may need to take the transpose instead.
	rot_mtx.Set(tm_row0_[0], tm_row0_[1], tm_row0_[2],
		     tm_row1_[0], tm_row1_[1], tm_row1_[2],
		     tm_row2_[0], tm_row2_[1], tm_row2_[2]);
}

void ASELoader::NodeTM::GetPosition(lepra::Vector3DF& pos) {
	pos.Set((float)tm_pos_[0], (float)tm_pos_[1], (float)tm_pos_[2]);
}

void ASELoader::NodeTM::GetPosition(lepra::Vector3DD& pos) {
	pos.Set(tm_pos_[0], tm_pos_[1], tm_pos_[2]);
}

void ASELoader::NodeTM::GetTransformation(lepra::TransformationF& transform) {
	// Since transform.GetPosition() returns a reference, the following works.
	GetPosition(transform.GetPosition());

	lepra::RotationMatrixF _rot_mtx;
	GetRotationMatrix(_rot_mtx);
	_rot_mtx.Reorthogonalize();
	transform.SetOrientation(_rot_mtx);
}

void ASELoader::NodeTM::GetTransformation(lepra::TransformationD& transform) {
	// Since transform.GetPosition() returns a reference, the following works.
	GetPosition(transform.GetPosition());

	lepra::RotationMatrixD _rot_mtx;
	GetRotationMatrix(_rot_mtx);
	_rot_mtx.Reorthogonalize();
	transform.SetOrientation(_rot_mtx);
}




ASELoader::ControlPosSample::ControlPosSample() :
	time_value_(0) {
	pos_[0] = 0;
	pos_[1] = 0;
	pos_[2] = 0;
}

ASELoader::ControlPosSample::~ControlPosSample() {
}

void ASELoader::ControlPosSample::Init() {
	time_value_ = 0;
	pos_[0] = 0;
	pos_[1] = 0;
	pos_[2] = 0;
}

ASELoader::ControlRotSample::ControlRotSample() :
	time_value_(0) {
	rot_[0] = 0;
	rot_[1] = 0;
	rot_[2] = 0;
	rot_[3] = 0;
}

ASELoader::ControlRotSample::~ControlRotSample() {
}

void ASELoader::ControlRotSample::Init() {
	time_value_ = 0;
	rot_[0] = 0;
	rot_[1] = 0;
	rot_[2] = 0;
	rot_[3] = 0;
}

ASELoader::TMAnimation::TMAnimation() {
}

ASELoader::TMAnimation::~TMAnimation() {
	Init();
}

void ASELoader::TMAnimation::Init() {
	node_name_ = "";
	comment_ = "";
	control_pos_track_.DeleteAll();
	control_rot_track_.DeleteAll();
}



ASELoader::MappingChannel::MappingChannel() :
	comment_(""),
	num_t_vertex_(0),
	t_vertex_(0),
	num_tv_faces_(0),
	tv_face_(0) {
}

ASELoader::MappingChannel::~MappingChannel() {
	Init();
}

void ASELoader::MappingChannel::Init() {
	comment_ = "";
	num_t_vertex_ = 0;
	if (t_vertex_ != 0) {
		delete[] t_vertex_;
		t_vertex_ = 0;
	}
	num_tv_faces_ = 0;
	if (tv_face_ != 0) {
		delete[] tv_face_;
		tv_face_ = 0;
	}
}

void ASELoader::MappingChannel::AllocVertexArray() {
	if (t_vertex_ != 0) {
		delete[] t_vertex_;
		t_vertex_ = 0;
	}

	if (num_t_vertex_ > 0) {
		t_vertex_ = new float[num_t_vertex_ * 3];
	}
}

void ASELoader::MappingChannel::AllocFaceArray() {
	if (tv_face_ != 0) {
		delete[] tv_face_;
		tv_face_ = 0;
	}

	if (num_tv_faces_ > 0) {
		tv_face_ = new int[num_tv_faces_ * 3];
	}
}



ASELoader::Mesh::Mesh() :
	comment_(""),
	time_value_(0),
	num_vertex_(0),
	num_faces_(0),
	vertex_(0),
	face_(0),
	face_edge_visible_(0),
	smoothing_group_list_(0),
	sub_material_(0),
	have_default_uv_set_(false),
	face_normals_(0),
	vertex_normals_(0),
	num_c_vertex_(0),
	c_vertex_(0),
	num_cv_faces_(0),
	c_face_(0) {
}

ASELoader::Mesh::~Mesh() {
	Init();
}

void ASELoader::Mesh::Init() {
	comment_ = "";
	time_value_ = 0;
	num_vertex_ = 0;
	num_faces_ = 0;
	have_default_uv_set_ = false;

	if (vertex_ != 0) {
		delete[] vertex_;
		vertex_ = 0;
	}
	if (face_ != 0) {
		delete[] face_;
		face_ = 0;
	}
	if (face_edge_visible_ != 0) {
		delete[] face_edge_visible_;
		face_edge_visible_ = 0;
	}
	if (smoothing_group_list_ != 0) {
		delete[] smoothing_group_list_;
		smoothing_group_list_ = 0;
	}
	if (sub_material_ != 0) {
		delete[] sub_material_;
		sub_material_ = 0;
	}
	if (face_normals_ != 0) {
		delete[] face_normals_;
		face_normals_ = 0;
	}
	if (vertex_normals_ != 0) {
		delete[] vertex_normals_;
		vertex_normals_ = 0;
	}
	num_c_vertex_ = 0;
	if (c_vertex_ != 0) {
		delete[] c_vertex_;
		c_vertex_ = 0;
	}
	num_cv_faces_ = 0;
	if (c_face_ != 0) {
		delete[] c_face_;
		c_face_ = 0;
	}

	mapping_channel_list_.DeleteAll();
}

ASELoader::MappingChannel* ASELoader::Mesh::GetDefaultMappingChannel() {
	MappingChannel* mapping_channel;
	if (mapping_channel_list_.IsEmpty() || have_default_uv_set_ == false) {
		mapping_channel = new MappingChannel;
		mapping_channel_list_.PushFront(mapping_channel);
		have_default_uv_set_ = true;
	} else {
		mapping_channel = *mapping_channel_list_.First();
	}
	return mapping_channel;
}

void ASELoader::Mesh::AllocVertexArray() {
	if (vertex_ != 0) {
		delete[] vertex_;
		vertex_ = 0;
	}

	if (num_vertex_ > 0) {
		vertex_ = new float[num_vertex_ * 3];
	}
}

void ASELoader::Mesh::AllocFaceArray() {
	if (face_ != 0) {
		delete[] face_;
		face_ = 0;
	}

	if (face_edge_visible_ != 0) {
		delete[] face_edge_visible_;
		face_edge_visible_ = 0;
	}

	if (smoothing_group_list_ != 0) {
		delete[] smoothing_group_list_;
		smoothing_group_list_ = 0;
	}

	if (sub_material_ != 0) {
		delete[] sub_material_;
		sub_material_ = 0;
	}

	if (num_faces_ > 0) {
		face_ = new int[num_faces_ * 3];
		face_edge_visible_ = new bool[num_faces_ * 3];
		smoothing_group_list_ = new std::list<int>[num_faces_];
		sub_material_ = new int[num_faces_];
	}
}

void ASELoader::Mesh::AllocCVertexArray() {
	if (c_vertex_ != 0) {
		delete[] c_vertex_;
		c_vertex_ = 0;
	}

	if (num_c_vertex_ > 0) {
		c_vertex_ = new float[num_c_vertex_ * 3];
	}
}

void ASELoader::Mesh::AllocCFaceArray() {
	if (c_face_ != 0) {
		delete[] c_face_;
		c_face_ = 0;
	}

	if (num_cv_faces_ > 0) {
		c_face_ = new int[num_cv_faces_ * 3];
	}
}

void ASELoader::Mesh::AllocNormalArrays() {
	if (face_normals_ != 0) {
		delete[] face_normals_;
		face_normals_ = 0;
	}

	if (vertex_normals_ != 0) {
		delete[] vertex_normals_;
		vertex_normals_ = 0;
	}

	if (num_faces_ > 0) {
		face_normals_ = new float[num_faces_ * 3];
	}

	if (num_vertex_ > 0) {
		vertex_normals_ = new float[num_vertex_ * 3];
	}
}

void ASELoader::Mesh::GetGeometry(std::list<TriangleBasedGeometry*>& geometry_list) {
	int _num_uv_sets = mapping_channel_list_.GetCount();

	Face* _face = new Face[num_faces_];
	FaceListTable _face_list_table;
	SetupFaceListTable(_face, _face_list_table);

	// For each submaterial, build one TriangleBasedGeometry.
	FaceListTable::Iterator iter;
	for (iter = _face_list_table.First(); iter != _face_list_table.End(); ++iter) {
		// Create the vertex lookup table. This table maps a FaceVertex (the "old" vertex) to
		// an index into the newly created vertex list (see further down).
		typedef lepra::HashTable<FaceVertex, int, FaceVertex> VertexLookupTable;
		VertexLookupTable vertex_lookup_table_;

		FaceList* face_list = *iter;

		// The vertex list, vertex color list and lists for all UV-sets.
		std::list<lepra::Vector3DF> vertex_list;
		std::list<lepra::Vector3DF> vertex_color_list;
		std::list<lepra::Vector2DF>* uv_list = 0;

		if (_num_uv_sets > 0) {
			uv_list = new std::list<lepra::Vector2DF>[_num_uv_sets];
		}

		lepra::uint32* _index = new lepra::uint32[face_list->GetCount() * 3];

		FaceList::Iterator face_iter;
		int _face = 0;
		for (face_iter = face_list->First(); face_iter != face_list->End(); ++face_iter, ++_face) {
			Face* _face = *face_iter;

			// For each of the face's vertices.
			int i;
			for (i = 0; i < 3; i++) {
				// Check if we have a new vertex.
				VertexLookupTable::Iterator v_iter = vertex_lookup_table_.Find(_face->vertex_[i]);

				if (v_iter == vertex_lookup_table_.End()) {
					// This is a new vertex. Update the lookup table and store it.
					vertex_lookup_table_.Insert(_face->vertex_[i], vertex_list.GetCount());

					int v_index = _face->vertex_[i].v_index_;

					lepra::Vector3DF _vertex(vertex_[v_index * 3 + 0],
								  vertex_[v_index * 3 + 1],
								  vertex_[v_index * 3 + 2]);
					vertex_list.PushBack(_vertex);

					// Store vertex color.
					if (num_cv_faces_ > 0) {
						int c_index = _face->vertex_[i].c_index_;
						lepra::Vector3DF vertex_color(c_vertex_[c_index * 3 + 0],
									       c_vertex_[c_index * 3 + 1],
									       c_vertex_[c_index * 3 + 2]);
						vertex_color_list.PushBack(vertex_color);
					}

					// Store UV coordinates.
					MappingChannelList::Iterator m_iter = mapping_channel_list_.First();
					for (int j = 0; j < _num_uv_sets; ++j, ++m_iter) {
						MappingChannel* mapping_channel = *m_iter;
						int t_index = _face->vertex_[i].t_index_[j];
						lepra::Vector2DF uv_coords(mapping_channel->t_vertex_[t_index * 3 + 0],
									    mapping_channel->t_vertex_[t_index * 3 + 1]);
						uv_list[j].PushBack(uv_coords);
					}
				}

				_index[_face * 3 + i] = *vertex_lookup_table_.Find(_face->vertex_[i]);
			}
		}

		// Finally, setup the necessary arrays and create a TriangleBasedGeometry.
		float* _vertex = new float[vertex_list.GetCount() * 3];
		std::list<lepra::Vector3DF>::Iterator v_iter;
		int i;
		for (v_iter = vertex_list.First(), i = 0; v_iter != vertex_list.End(); ++v_iter, i+=3) {
			_vertex[i + 0] = (*v_iter).x;
			_vertex[i + 1] = (*v_iter).y;
			_vertex[i + 2] = (*v_iter).z;
		}

		lepra::uint8* vertex_color = 0;
		if (vertex_color_list.IsEmpty() == false) {
			vertex_color = new lepra::uint8[vertex_color_list.GetCount() * 3];
			for (v_iter = vertex_color_list.First(), i = 0; v_iter != vertex_color_list.End(); ++v_iter, i+=3) {
				vertex_color[i + 0] = (lepra::uint8)((*v_iter).x * 255.0f);
				vertex_color[i + 1] = (lepra::uint8)((*v_iter).y * 255.0f);
				vertex_color[i + 2] = (lepra::uint8)((*v_iter).z * 255.0f);
			}
		}

		float** uv_data = 0;
		if (_num_uv_sets > 0) {
			uv_data = new float*[_num_uv_sets];
			for (i = 0; i < _num_uv_sets; i++) {
				uv_data[i] = new float[uv_list[i].GetCount() * 2];
				std::list<lepra::Vector2DF>::Iterator uv_iter;
				int j;
				for (uv_iter = uv_list[i].First(), j = 0; uv_iter != uv_list[i].End(); ++uv_iter, j+=2) {
					uv_data[i][j + 0] = (*uv_iter).x;
					uv_data[i][j + 1] = (*uv_iter).y;
				}
			}
		}

		float* uv_data = 0;
		if (uv_data != 0)
			uv_data = uv_data[0];
		TriangleBasedGeometry* geometry = new TriangleBasedGeometry(_vertex, 0, uv_data, vertex_color, tbc::GeometryBase::kColorRgb, _index, vertex_list.GetCount(), face_list->GetCount(), tbc::GeometryBase::kTriangles, tbc::GeometryBase::kGeomStatic);

		for (i = 1; i < _num_uv_sets; i++) {
			geometry->AddUVSet(uv_data[i]);
		}

		geometry_list.PushBack(geometry);

		delete[] _vertex;
		delete[] _index;
		if (vertex_color != 0) {
			delete[] vertex_color;
		}
		if (_num_uv_sets > 0) {
			for (i = 0; i < _num_uv_sets; i++) {
				delete[] uv_data[i];
			}
			delete[] uv_data;
		}

		if (uv_list != 0) {
			delete[] uv_list;
		}
		delete face_list;
	}

	delete[] _face;
}

void ASELoader::Mesh::SetupFaceListTable(Face* face, FaceListTable& face_list_table) {
	// Setup all faces and put them in the list that corresponds to their material.
	int i;
	int _num_uv_sets = mapping_channel_list_.GetCount();
	for (i = 0; i < num_faces_; i++) {
		face[i].vertex_[0].v_index_ = face_[i * 3 + 0];
		face[i].vertex_[1].v_index_ = face_[i * 3 + 1];
		face[i].vertex_[2].v_index_ = face_[i * 3 + 2];
		face[i].sub_material_ = sub_material_[i];

		if (num_cv_faces_ > 0) {
			face[i].vertex_[0].c_index_ = c_face_[i * 3 + 0];
			face[i].vertex_[1].c_index_ = c_face_[i * 3 + 1];
			face[i].vertex_[2].c_index_ = c_face_[i * 3 + 2];
		}

		if (_num_uv_sets > 0) {
			// Allocate memory for all uv sets.
			face[i].vertex_[0].SetNumUVSets(_num_uv_sets);
			face[i].vertex_[1].SetNumUVSets(_num_uv_sets);
			face[i].vertex_[2].SetNumUVSets(_num_uv_sets);

			int j;
			std::list<MappingChannel*>::Iterator iter = mapping_channel_list_.First();
			for (j = 0; j < _num_uv_sets; ++j, ++iter) {
				MappingChannel* mapping_channel = *iter;
				face[i].vertex_[0].t_index_[j] = mapping_channel->tv_face_[i * 3 + 0];
				face[i].vertex_[1].t_index_[j] = mapping_channel->tv_face_[i * 3 + 1];
				face[i].vertex_[2].t_index_[j] = mapping_channel->tv_face_[i * 3 + 2];
			}
		}

		FaceListTable::Iterator __x = face_list_table.Find(face[i].sub_material_);
		if (__x == face_list_table.End()) {
			FaceList* face_list = new FaceList;
			face_list->PushBack(&face[i]);
			face_list_table.Insert(face[i].sub_material_, face_list);
		} else {
			(*__x)->PushBack(&face[i]);
		}
	}
}

ASELoader::Mesh::Face::Face() {
}

ASELoader::Mesh::Face::~Face() {
}

ASELoader::Mesh::FaceVertex::FaceVertex() :
	v_index_(0),
	c_index_(0),
	t_index_(0),
	num_uv_sets_(0) {
}

	ASELoader::Mesh::FaceVertex::FaceVertex(const FaceVertex& other) {
	*this = other;
}

ASELoader::Mesh::FaceVertex::~FaceVertex() {
	if (t_index_ != 0) {
		delete[] t_index_;
	}
}

void ASELoader::Mesh::FaceVertex::SetNumUVSets(int num_uv_sets) {
	if (t_index_ != 0) {
		delete[] t_index_;
		t_index_ = 0;
	}

	num_uv_sets_ = num_uv_sets;

	if (num_uv_sets_ > 0) {
		t_index_ = new int[num_uv_sets_];
	}
}

size_t ASELoader::Mesh::FaceVertex::GetHashCode(const FaceVertex& key) {
	return (size_t)(key.v_index_ * 100 + key.c_index_);
}

bool ASELoader::Mesh::FaceVertex::operator== (const FaceVertex& other) const {
	bool ok = (v_index_ == other.v_index_ && c_index_ == other.c_index_ && num_uv_sets_ == other.num_uv_sets_);
	if (ok) {
		for (int i = 0; ok && i < num_uv_sets_; i++) {
			ok = (t_index_[i] == other.t_index_[i]);
		}
	}
	return ok;
}

ASELoader::Mesh::FaceVertex& ASELoader::Mesh::FaceVertex::operator= (const FaceVertex& other) {
	v_index_ = other.v_index_;
	c_index_ = other.c_index_;

	if (t_index_ != 0) {
		delete[] t_index_;
		t_index_ = 0;
	}

	num_uv_sets_ = other.num_uv_sets_;

	if (num_uv_sets_ > 0) {
		t_index_ = new int[num_uv_sets_];
		for (int i = 0; i < num_uv_sets_; i++) {
			t_index_[i] = other.t_index_[i];
		}
	}

	return *this;
}



ASELoader::GeomObject::GeomObject() :
	node_name_(""),
	comment_(""),
	motion_blur_(false),
	cast_shadow_(false),
	recv_shadow_(false),
	tm_animation_(0),
	material_ref_(0) {
}

ASELoader::GeomObject::~GeomObject() {
	Init();
}

void ASELoader::GeomObject::Init() {
	node_name_ = "";
	comment_ = "";
	node_tm_list_.DeleteAll();
	motion_blur_ = false;
	cast_shadow_ = false;
	recv_shadow_ = false;
	if (tm_animation_ != 0) {
		delete tm_animation_;
		tm_animation_ = 0;
	}
	material_ref_ = 0;

	mesh_list_.DeleteAll();
}

void ASELoader::GeomObject::AllocTMAnimation() {
	if (tm_animation_ == 0) {
		tm_animation_ = new TMAnimation;
	} else {
		tm_animation_->Init();
	}
}



ASELoader::CameraSettings::CameraSettings() :
	time_value_(0),
	near_(0),
	far_(0),
	fov_(0),
	t_dist_(0) {
}

ASELoader::CameraSettings::~CameraSettings() {
}

void ASELoader::CameraSettings::Init() {
	time_value_ = 0;
	near_ = 0;
	far_ = 0;
	fov_ = 0;
	t_dist_ = 0;
}

ASELoader::CameraObject::CameraObject() :
	node_name_(""),
	comment_(""),
	camera_type_(""),
	settings_(0),
	tm_animation_(0) {
}

ASELoader::CameraObject::~CameraObject() {
	Init();
}

void ASELoader::CameraObject::Init() {
	node_name_ = "";
	comment_ = "";
	camera_type_ = "";
	node_tm_list_.DeleteAll();

	if (settings_ != 0) {
		delete settings_;
		settings_ = 0;
	}

	if (tm_animation_ != 0) {
		delete tm_animation_;
		tm_animation_ = 0;
	}
}

void ASELoader::CameraObject::AllocCameraSettings() {
	if (settings_ == 0) {
		settings_ = new CameraSettings;
	} else {
		settings_->Init();
	}
}

void ASELoader::CameraObject::AllocTMAnimation() {
	if (tm_animation_ == 0) {
		tm_animation_ = new TMAnimation;
	} else {
		tm_animation_->Init();
	}
}



ASELoader::LightSettings::LightSettings() :
	time_value_(0),
	intens_(0),
	aspect_(0),
	hot_spot_(0),
	falloff_(0),
	t_dist_(0),
	map_bias_(0),
	map_range_(0),
	map_size_(0),
	ray_bias_(0) {
	color_[0] = 0;
	color_[1] = 0;
	color_[2] = 0;
}

ASELoader::LightSettings::~LightSettings() {
}

void ASELoader::LightSettings::Init() {
	time_value_ = 0;
	color_[0] = 0;
	color_[1] = 0;
	color_[2] = 0;
	intens_ = 0;
	aspect_ = 0;
	hot_spot_ = 0;
	falloff_ = 0;
	t_dist_ = 0;
	map_bias_ = 0;
	map_range_ = 0;
	map_size_ = 0;
	ray_bias_ = 0;
}

ASELoader::LightObject::LightObject() :
	node_name_(""),
	comment_(""),
	light_type_(""),
	shadows_(""),
	use_light_(false),
	spot_shape_(""),
	use_global_(false),
	abs_map_bias_(false),
	over_shoot_(false),
	settings_(0),
	tm_animation_(0) {
}

ASELoader::LightObject::~LightObject() {
	Init();
}

void ASELoader::LightObject::Init() {
	node_name_ = "";
	comment_ = "";
	light_type_ = "";
	node_tm_list_.DeleteAll();

	shadows_ = "";
	use_light_ = false;
	spot_shape_ = "";
	use_global_ = false;
	abs_map_bias_ = false;
	over_shoot_ = false;

	if (settings_ != 0) {
		delete settings_;
		settings_ = 0;
	}

	if (tm_animation_ != 0) {
		delete tm_animation_;
		tm_animation_ = 0;
	}
}

void ASELoader::LightObject::AllocLightSettings() {
	if (settings_ == 0) {
		settings_ = new LightSettings;
	} else {
		settings_->Init();
	}
}

void ASELoader::LightObject::AllocTMAnimation() {
	if (tm_animation_ == 0) {
		tm_animation_ = new TMAnimation;
	} else {
		tm_animation_->Init();
	}
}



ASELoader::ASEData::ASEData() :
	m3DSMaxAsciiExport_(0),
	comment_(""),
	scene_(0) {
}

ASELoader::ASEData::~ASEData() {
	Init();
}

void ASELoader::ASEData::Init() {
	m3DSMaxAsciiExport_ = 0;
	comment_ = "";
	if (scene_ != 0) {
		delete scene_;
		scene_ = 0;
	}

	material_list_.DeleteAll();
	geom_list_.DeleteAll();
	cam_list_.DeleteAll();
	light_list_.DeleteAll();
}


}
