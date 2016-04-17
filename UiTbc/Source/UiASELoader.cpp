/*
	Class:  ASELoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "pch.h"
#include "../../Lepra/Include/ImageLoader.h"
#include "../../Lepra/Include/Canvas.h"
#include "../../Lepra/Include/DiskFile.h"
#include "../../Lepra/Include/Params.h"
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/Vector2D.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../Include/UiASELoader.h"
#include "../Include/UiTexture.h"
#include "../Include/UiTriangleBasedGeometry.h"
#include <list>

namespace UiTbc
{

ASELoader::ASELoader()
{
}


bool ASELoader::Load(ASEData& pASEData, const Lepra::String& pFileName)
{
	bool lOk = false;
	Lepra::DiskFile lFile;
	if (lFile.Open(pFileName, Lepra::DiskFile::MODE_READ) == true)
	{
		lOk = Load(pASEData, lFile);
		lFile.Close();
	}

	return lOk;
}

bool ASELoader::Load(ASEData& pASEData, Lepra::Reader& pReader)
{
	pASEData.Init();

	//
	// Read the file into an AnsiString.
	//

	bool lStatusOk = true;
	unsigned int lSize = (unsigned int)pReader.GetAvailable();
	char* lData = new char[lSize + 1];
	lStatusOk = pReader.ReadData(lData, (unsigned int)pReader.GetAvailable()) == Lepra::IO_OK;
	lData[lSize] = 0;

	Lepra::AnsiString lDataString;
	if (lStatusOk == true)
	{
		lDataString = lData;
	}
	delete[] lData;

	//
	// Parse the file.
	//

	if (lStatusOk)
	{
		lStatusOk = Parse(pASEData, lDataString);
	}

	return lStatusOk;
}

bool ASELoader::Parse(ASEData& pASEData, const Lepra::AnsiString& pDataString)
{
	//
	// Setup special tokens.
	//

	Lepra::AnsiString lSpecialTokens;
	lSpecialTokens.AddToken("*");
	lSpecialTokens.AddToken("{");
	lSpecialTokens.AddToken("}");
	lSpecialTokens.AddToken("\"");
	lSpecialTokens.AddToken(",");

	static char* slDelimitors = " \t\v\b\r\n";

	int lIndex = 0;
	bool lStatusOk = true;
	bool lDone = false;
	Lepra::AnsiString lToken;
	while (lStatusOk && !lDone)
	{
		lIndex = pDataString.GetToken(lToken, lIndex, " \t\v\b\r\n", lSpecialTokens);
		lDone = lIndex < 0;
		lStatusOk = (lToken == "*") && lIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			lIndex = pDataString.GetToken(lToken, lIndex, " \t\v\b\r\n", lSpecialTokens);
			if (lToken == "3DSMAX_ASCIIEXPORT")
				lStatusOk = ReadInt(pASEData.m3DSMaxAsciiExport, pDataString, lIndex, lSpecialTokens);
			else if(lToken == "COMMENT")
				lStatusOk = ReadQuotedString(pASEData.mComment, pDataString, lIndex, lSpecialTokens);
			else if(lToken == "SCENE")
				lStatusOk = ReadSCENE(pASEData, pDataString, lIndex, lSpecialTokens);
			else if(lToken == "MATERIAL_LIST")
				lStatusOk = ReadMATERIAL_LIST(pASEData, pDataString, lIndex, lSpecialTokens);
			else if(lToken == "GEOMOBJECT")
			{
				GeomObject* lGeomObj = new GeomObject;
				lStatusOk = ReadGEOMOBJECT(lGeomObj, pDataString, lIndex, lSpecialTokens);

				if (lStatusOk)
				{
					pASEData.mGeomList.PushBack(lGeomObj);
				}
				else
				{
					delete lGeomObj;
				}
			}
			else if(lToken == "CAMERAOBJECT")
			{
				CameraObject* lCamObj = new CameraObject;
				lStatusOk = ReadCAMERAOBJECT(lCamObj, pDataString, lIndex, lSpecialTokens);

				if (lStatusOk)
				{
					pASEData.mCamList.PushBack(lCamObj);
				}
				else
				{
					delete lCamObj;
				}
			}
			else if(lToken == "LIGHTOBJECT")
			{
				LightObject* lLightObj = new LightObject;
				lStatusOk = ReadLIGHTOBJECT(lLightObj, pDataString, lIndex, lSpecialTokens);

				if (lStatusOk)
				{
					pASEData.mLightList.PushBack(lLightObj);
				}
				else
				{
					delete lLightObj;
				}
			}
			else
				lStatusOk = false;
		}
	}

	return lStatusOk;
}

bool ASELoader::ReadQuotedString(Lepra::AnsiString& pString, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the first quote.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "\"" && pIndex > 0;
	}
	if (lStatusOk)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, "", pSpecialTokens);
	}

	if (lToken == "\"")
	{
		pString = "";
	}
	else
	{
		pString = lToken;

		lStatusOk = pIndex > 0;
		if (lStatusOk)
		{
			// Read the final quote.
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			lStatusOk = lToken == "\"";
		}
	}

	return lStatusOk;
}

bool ASELoader::ReadUnquotedString(Lepra::AnsiString& pString, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	if (lStatusOk)
	{
		pIndex = pDataString.GetToken(pString, pIndex, " \t\v\b\r\n:", pSpecialTokens);
	}

	return lStatusOk;
}

bool ASELoader::ReadInt(int& pInt, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;
	if (lStatusOk)
	{
		// Read the integer.
		Lepra::AnsiString lToken;
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n:", pSpecialTokens);
		lStatusOk = Lepra::AnsiStringUtility::StringToInt(lToken, pInt);
	}
	return lStatusOk;
}

bool ASELoader::ReadIntVec(int* pInt, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;
	if (lStatusOk)
		lStatusOk = ReadInt(pInt[0], pDataString, pIndex, pSpecialTokens);
	if (lStatusOk)
		lStatusOk = ReadInt(pInt[1], pDataString, pIndex, pSpecialTokens);
	if (lStatusOk)
		lStatusOk = ReadInt(pInt[2], pDataString, pIndex, pSpecialTokens);

	return lStatusOk;
}

bool ASELoader::ReadFloat(double& pFloat, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = (pIndex > 0);
	if (lStatusOk)
	{
		// Read the float value.
		Lepra::AnsiString lToken;
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n:", pSpecialTokens);
		lStatusOk = Lepra::AnsiStringUtility::StringToDouble(lToken, pFloat);
	}
	return lStatusOk;
}

bool ASELoader::ReadFloatVec(double* pFloat, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;
	if (lStatusOk)
		lStatusOk = ReadFloat(pFloat[0], pDataString, pIndex, pSpecialTokens);
	if (lStatusOk)
		lStatusOk = ReadFloat(pFloat[1], pDataString, pIndex, pSpecialTokens);
	if (lStatusOk)
		lStatusOk = ReadFloat(pFloat[2], pDataString, pIndex, pSpecialTokens);

	return lStatusOk;
}

bool ASELoader::ReadBool(bool& pBool, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	int lInt = -1;
	bool lStatusOk = ReadInt(lInt, pDataString, pIndex, pSpecialTokens);
	if (lStatusOk)
	{
		switch(lInt)
		{
		case 0:
			pBool = false;
			break;
		case 1:
			pBool = true;
			break;
		default:
			lStatusOk = false;
		}
	}

	return lStatusOk;
}

bool ASELoader::ReadVertexList(float* pVertex, int pNumVertex, char* pVertexIdentifier, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	if (lStatusOk)
	{
		int i;
		for (i = 0; lStatusOk && i < pNumVertex; i++)
		{
			if (lStatusOk)
			{
				// Read the '*'.
				pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
				lStatusOk = lToken == "*" && pIndex > 0;
			}

			if (lStatusOk)
			{
				pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
				lStatusOk = lToken == pVertexIdentifier && pIndex > 0;
			}

			// Read vertex index. Must equal 'i'.
			int lVIndex = -1;
			if (lStatusOk)
			{
				lStatusOk = ReadInt(lVIndex, pDataString, pIndex, pSpecialTokens);
			}

			if (lStatusOk)
			{
				lStatusOk = (lVIndex == i);
			}

			double lVertex[3];
			if (lStatusOk)
			{
				// Read the vector...
				lStatusOk = ReadFloatVec(lVertex, pDataString, pIndex, pSpecialTokens);
			}

			if (lStatusOk)
			{
				// Type cast.
				pVertex[lVIndex * 3 + 0] = (float)lVertex[0];
				pVertex[lVIndex * 3 + 1] = (float)lVertex[1];
				pVertex[lVIndex * 3 + 2] = (float)lVertex[2];
			}
		}
	}

	if (lStatusOk)
	{
		// Read the end brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "}" && pIndex > 0;
	}

	return lStatusOk;
}

bool ASELoader::ReadFaceList(int* pFace, int pNumFaces, char* pFaceIdentifier, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	if (lStatusOk)
	{
		int i;
		for (i = 0; lStatusOk && i < pNumFaces; i++)
		{
			if (lStatusOk)
			{
				// Read the '*'.
				pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
				lStatusOk = lToken == "*" && pIndex > 0;
			}

			if (lStatusOk)
			{
				pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
				lStatusOk = lToken == pFaceIdentifier && pIndex > 0;
			}

			// Read face index. Must equal 'i'.
			int lFIndex = -1;
			if (lStatusOk)
			{
				lStatusOk = ReadInt(lFIndex, pDataString, pIndex, pSpecialTokens);
			}

			if (lStatusOk)
			{
				lStatusOk = (lFIndex == i);
			}

			if (lStatusOk)
			{
				// Read the face...
				lStatusOk = ReadIntVec(&pFace[lFIndex * 3], pDataString, pIndex, pSpecialTokens);
			}
		}
	}

	if (lStatusOk)
	{
		// Read the end brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "}" && pIndex > 0;
	}

	return lStatusOk;
}

bool ASELoader::ReadSCENE(ASEData& pASEData, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	if (pASEData.mScene == 0)
	{
		pASEData.mScene = new Scene;
	}
	else
	{
		pASEData.mScene->Init();
	}

	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "SCENE_FILENAME")
				lStatusOk = ReadQuotedString(pASEData.mScene->mFilename, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "COMMENT")
				lStatusOk = ReadQuotedString(pASEData.mScene->mComment, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "SCENE_FIRSTFRAME")
				lStatusOk = ReadInt(pASEData.mScene->mFirstFrame, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "SCENE_LASTFRAME")
				lStatusOk = ReadInt(pASEData.mScene->mLastFrame, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "SCENE_FRAMESPEED")
				lStatusOk = ReadInt(pASEData.mScene->mFrameSpeed, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "SCENE_TICKSPERFRAME")
				lStatusOk = ReadInt(pASEData.mScene->mTicksPerFrame, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "SCENE_BACKGROUND_STATIC")
				lStatusOk = ReadFloatVec(pASEData.mScene->mBackgroundStatic, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "SCENE_AMBIENT_STATIC")
				lStatusOk = ReadFloatVec(pASEData.mScene->mAmbientStatic, pDataString, pIndex, pSpecialTokens);
			else
				lStatusOk = false;
		}
	}

	return lStatusOk;
}

bool ASELoader::ReadMATERIAL_LIST(ASEData& pASEData, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	if (lStatusOk)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;
	}
	if (lStatusOk && !lDone)
	{
		// Read tag name. Should be MATERIAL_COUNT.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "MATERIAL_COUNT" && pIndex > 0;
	}

	int mMaterialCount = 0;
	if (lStatusOk && !lDone)
	{
		// Read the material count.
		lStatusOk = ReadInt(mMaterialCount, pDataString, pIndex, pSpecialTokens);
	}

	if (lStatusOk && !lDone)
	{
		for (int i = 0; i < mMaterialCount && lStatusOk; i++)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			lStatusOk = (lToken == "*") && pIndex > 0;

			if (lStatusOk)
			{
				pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
				lStatusOk = (lToken == "MATERIAL") && pIndex > 0;
			}

			int lMaterialIndex = -1;
			if (lStatusOk)
			{
				lStatusOk = ReadInt(lMaterialIndex, pDataString, pIndex, pSpecialTokens);
			}
			if (lStatusOk)
			{
				lStatusOk = (lMaterialIndex == i);
			}

			Material* lMaterial = 0;
			if (lStatusOk)
			{
				lMaterial = new Material;
				lStatusOk = ReadMATERIAL(lMaterial, pDataString, pIndex, pSpecialTokens);

				if (lStatusOk)
					pASEData.mMaterialList.PushBack(lMaterial);
				else
					delete lMaterial;
			}
		}
	}

	if (lStatusOk && !lDone)
	{
		// The block must end with a brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = (lToken == "}") && pIndex > 0;
	}

	return lStatusOk;
}

bool ASELoader::ReadMATERIAL(Material* pMaterial, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "MATERIAL_NAME")
				lStatusOk = ReadQuotedString(pMaterial->mName, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "COMMENT")
				lStatusOk = ReadQuotedString(pMaterial->mComment, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_CLASS")
				lStatusOk = ReadQuotedString(pMaterial->mClass, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_AMBIENT")
				lStatusOk = ReadFloatVec(pMaterial->mAmbient, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_DIFFUSE")
				lStatusOk = ReadFloatVec(pMaterial->mDiffuse, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_SPECULAR")
				lStatusOk = ReadFloatVec(pMaterial->mSpecular, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_SHINE")
				lStatusOk = ReadFloat(pMaterial->mShine, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_SHINESTRENGTH")
				lStatusOk = ReadFloat(pMaterial->mShineStrength, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_TRANSPARENCY")
				lStatusOk = ReadFloat(pMaterial->mTransparency, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_WIRESIZE")
				lStatusOk = ReadFloat(pMaterial->mWireSize, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_SHADING")
				lStatusOk = ReadUnquotedString(pMaterial->mShading, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_XP_FALLOFF")
				lStatusOk = ReadFloat(pMaterial->mXPFalloff, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_SELFILLUM")
				lStatusOk = ReadFloat(pMaterial->mSelfIllum, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_FALLOFF")
				lStatusOk = ReadUnquotedString(pMaterial->mFalloff, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MATERIAL_XP_TYPE")
				lStatusOk = ReadUnquotedString(pMaterial->mXPType, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_AMBIENT")
				lStatusOk = ReadMAP(pMaterial->mAmbientMapList, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_DIFFUSE")
				lStatusOk = ReadMAP(pMaterial->mDiffuseMapList, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_SPECULAR")
				lStatusOk = ReadMAP(pMaterial->mSpecularMapList, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_SHINE")
				lStatusOk = ReadMAP(pMaterial->mShineMapList, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_SHINESTRENGTH")
				lStatusOk = ReadMAP(pMaterial->mShineStrengthMapList, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_SELFILLUM")
				lStatusOk = ReadMAP(pMaterial->mSelfIllumMapList, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_OPACITY")
				lStatusOk = ReadMAP(pMaterial->mOpacityMapList, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_FILTERCOLOR")
				lStatusOk = ReadMAP(pMaterial->mFilterColorMapList, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_BUMP")
				lStatusOk = ReadMAP(pMaterial->mBumpMapList, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_REFLECT")
				lStatusOk = ReadMAP(pMaterial->mReflectMapList, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_REFRACT")
				lStatusOk = ReadMAP(pMaterial->mRefractMapList, pDataString, pIndex, pSpecialTokens);
			else
				lStatusOk = false;
		}
	}
	return lStatusOk;
}

bool ASELoader::ReadMAP(std::list<Map*>& pMapList, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	Map* lMap = new Map;

	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "MAP_NAME")
				lStatusOk = ReadQuotedString(lMap->mName, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "COMMENCT")
				lStatusOk = ReadQuotedString(lMap->mComment, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_CLASS")
				lStatusOk = ReadQuotedString(lMap->mClass, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_SUBNO")
				lStatusOk = ReadInt(lMap->mSubNo, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_AMOUNT")
				lStatusOk = ReadFloat(lMap->mAmount, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MAP_TYPE")
				lStatusOk = ReadUnquotedString(lMap->mType, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "BITMAP")
				lStatusOk = ReadQuotedString(lMap->mBitmapPath, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "BITMAP_FILTER")
				lStatusOk = ReadUnquotedString(lMap->mBitmapFilter, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "UVW_U_OFFSET")
				lStatusOk = ReadFloat(lMap->mUOffset, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "UVW_V_OFFSET")
				lStatusOk = ReadFloat(lMap->mVOffset, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "UVW_U_TILING")
				lStatusOk = ReadFloat(lMap->mUTiling, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "UVW_V_TILING")
				lStatusOk = ReadFloat(lMap->mVTiling, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "UVW_ANGLE")
				lStatusOk = ReadFloat(lMap->mUVWAngle, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "UVW_BLUR")
				lStatusOk = ReadFloat(lMap->mUVWBlur, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "UVW_BLUR_OFFSET")
				lStatusOk = ReadFloat(lMap->mUVWBlurOffset, pDataString, pIndex, pSpecialTokens);
			// Yeah, I know... "NOUSE" should be "NOISE". But don't correct this! It is correct.
			else if(lToken == "UVW_NOUSE_AMT") 
				lStatusOk = ReadFloat(lMap->mUVWNoiseAmt, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "UVW_NOISE_SIZE") 
				lStatusOk = ReadFloat(lMap->mUVWNoiseSize, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "UVW_NOISE_LEVEL")
				lStatusOk = ReadInt(lMap->mUVWNoiseLevel, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "UVW_NOISE_PHASE")
				lStatusOk = ReadFloat(lMap->mUVWNoisePhase, pDataString, pIndex, pSpecialTokens);
			else
				lStatusOk = false;
		}
	}

	if (lStatusOk)
	{
		pMapList.PushBack(lMap);
	}
	else
	{
		delete lMap;
	}

	return lStatusOk;
}

bool ASELoader::ReadGEOMOBJECT(GeomObject* pGeomObj, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "NODE_NAME")
				lStatusOk = ReadQuotedString(pGeomObj->mNodeName, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "COMMENT")
				lStatusOk = ReadQuotedString(pGeomObj->mComment, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "NODE_TM")
			{
				NodeTM* lNodeTM = new NodeTM;
				lStatusOk = ReadNODE_TM(lNodeTM, pDataString, pIndex, pSpecialTokens);

				if (lStatusOk)
					pGeomObj->mNodeTMList.PushBack(lNodeTM);
				else
					delete lNodeTM;
			}
			else if(lToken == "MESH")
			{
				Mesh* lMesh = new Mesh;
				lStatusOk = ReadMESH(lMesh, pDataString, pIndex, pSpecialTokens);

				if (lStatusOk)
					pGeomObj->mMeshList.PushBack(lMesh);
				else
					delete lMesh;
			}
			else if(lToken == "PROP_MOTIONBLUR")
				lStatusOk = ReadBool(pGeomObj->mMotionBlur, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "PROP_CASTSHADOW")
				lStatusOk = ReadBool(pGeomObj->mCastShadow, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "PROP_RECVSHADOW")
				lStatusOk = ReadBool(pGeomObj->mRecvShadow, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "TM_ANIMATION")
			{
				pGeomObj->AllocTMAnimation();
				lStatusOk = ReadTM_ANIMATION(pGeomObj->mTMAnimation, pDataString, pIndex, pSpecialTokens);
			}
			else if(lToken == "MATERIAL_REF")
				lStatusOk = ReadInt(pGeomObj->mMaterialRef, pDataString, pIndex, pSpecialTokens);
			else
				lStatusOk = false;
		}
	}
	return lStatusOk;
}

bool ASELoader::ReadCAMERAOBJECT(CameraObject* pCamObj, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "NODE_NAME")
				lStatusOk = ReadQuotedString(pCamObj->mNodeName, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "COMMENT")
				lStatusOk = ReadQuotedString(pCamObj->mComment, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "CAMERA_TYPE")
				lStatusOk = ReadUnquotedString(pCamObj->mCameraType, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "NODE_TM")
			{
				NodeTM* lNodeTM = new NodeTM;
				lStatusOk = ReadNODE_TM(lNodeTM, pDataString, pIndex, pSpecialTokens);
				if (lStatusOk)
					pCamObj->mNodeTMList.PushBack(lNodeTM);
				else
					delete lNodeTM;

			}
			else if(lToken == "CAMERA_SETTINGS")
			{
				pCamObj->AllocCameraSettings();
				lStatusOk = ReadCAMERA_SETTINGS(pCamObj->mSettings, pDataString, pIndex, pSpecialTokens);
			}
			else if(lToken == "TM_ANIMATION")
			{
				pCamObj->AllocTMAnimation();
				lStatusOk = ReadTM_ANIMATION(pCamObj->mTMAnimation, pDataString, pIndex, pSpecialTokens);
			}
			else
				lStatusOk = false;
		}
	}
	return lStatusOk;
}

bool ASELoader::ReadLIGHTOBJECT(LightObject* pLightObj, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "NODE_NAME")
				lStatusOk = ReadQuotedString(pLightObj->mNodeName, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "COMMENT")
				lStatusOk = ReadQuotedString(pLightObj->mComment, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_TYPE")
				lStatusOk = ReadUnquotedString(pLightObj->mLightType, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "NODE_TM")
			{
				NodeTM* lNodeTM = new NodeTM;
				lStatusOk = ReadNODE_TM(lNodeTM, pDataString, pIndex, pSpecialTokens);
				if (lStatusOk)
					pLightObj->mNodeTMList.PushBack(lNodeTM);
				else
					delete lNodeTM;

			}
			else if(lToken == "LIGHT_SHADOWS")
				lStatusOk = ReadUnquotedString(pLightObj->mShadows, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_USELIGHT")
				lStatusOk = ReadBool(pLightObj->mUseLight, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_SPOTSHAPE")
				lStatusOk = ReadUnquotedString(pLightObj->mSpotShape, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_USEGLOBAL")
				lStatusOk = ReadBool(pLightObj->mUseGlobal, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_ABSMAPBIAS")
				lStatusOk = ReadBool(pLightObj->mAbsMapBias, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_OVERSHOOT")
				lStatusOk = ReadBool(pLightObj->mOverShoot, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_SETTINGS")
			{
				pLightObj->AllocLightSettings();
				lStatusOk = ReadLIGHT_SETTINGS(pLightObj->mSettings, pDataString, pIndex, pSpecialTokens);
			}
			else if(lToken == "TM_ANIMATION")
			{
				pLightObj->AllocTMAnimation();
				lStatusOk = ReadTM_ANIMATION(pLightObj->mTMAnimation, pDataString, pIndex, pSpecialTokens);
			}
			else
				lStatusOk = false;
		}
	}
	return lStatusOk;
}

bool ASELoader::ReadCAMERA_SETTINGS(CameraSettings* pCameraSettings, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "TIMEVALUE")
				lStatusOk = ReadInt(pCameraSettings->mTimeValue, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "CAMERA_NEAR")
				lStatusOk = ReadFloat(pCameraSettings->mNear, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "CAMERA_FAR")
				lStatusOk = ReadFloat(pCameraSettings->mFar, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "CAMERA_FOV")
				lStatusOk = ReadFloat(pCameraSettings->mFOV, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "CAMERA_TDIST")
				lStatusOk = ReadFloat(pCameraSettings->mTDist, pDataString, pIndex, pSpecialTokens);
			else
				lStatusOk = false;
		}
	}
	return lStatusOk;
}

bool ASELoader::ReadLIGHT_SETTINGS(LightSettings* pLightSettings, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "TIMEVALUE")
				lStatusOk = ReadInt(pLightSettings->mTimeValue, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_COLOR")
				lStatusOk = ReadFloatVec(pLightSettings->mColor, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_INTENS")
				lStatusOk = ReadFloat(pLightSettings->mIntens, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_ASPECT")
				lStatusOk = ReadFloat(pLightSettings->mAspect, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_HOTSPOT")
				lStatusOk = ReadFloat(pLightSettings->mHotSpot, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_FALLOFF")
				lStatusOk = ReadFloat(pLightSettings->mFalloff, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_TDIST")
				lStatusOk = ReadFloat(pLightSettings->mTDist, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_MAPBIAS")
				lStatusOk = ReadFloat(pLightSettings->mMapBias, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_MAPRANGE")
				lStatusOk = ReadFloat(pLightSettings->mMapRange, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_MAPSIZE")
				lStatusOk = ReadFloat(pLightSettings->mMapSize, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "LIGHT_RAYBIAS")
				lStatusOk = ReadFloat(pLightSettings->mRayBias, pDataString, pIndex, pSpecialTokens);
			else
				lStatusOk = false;
		}
	}
	return lStatusOk;
}

bool ASELoader::ReadNODE_TM(NodeTM* pNodeTM, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "NODE_NAME")
				lStatusOk = ReadQuotedString(pNodeTM->mNodeName, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "COMMENT")
				lStatusOk = ReadQuotedString(pNodeTM->mComment, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "INHERIT_POS")
				lStatusOk = ReadIntVec(pNodeTM->mInheritPos, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "INHERIT_ROT")
				lStatusOk = ReadIntVec(pNodeTM->mInheritRot, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "INHERIT_SCL")
				lStatusOk = ReadIntVec(pNodeTM->mInheritScl, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "TM_ROW0")
				lStatusOk = ReadFloatVec(pNodeTM->mTMRow0, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "TM_ROW1")
				lStatusOk = ReadFloatVec(pNodeTM->mTMRow1, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "TM_ROW2")
				lStatusOk = ReadFloatVec(pNodeTM->mTMRow2, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "TM_ROW3")
				lStatusOk = ReadFloatVec(pNodeTM->mTMRow3, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "TM_POS")
				lStatusOk = ReadFloatVec(pNodeTM->mTMPos, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "TM_ROTAXIS")
				lStatusOk = ReadFloatVec(pNodeTM->mTMRotAxis, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "TM_ROTANGLE")
				lStatusOk = ReadFloat(pNodeTM->mTMRotAngle, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "TM_SCALE")
				lStatusOk = ReadFloatVec(pNodeTM->mTMScale, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "TM_SCALEAXIS")
				lStatusOk = ReadFloatVec(pNodeTM->mTMScaleAxis, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "TM_SCALEAXISANG")
				lStatusOk = ReadFloat(pNodeTM->mTMScaleAxisAng, pDataString, pIndex, pSpecialTokens);
			else
				lStatusOk = false;
		}
	}
	return lStatusOk;
}

bool ASELoader::ReadTM_ANIMATION(TMAnimation* pTMAnimation, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "NODE_NAME")
				lStatusOk = ReadQuotedString(pTMAnimation->mNodeName, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "COMMENT")
				lStatusOk = ReadQuotedString(pTMAnimation->mComment, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "CONTROL_POS_TRACK")
				lStatusOk = ReadCONTROL_POS_TRACK(pTMAnimation, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "CONTROL_ROT_TRACK")
				lStatusOk = ReadCONTROL_ROT_TRACK(pTMAnimation, pDataString, pIndex, pSpecialTokens);
			else
				lStatusOk = false;
		}
	}
	return lStatusOk;
}

bool ASELoader::ReadCONTROL_POS_TRACK(TMAnimation* pTMAnimation, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "CONTROL_POS_SAMPLE")
			{
				ControlPosSample* lSample = new ControlPosSample;
				lStatusOk = ReadCONTROL_POS_SAMPLE(lSample, pDataString, pIndex, pSpecialTokens);
				if (lStatusOk)
				{
					pTMAnimation->mControlPosTrack.PushBack(lSample);
				}
				else
				{
					delete lSample;
				}
			}
			else
			{
				lStatusOk = false;
			}
		}
	}
	return lStatusOk;
}

bool ASELoader::ReadCONTROL_ROT_TRACK(TMAnimation* pTMAnimation, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "CONTROL_ROT_SAMPLE")
			{
				ControlRotSample* lSample = new ControlRotSample;
				lStatusOk = ReadCONTROL_ROT_SAMPLE(lSample, pDataString, pIndex, pSpecialTokens);
				if (lStatusOk)
				{
					pTMAnimation->mControlRotTrack.PushBack(lSample);
				}
				else
				{
					delete lSample;
				}
			}
			else
			{
				lStatusOk = false;
			}
		}
	}
	return lStatusOk;
}

bool ASELoader::ReadCONTROL_POS_SAMPLE(ControlPosSample* pControlPosSample, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;
	if (lStatusOk)
		lStatusOk = ReadInt(pControlPosSample->mTimeValue, pDataString, pIndex, pSpecialTokens);
	if (lStatusOk)
		lStatusOk = ReadFloat(pControlPosSample->mPos[0], pDataString, pIndex, pSpecialTokens);
	if (lStatusOk)
		lStatusOk = ReadFloat(pControlPosSample->mPos[1], pDataString, pIndex, pSpecialTokens);
	if (lStatusOk)
		lStatusOk = ReadFloat(pControlPosSample->mPos[2], pDataString, pIndex, pSpecialTokens);
	return lStatusOk;
}

bool ASELoader::ReadCONTROL_ROT_SAMPLE(ControlRotSample* pControlRotSample, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;
	if (lStatusOk)
		lStatusOk = ReadInt(pControlRotSample->mTimeValue, pDataString, pIndex, pSpecialTokens);
	if (lStatusOk)
		lStatusOk = ReadFloat(pControlRotSample->mRot[0], pDataString, pIndex, pSpecialTokens);
	if (lStatusOk)
		lStatusOk = ReadFloat(pControlRotSample->mRot[1], pDataString, pIndex, pSpecialTokens);
	if (lStatusOk)
		lStatusOk = ReadFloat(pControlRotSample->mRot[2], pDataString, pIndex, pSpecialTokens);
	if (lStatusOk)
		lStatusOk = ReadFloat(pControlRotSample->mRot[3], pDataString, pIndex, pSpecialTokens);
	return lStatusOk;
}

bool ASELoader::ReadMESH(Mesh* pMesh, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "COMMENT")
				lStatusOk = ReadQuotedString(pMesh->mComment, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "TIMEVALUE")
				lStatusOk = ReadInt(pMesh->mTimeValue, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MESH_NUMVERTEX")
				lStatusOk = ReadInt(pMesh->mNumVertex, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MESH_NUMFACES")
				lStatusOk = ReadInt(pMesh->mNumFaces, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MESH_VERTEX_LIST")
			{
				pMesh->AllocVertexArray();
				lStatusOk = ReadVertexList(pMesh->mVertex, pMesh->mNumVertex, "MESH_VERTEX", pDataString, pIndex, pSpecialTokens);
			}
			else if(lToken == "MESH_FACE_LIST")
				lStatusOk = ReadMESH_FACE_LIST(pMesh, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MESH_NUMTVERTEX")
				lStatusOk = ReadInt(pMesh->GetDefaultMappingChannel()->mNumTVertex, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MESH_TVERTLIST")
			{
				MappingChannel* lMappingChannel = pMesh->GetDefaultMappingChannel();
				lMappingChannel->AllocVertexArray();
				lStatusOk = ReadVertexList(lMappingChannel->mTVertex, lMappingChannel->mNumTVertex, "MESH_TVERT", pDataString, pIndex, pSpecialTokens);
			}
			else if(lToken == "MESH_NUMTVFACES")
				lStatusOk = ReadInt(pMesh->GetDefaultMappingChannel()->mNumTVFaces, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MESH_TFACELIST")
			{
				MappingChannel* lMappingChannel = pMesh->GetDefaultMappingChannel();
				lMappingChannel->AllocFaceArray();
				lStatusOk = ReadFaceList(lMappingChannel->mTVFace, lMappingChannel->mNumTVFaces, "MESH_TFACE", pDataString, pIndex, pSpecialTokens);
			}
			else if(lToken == "MESH_NUMCVERTEX")
				lStatusOk = ReadInt(pMesh->mNumCVertex, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MESH_CVERTLIST")
			{
				pMesh->AllocCVertexArray();
				lStatusOk = ReadVertexList(pMesh->mCVertex, pMesh->mNumCVertex, "MESH_VERTCOL", pDataString, pIndex, pSpecialTokens);
			}
			else if(lToken == "MESH_NUMCVFACES")
				lStatusOk = ReadInt(pMesh->mNumCVFaces, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MESH_CFACELIST")
			{
				pMesh->AllocCFaceArray();
				lStatusOk = ReadFaceList(pMesh->mCFace, pMesh->mNumCVFaces, "MESH_CFACE", pDataString, pIndex, pSpecialTokens);
			}
			else if(lToken == "MESH_NORMALS")
				lStatusOk = ReadMESH_NORMALS(pMesh, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MESH_MAPPINGCHANNEL")
				lStatusOk = ReadMESH_MAPPINGCHANNEL(pMesh, pDataString, pIndex, pSpecialTokens);
			else
				lStatusOk = false;
		}
	}
	return lStatusOk;
}

bool ASELoader::ReadMESH_FACE_LIST(Mesh* pMesh, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	if (lStatusOk)
	{
		pMesh->AllocFaceArray();

		int i;
		for (i = 0; lStatusOk && i < pMesh->mNumFaces; i++)
		{
			if (lStatusOk)
			{
				// Read the '*'.
				pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
				lStatusOk = lToken == "*" && pIndex > 0;
			}

			if (lStatusOk)
			{
				// Read 'MESH_FACE'.
				pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
				lStatusOk = lToken == "MESH_FACE" && pIndex > 0;
			}

			// Read face index. Must equal 'i'.
			int lFIndex = -1;
			if (lStatusOk)
			{
				lStatusOk = ReadInt(lFIndex, pDataString, pIndex, pSpecialTokens);
			}

			if (lStatusOk)
			{
				lStatusOk = (lFIndex == i);
			}

			if (lStatusOk)
			{
				// Read an 'A'.
				lStatusOk = ReadUnquotedString(lToken, pDataString, pIndex, pSpecialTokens);
				if (lStatusOk)
				{
					lStatusOk = lToken == "A";
				}
			}

			if (lStatusOk)
			{
				// Read the first index.
				lStatusOk = ReadInt(pMesh->mFace[lFIndex * 3 + 0], pDataString, pIndex, pSpecialTokens);
			}

			if (lStatusOk)
			{
				// Check constraints.
				lStatusOk = pMesh->mFace[lFIndex * 3 + 0] >= 0 && 
					      pMesh->mFace[lFIndex * 3 + 0] < pMesh->mNumFaces;
			}

			if (lStatusOk)
			{
				// Read a 'B'.
				lStatusOk = ReadUnquotedString(lToken, pDataString, pIndex, pSpecialTokens);
				if (lStatusOk)
				{
					lStatusOk = lToken == "B";
				}
			}

			if (lStatusOk)
			{
				// Read the second index.
				lStatusOk = ReadInt(pMesh->mFace[lFIndex * 3 + 1], pDataString, pIndex, pSpecialTokens);
			}

			if (lStatusOk)
			{
				// Check constraints.
				lStatusOk = pMesh->mFace[lFIndex * 3 + 1] >= 0 && 
					      pMesh->mFace[lFIndex * 3 + 1] < pMesh->mNumFaces;
			}

			if (lStatusOk)
			{
				// Read a 'C'.
				lStatusOk = ReadUnquotedString(lToken, pDataString, pIndex, pSpecialTokens);
				if (lStatusOk)
				{
					lStatusOk = lToken == "C";
				}
			}

			if (lStatusOk)
			{
				// Read the third index.
				lStatusOk = ReadInt(pMesh->mFace[lFIndex * 3 + 2], pDataString, pIndex, pSpecialTokens);
			}

			if (lStatusOk)
			{
				// Check constraints.
				lStatusOk = pMesh->mFace[lFIndex * 3 + 2] >= 0 && 
					      pMesh->mFace[lFIndex * 3 + 2] < pMesh->mNumFaces;
			}

			if (lStatusOk)
			{
				// Read 'AB'.
				lStatusOk = ReadUnquotedString(lToken, pDataString, pIndex, pSpecialTokens);
				if (lStatusOk)
				{
					lStatusOk = lToken == "AB";
				}
			}

			if (lStatusOk)
			{
				lStatusOk = ReadBool(pMesh->mFaceEdgeVisible[lFIndex * 3 + 0], pDataString, pIndex, pSpecialTokens);
			}

			if (lStatusOk)
			{
				// Read 'BC'.
				lStatusOk = ReadUnquotedString(lToken, pDataString, pIndex, pSpecialTokens);
				if (lStatusOk)
				{
					lStatusOk = lToken == "BC";
				}
			}

			if (lStatusOk)
			{
				lStatusOk = ReadBool(pMesh->mFaceEdgeVisible[lFIndex * 3 + 1], pDataString, pIndex, pSpecialTokens);
			}

			if (lStatusOk)
			{
				// Read 'CA'.
				lStatusOk = ReadUnquotedString(lToken, pDataString, pIndex, pSpecialTokens);
				if (lStatusOk)
				{
					lStatusOk = lToken == "CA";
				}
			}

			if (lStatusOk)
			{
				lStatusOk = ReadBool(pMesh->mFaceEdgeVisible[lFIndex * 3 + 2], pDataString, pIndex, pSpecialTokens);
			}

			if (lStatusOk)
			{
				// Read '*'.
				pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
				lStatusOk = lToken == "*" && pIndex > 0;
			}
			
			if (lStatusOk)
			{
				// Read 'MESH_SMOOTHING'.
				pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
				lStatusOk = lToken == "MESH_SMOOTHING" && pIndex > 0;
			}

			bool lDone = false;
			while (lStatusOk && !lDone)
			{
				int lSmoothingGroup;
				lStatusOk = ReadInt(lSmoothingGroup, pDataString, pIndex, pSpecialTokens);

				if (lStatusOk)
				{
					pMesh->mSmoothingGroupList[i].PushBack(lSmoothingGroup);

					// Read comma or '*'...
					pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
					lDone = lToken == "*";
					lStatusOk = (lToken == ",") || lDone && pIndex > 0;
				}
			}

			if (lStatusOk)
			{
				// Read 'MESH_MTLID'.
				pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
				lStatusOk = lToken == "MESH_MTLID" && pIndex > 0;
			}

			if (lStatusOk)
			{
				lStatusOk = ReadInt(pMesh->mSubMaterial[i], pDataString, pIndex, pSpecialTokens);
			}
		}
	}

	if (lStatusOk)
	{
		// Read the end brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "}" && pIndex > 0;
	}

	return lStatusOk;
}

bool ASELoader::ReadMESH_NORMALS(Mesh* pMesh, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	pMesh->AllocNormalArrays();

	bool lStatusOk = pIndex > 0;

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	if (lStatusOk)
	{
		// Normals are stored in a "per face"-order.
		for (int i = 0; lStatusOk && i < pMesh->mNumFaces; i++)
		{
			if (lStatusOk)
			{
				// Read '*'.
				pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
				lStatusOk = lToken == "*" && pIndex > 0;
			}

			if (lStatusOk)
			{
				// Read 'MESH_FACENORMAL'.
				pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
				lStatusOk = lToken == "MESH_FACENORMAL" && pIndex > 0;
			}

			int lFIndex = -1;
			if (lStatusOk)
			{
				// Read face index.
				lStatusOk = ReadInt(lFIndex, pDataString, pIndex, pSpecialTokens);
			}

			if (lStatusOk)
			{
				// Check constraints.
				lStatusOk = lFIndex >= 0 && lFIndex < pMesh->mNumFaces;
			}

			double lFaceNormal[3];
			if (lStatusOk)
			{
				// Read the face normal.
				lStatusOk = ReadFloatVec(lFaceNormal, pDataString, pIndex, pSpecialTokens);
			}

			if (lStatusOk)
			{
				// Type cast.
				pMesh->mFaceNormals[lFIndex * 3 + 0] = (float)lFaceNormal[0];
				pMesh->mFaceNormals[lFIndex * 3 + 1] = (float)lFaceNormal[1];
				pMesh->mFaceNormals[lFIndex * 3 + 2] = (float)lFaceNormal[2];
			}

			// Read the face's 3 vertex normals. 
			// Alex: This redundancy is such a total waste of disk 
			//       space. Well well... It's not my file format anyway.
			for (int j = 0; lStatusOk && j < 3; j++)
			{
				if (lStatusOk)
				{
					// Read '*'.
					pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
					lStatusOk = lToken == "*" && pIndex > 0;
				}

				if (lStatusOk)
				{
					// Read 'MESH_VERTEXNORMAL'.
					pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
					lStatusOk = lToken == "MESH_VERTEXNORMAL" && pIndex > 0;
				}

				int lVIndex = -1;
				if (lStatusOk)
				{
					// Read vertex index.
					lStatusOk = ReadInt(lVIndex, pDataString, pIndex, pSpecialTokens);
				}

				if (lStatusOk)
				{
					// Check constraints.
					lStatusOk = lVIndex >= 0 && lVIndex < pMesh->mNumVertex;
				}

				double lVertexNormal[3];
				if (lStatusOk)
				{
					// Read the vertex normal.
					lStatusOk = ReadFloatVec(lVertexNormal, pDataString, pIndex, pSpecialTokens);
				}

				if (lStatusOk)
				{
					// Type cast.
					pMesh->mVertexNormals[lVIndex * 3 + 0] = (float)lVertexNormal[0];
					pMesh->mVertexNormals[lVIndex * 3 + 1] = (float)lVertexNormal[1];
					pMesh->mVertexNormals[lVIndex * 3 + 2] = (float)lVertexNormal[2];
				}
			}
		}
	}

	if (lStatusOk)
	{
		// Read the end brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "}" && pIndex > 0;
	}

	return lStatusOk;
}

bool ASELoader::ReadMESH_MAPPINGCHANNEL(Mesh* pMesh, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens)
{
	bool lStatusOk = pIndex > 0;

	MappingChannel* lMappingChannel = new MappingChannel;
	int lMappingChannelIndex;

	if (lStatusOk)
	{
		// Read the mapping channel index.
		lStatusOk = ReadInt(lMappingChannelIndex, pDataString, pIndex, pSpecialTokens);
	}

	Lepra::AnsiString lToken;
	if (lStatusOk)
	{
		// Read the start brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "{" && pIndex > 0;
	}

	bool lDone = false;
	while (lStatusOk && !lDone)
	{
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lDone = lToken == "}";
		lStatusOk = (lToken == "*") && pIndex > 0 || lDone;

		if (lStatusOk && !lDone)
		{
			pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
			if (lToken == "COMMENT")
				lStatusOk = ReadQuotedString(lMappingChannel->mComment, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MESH_NUMTVERTEX")
				lStatusOk = ReadInt(lMappingChannel->mNumTVertex, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MESH_TVERTLIST")
			{
				lMappingChannel->AllocVertexArray();
				lStatusOk = ReadVertexList(lMappingChannel->mTVertex, lMappingChannel->mNumTVertex, "MESH_TVERT", pDataString, pIndex, pSpecialTokens);
			}
			else if(lToken == "MESH_NUMTVFACES")
				lStatusOk = ReadInt(lMappingChannel->mNumTVFaces, pDataString, pIndex, pSpecialTokens);
			else if(lToken == "MESH_TFACELIST")
			{
				lMappingChannel->AllocFaceArray();
				lStatusOk = ReadFaceList(lMappingChannel->mTVFace, lMappingChannel->mNumTVFaces, "MESH_TFACE", pDataString, pIndex, pSpecialTokens);
			}
			else
				lStatusOk = false;
		}
	}

	if (lStatusOk)
	{
		// Read the end brace.
		pIndex = pDataString.GetToken(lToken, pIndex, " \t\v\b\r\n", pSpecialTokens);
		lStatusOk = lToken == "}" && pIndex > 0;
	}

	if (lStatusOk)
	{
		pMesh->mMappingChannelList.PushBack(lMappingChannel);
	}
	else
	{
		delete lMappingChannel;
	}

	return lStatusOk;
}

// bool ASELoader::Save(const GeometryList& pGeometryList, const Lepra::AnsiString& pFileName);






ASELoader::Scene::Scene() :
	mFirstFrame(0),
	mLastFrame(0),
	mFrameSpeed(0),
	mTicksPerFrame(0)
{
	mBackgroundStatic[0] = 0;
	mBackgroundStatic[1] = 0;
	mBackgroundStatic[2] = 0;
	mAmbientStatic[0] = 0;
	mAmbientStatic[1] = 0;
	mAmbientStatic[2] = 0;
}

ASELoader::Scene::~Scene()
{
}

void ASELoader::Scene::Init()
{
	mFilename = "";
	mComment = "";
	mFirstFrame = 0;
	mLastFrame = 0;
	mFrameSpeed = 0;
	mTicksPerFrame = 0;
	mBackgroundStatic[0] = 0;
	mBackgroundStatic[1] = 0;
	mBackgroundStatic[2] = 0;
	mAmbientStatic[0] = 0;
	mAmbientStatic[1] = 0;
	mAmbientStatic[2] = 0;
}



ASELoader::Map::Map() :
	mSubNo(0),
	mAmount(0),
	mUOffset(0),
	mVOffset(0),
	mUTiling(0),
	mVTiling(0),
	mUVWAngle(0),
	mUVWBlur(0),
	mUVWBlurOffset(0),
	mUVWNoiseAmt(0),
	mUVWNoiseSize(0),
	mUVWNoiseLevel(0),
	mUVWNoisePhase(0)
{
}

ASELoader::Map::~Map()
{
}

void ASELoader::Map::Init()
{
	mName = "";
	mClass = "";
	mComment = "";
	mSubNo = 0;
	mAmount = 0;
	mBitmapPath = "";
	mType = "";
	mUOffset = 0;
	mVOffset = 0;
	mUTiling = 0;
	mVTiling = 0;
	mUVWAngle = 0;
	mUVWBlur = 0;
	mUVWBlurOffset = 0;
	mUVWNoiseAmt = 0;
	mUVWNoiseSize = 0;
	mUVWNoiseLevel = 0;
	mUVWNoisePhase = 0;
	mBitmapFilter = "";
}



ASELoader::Material::Material() :
	mShine(0),
	mShineStrength(0),
	mTransparency(0),
	mWireSize(0),
	mXPFalloff(0),
	mSelfIllum(0)
{
	mAmbient[0] = 0;
	mAmbient[1] = 0;
	mAmbient[2] = 0;
	mDiffuse[0] = 0;
	mDiffuse[1] = 0;
	mDiffuse[2] = 0;
	mSpecular[0] = 0;
	mSpecular[1] = 0;
	mSpecular[2] = 0;
}

ASELoader::Material::~Material()
{
	Init();
}

void ASELoader::Material::Init()
{
	mName = "";
	mClass = "";
	mComment = "";
	mShine = 0;
	mShineStrength = 0;
	mTransparency = 0;
	mWireSize = 0;
	mShading = "";
	mXPFalloff = 0;
	mSelfIllum = 0;
	mFalloff = "";
	mXPType = "";

	mAmbientMapList.DeleteAll();
	mDiffuseMapList.DeleteAll();
	mSpecularMapList.DeleteAll();
	mShineMapList.DeleteAll();
	mShineStrengthMapList.DeleteAll();
	mSelfIllumMapList.DeleteAll();
	mOpacityMapList.DeleteAll();
	mFilterColorMapList.DeleteAll();
	mBumpMapList.DeleteAll();
	mReflectMapList.DeleteAll();
	mRefractMapList.DeleteAll();

	mSubMaterialList.DeleteAll();
}

bool ASELoader::Material::LoadAsTexture(Texture& pTexture)
{
	Lepra::ImageLoader lLoader;
	Lepra::Canvas lColorMap;
	Lepra::Canvas lAlphaMap;
	Lepra::Canvas lNormalMap;
	Lepra::Canvas lSpecularMap;
	Lepra::Canvas* lAlphaMap = 0;
	Lepra::Canvas* lNormalMap = 0;
	Lepra::Canvas* lSpecularMap = 0;

	bool lStatusOk = !mDiffuseMapList.IsEmpty();

	if (lStatusOk)
	{
		Map* lDiffuseMap = *mDiffuseMapList.First();
		lStatusOk = lLoader.Load(lDiffuseMap->mBitmapPath.ToCurrentCode(), lColorMap);
	}

	if (lStatusOk && mOpacityMapList.IsEmpty() == false)
	{
		Map* lOpacityMap = *mOpacityMapList.First();
		lStatusOk = lLoader.Load(lOpacityMap->mBitmapPath.ToCurrentCode(), lAlphaMap);
		if (lStatusOk)
		{
			lAlphaMap.ConvertToGrayscale();
			lAlphaMap = &lAlphaMap;
		}
	}

	if (lStatusOk && mBumpMapList.IsEmpty() == false)
	{
		// TODO: Verify that the map is a normal map, and not a bump map.
		Map* lBumpMap = *mBumpMapList.First();
		lStatusOk = lLoader.Load(lBumpMap->mBitmapPath.ToCurrentCode(), lNormalMap);
		if (lStatusOk)
		{
			lNormalMap = &lNormalMap;
		}
	}

	if (lStatusOk && mShineMapList.IsEmpty() == false)
	{
		Map* lShineMap = *mShineMapList.First();
		lStatusOk = lLoader.Load(lShineMap->mBitmapPath.ToCurrentCode(), lSpecularMap);
		if (lStatusOk)
		{
			lSpecularMap.ConvertToGrayscale();
			lSpecularMap = &lSpecularMap;
		}
	}

	if (lStatusOk)
	{
		pTexture.Set(lColorMap, lAlphaMap, lNormalMap, lSpecularMap);
	}

	return lStatusOk;
}

Renderer::MaterialType ASELoader::Material::GetRecommendedMaterial()
{
	bool lDiffuse  = !mDiffuseMapList.IsEmpty();
	bool lSpecular = !mShineMapList.IsEmpty();
	bool lAlpha    = !mOpacityMapList.IsEmpty();
	bool lBump     = !mBumpMapList.IsEmpty();
	bool lLight    = !mSelfIllumMapList.IsEmpty() || !mAmbientMapList.IsEmpty();

	Renderer::MaterialType lMaterialType = Renderer::MAT_SINGLE_COLOR_SOLID;

	if (!lDiffuse)
	{
		// Single color or vertex color.
	}
	else if(lBump && lSpecular)
	{
		lMaterialType = Renderer::MAT_TEXTURE_SBMAP_PXS;
	}
	else if(lBump && !lSpecular)
	{
		lMaterialType = Renderer::MAT_TEXTURE_AND_DIFFUSE_BUMPMAP_PXS;
	}
	else if(lAlpha)
	{
		lMaterialType = Renderer::MAT_SINGLE_TEXTURE_BLENDED;
	}
	else if(lLight)
	{
		if (mShading == "Phong")
		{
			lMaterialType = Renderer::MAT_TEXTURE_AND_LIGHTMAP_PXS;
		}
		else
		{
			lMaterialType = Renderer::MAT_TEXTURE_AND_LIGHTMAP;
		}
	}
	else
	{
		if (mShading == "Phong")
		{
			lMaterialType = Renderer::MAT_SINGLE_TEXTURE_SOLID_PXS;
		}
		else
		{
			lMaterialType = Renderer::MAT_SINGLE_TEXTURE_SOLID;
		}
	}

	return lMaterialType;
}



ASELoader::NodeTM::NodeTM()
{
	Init();
}

ASELoader::NodeTM::~NodeTM()
{
}

void ASELoader::NodeTM::Init()
{
	mNodeName = "";
	mComment = "";
	mTMRotAngle = 0;
	mInheritPos[0] = 0;
	mInheritPos[1] = 0;
	mInheritPos[2] = 0;
	mInheritRot[0] = 0;
	mInheritRot[1] = 0;
	mInheritRot[2] = 0;
	mInheritScl[0] = 0;
	mInheritScl[1] = 0;
	mInheritScl[2] = 0;
	mTMRow0[0] = 1;
	mTMRow0[1] = 0;
	mTMRow0[2] = 0;
	mTMRow1[0] = 0;
	mTMRow1[1] = 1;
	mTMRow1[2] = 0;
	mTMRow2[0] = 0;
	mTMRow2[1] = 0;
	mTMRow2[2] = 1;
	mTMRow3[0] = 0;
	mTMRow3[1] = 0;
	mTMRow3[2] = 0;
	mTMPos[0] = 0;
	mTMPos[1] = 0;
	mTMPos[2] = 0;
	mTMRotAxis[0] = 0;
	mTMRotAxis[1] = 0;
	mTMRotAxis[2] = 0;
	mTMScale[0] = 1;
	mTMScale[1] = 1;
	mTMScale[2] = 1;
	mTMScaleAxis[0] = 0;
	mTMScaleAxis[1] = 0;
	mTMScaleAxis[2] = 0;
	mTMScaleAxisAng = 0;
}

void ASELoader::NodeTM::GetRotationMatrix(Lepra::RotationMatrixF& pRotMtx)
{
	// TODO: Verify this. We may need to take the transpose instead.
	pRotMtx.Set((float)mTMRow0[0], (float)mTMRow0[1], (float)mTMRow0[2],
		     (float)mTMRow1[0], (float)mTMRow1[1], (float)mTMRow1[2],
		     (float)mTMRow2[0], (float)mTMRow2[1], (float)mTMRow2[2]);
}

void ASELoader::NodeTM::GetRotationMatrix(Lepra::RotationMatrixD& pRotMtx)
{
	// TODO: Verify this. We may need to take the transpose instead.
	pRotMtx.Set(mTMRow0[0], mTMRow0[1], mTMRow0[2],
		     mTMRow1[0], mTMRow1[1], mTMRow1[2],
		     mTMRow2[0], mTMRow2[1], mTMRow2[2]);
}

void ASELoader::NodeTM::GetPosition(Lepra::Vector3DF& pPos)
{
	pPos.Set((float)mTMPos[0], (float)mTMPos[1], (float)mTMPos[2]);
}

void ASELoader::NodeTM::GetPosition(Lepra::Vector3DD& pPos)
{
	pPos.Set(mTMPos[0], mTMPos[1], mTMPos[2]);
}

void ASELoader::NodeTM::GetTransformation(Lepra::TransformationF& pTransform)
{
	// Since pTransform.GetPosition() returns a reference, the following works.
	GetPosition(pTransform.GetPosition());

	Lepra::RotationMatrixF lRotMtx;
	GetRotationMatrix(lRotMtx);
	lRotMtx.Reorthogonalize();
	pTransform.SetOrientation(lRotMtx);
}

void ASELoader::NodeTM::GetTransformation(Lepra::TransformationD& pTransform)
{
	// Since pTransform.GetPosition() returns a reference, the following works.
	GetPosition(pTransform.GetPosition());

	Lepra::RotationMatrixD lRotMtx;
	GetRotationMatrix(lRotMtx);
	lRotMtx.Reorthogonalize();
	pTransform.SetOrientation(lRotMtx);
}




ASELoader::ControlPosSample::ControlPosSample() :
	mTimeValue(0)
{
	mPos[0] = 0;
	mPos[1] = 0;
	mPos[2] = 0;
}

ASELoader::ControlPosSample::~ControlPosSample()
{
}

void ASELoader::ControlPosSample::Init()
{
	mTimeValue = 0;
	mPos[0] = 0;
	mPos[1] = 0;
	mPos[2] = 0;
}

ASELoader::ControlRotSample::ControlRotSample() :
	mTimeValue(0)
{
	mRot[0] = 0;
	mRot[1] = 0;
	mRot[2] = 0;
	mRot[3] = 0;
}

ASELoader::ControlRotSample::~ControlRotSample()
{
}

void ASELoader::ControlRotSample::Init()
{
	mTimeValue = 0;
	mRot[0] = 0;
	mRot[1] = 0;
	mRot[2] = 0;
	mRot[3] = 0;
}

ASELoader::TMAnimation::TMAnimation()
{
}

ASELoader::TMAnimation::~TMAnimation()
{
	Init();
}

void ASELoader::TMAnimation::Init()
{
	mNodeName = "";
	mComment = "";
	mControlPosTrack.DeleteAll();
	mControlRotTrack.DeleteAll();
}



ASELoader::MappingChannel::MappingChannel() :
	mComment(""),
	mNumTVertex(0),
	mTVertex(0),
	mNumTVFaces(0),
	mTVFace(0)
{
}

ASELoader::MappingChannel::~MappingChannel()
{
	Init();
}

void ASELoader::MappingChannel::Init()
{
	mComment = "";
	mNumTVertex = 0;
	if (mTVertex != 0)
	{
		delete[] mTVertex;
		mTVertex = 0;
	}
	mNumTVFaces = 0;
	if (mTVFace != 0)
	{
		delete[] mTVFace;
		mTVFace = 0;
	}
}

void ASELoader::MappingChannel::AllocVertexArray()
{
	if (mTVertex != 0)
	{
		delete[] mTVertex;
		mTVertex = 0;
	}

	if (mNumTVertex > 0)
	{
		mTVertex = new float[mNumTVertex * 3];
	}
}

void ASELoader::MappingChannel::AllocFaceArray()
{
	if (mTVFace != 0)
	{
		delete[] mTVFace;
		mTVFace = 0;
	}

	if (mNumTVFaces > 0)
	{
		mTVFace = new int[mNumTVFaces * 3];
	}
}



ASELoader::Mesh::Mesh() :
	mComment(""),
	mTimeValue(0),
	mNumVertex(0),
	mNumFaces(0),
	mVertex(0),
	mFace(0),
	mFaceEdgeVisible(0),
	mSmoothingGroupList(0),
	mSubMaterial(0),
	mHaveDefaultUVSet(false),
	mFaceNormals(0),
	mVertexNormals(0),
	mNumCVertex(0),
	mCVertex(0),
	mNumCVFaces(0),
	mCFace(0)
{
}

ASELoader::Mesh::~Mesh()
{
	Init();
}

void ASELoader::Mesh::Init()
{
	mComment = "";
	mTimeValue = 0;
	mNumVertex = 0;
	mNumFaces = 0;
	mHaveDefaultUVSet = false;

	if (mVertex != 0)
	{
		delete[] mVertex;
		mVertex = 0;
	}
	if (mFace != 0)
	{
		delete[] mFace;
		mFace = 0;
	}
	if (mFaceEdgeVisible != 0)
	{
		delete[] mFaceEdgeVisible;
		mFaceEdgeVisible = 0;
	}
	if (mSmoothingGroupList != 0)
	{
		delete[] mSmoothingGroupList;
		mSmoothingGroupList = 0;
	}
	if (mSubMaterial != 0)
	{
		delete[] mSubMaterial;
		mSubMaterial = 0;
	}
	if (mFaceNormals != 0)
	{
		delete[] mFaceNormals;
		mFaceNormals = 0;
	}
	if (mVertexNormals != 0)
	{
		delete[] mVertexNormals;
		mVertexNormals = 0;
	}
	mNumCVertex = 0;
	if (mCVertex != 0)
	{
		delete[] mCVertex;
		mCVertex = 0;
	}
	mNumCVFaces = 0;
	if (mCFace != 0)
	{
		delete[] mCFace;
		mCFace = 0;
	}

	mMappingChannelList.DeleteAll();
}

ASELoader::MappingChannel* ASELoader::Mesh::GetDefaultMappingChannel()
{
	MappingChannel* lMappingChannel;
	if (mMappingChannelList.IsEmpty() || mHaveDefaultUVSet == false)
	{
		lMappingChannel = new MappingChannel;
		mMappingChannelList.PushFront(lMappingChannel);
		mHaveDefaultUVSet = true;
	}
	else
	{
		lMappingChannel = *mMappingChannelList.First();
	}
	return lMappingChannel;
}

void ASELoader::Mesh::AllocVertexArray()
{
	if (mVertex != 0)
	{
		delete[] mVertex;
		mVertex = 0;
	}

	if (mNumVertex > 0)
	{
		mVertex = new float[mNumVertex * 3];
	}
}

void ASELoader::Mesh::AllocFaceArray()
{
	if (mFace != 0)
	{
		delete[] mFace;
		mFace = 0;
	}

	if (mFaceEdgeVisible != 0)
	{
		delete[] mFaceEdgeVisible;
		mFaceEdgeVisible = 0;
	}

	if (mSmoothingGroupList != 0)
	{
		delete[] mSmoothingGroupList;
		mSmoothingGroupList = 0;
	}

	if (mSubMaterial != 0)
	{
		delete[] mSubMaterial;
		mSubMaterial = 0;
	}

	if (mNumFaces > 0)
	{
		mFace = new int[mNumFaces * 3];
		mFaceEdgeVisible = new bool[mNumFaces * 3];
		mSmoothingGroupList = new std::list<int>[mNumFaces];
		mSubMaterial = new int[mNumFaces];
	}
}

void ASELoader::Mesh::AllocCVertexArray()
{
	if (mCVertex != 0)
	{
		delete[] mCVertex;
		mCVertex = 0;
	}

	if (mNumCVertex > 0)
	{
		mCVertex = new float[mNumCVertex * 3];
	}
}

void ASELoader::Mesh::AllocCFaceArray()
{
	if (mCFace != 0)
	{
		delete[] mCFace;
		mCFace = 0;
	}

	if (mNumCVFaces > 0)
	{
		mCFace = new int[mNumCVFaces * 3];
	}
}

void ASELoader::Mesh::AllocNormalArrays()
{
	if (mFaceNormals != 0)
	{
		delete[] mFaceNormals;
		mFaceNormals = 0;
	}

	if (mVertexNormals != 0)
	{
		delete[] mVertexNormals;
		mVertexNormals = 0;
	}

	if (mNumFaces > 0)
	{
		mFaceNormals = new float[mNumFaces * 3];
	}

	if (mNumVertex > 0)
	{
		mVertexNormals = new float[mNumVertex * 3];
	}
}

void ASELoader::Mesh::GetGeometry(std::list<TriangleBasedGeometry*>& pGeometryList)
{
	int lNumUVSets = mMappingChannelList.GetCount();

	Face* lFace = new Face[mNumFaces];
	FaceListTable lFaceListTable;
	SetupFaceListTable(lFace, lFaceListTable);

	// For each submaterial, build one TriangleBasedGeometry.
	FaceListTable::Iterator lIter;
	for (lIter = lFaceListTable.First(); lIter != lFaceListTable.End(); ++lIter)
	{
		// Create the vertex lookup table. This table maps a FaceVertex (the "old" vertex) to
		// an index into the newly created vertex list (see further down).
		typedef Lepra::HashTable<FaceVertex, int, FaceVertex> VertexLookupTable;
		VertexLookupTable mVertexLookupTable;

		FaceList* lFaceList = *lIter;

		// The vertex list, vertex color list and lists for all UV-sets.
		std::list<Lepra::Vector3DF> lVertexList;
		std::list<Lepra::Vector3DF> lVertexColorList;
		std::list<Lepra::Vector2DF>* lUVList = 0;

		if (lNumUVSets > 0)
		{
			lUVList = new std::list<Lepra::Vector2DF>[lNumUVSets];
		}

		Lepra::uint32* lIndex = new Lepra::uint32[lFaceList->GetCount() * 3];

		FaceList::Iterator lFaceIter;
		int lFace = 0;
		for (lFaceIter = lFaceList->First(); lFaceIter != lFaceList->End(); ++lFaceIter, ++lFace)
		{
			Face* lFace = *lFaceIter;
			
			// For each of the face's vertices.
			int i;
			for (i = 0; i < 3; i++)
			{
				// Check if we have a new vertex.
				VertexLookupTable::Iterator lVIter = mVertexLookupTable.Find(lFace->mVertex[i]);

				if (lVIter == mVertexLookupTable.End())
				{
					// This is a new vertex. Update the lookup table and store it.
					mVertexLookupTable.Insert(lFace->mVertex[i], lVertexList.GetCount());

					int lVIndex = lFace->mVertex[i].mVIndex;

					Lepra::Vector3DF lVertex(mVertex[lVIndex * 3 + 0],
								  mVertex[lVIndex * 3 + 1],
								  mVertex[lVIndex * 3 + 2]);
					lVertexList.PushBack(lVertex);

					// Store vertex color.
					if (mNumCVFaces > 0)
					{
						int lCIndex = lFace->mVertex[i].mCIndex;
						Lepra::Vector3DF lVertexColor(mCVertex[lCIndex * 3 + 0],
									       mCVertex[lCIndex * 3 + 1],
									       mCVertex[lCIndex * 3 + 2]);
						lVertexColorList.PushBack(lVertexColor);
					}

					// Store UV coordinates.
					MappingChannelList::Iterator lMIter = mMappingChannelList.First();
					for (int j = 0; j < lNumUVSets; ++j, ++lMIter)
					{
						MappingChannel* lMappingChannel = *lMIter;
						int lTIndex = lFace->mVertex[i].mTIndex[j];
						Lepra::Vector2DF lUVCoords(lMappingChannel->mTVertex[lTIndex * 3 + 0],
									    lMappingChannel->mTVertex[lTIndex * 3 + 1]);
						lUVList[j].PushBack(lUVCoords);
					}
				}

				lIndex[lFace * 3 + i] = *mVertexLookupTable.Find(lFace->mVertex[i]);
			}
		}

		// Finally, setup the necessary arrays and create a TriangleBasedGeometry.
		float* lVertex = new float[lVertexList.GetCount() * 3];
		std::list<Lepra::Vector3DF>::Iterator lVIter;
		int i;
		for (lVIter = lVertexList.First(), i = 0; lVIter != lVertexList.End(); ++lVIter, i+=3)
		{
			lVertex[i + 0] = (*lVIter).x;
			lVertex[i + 1] = (*lVIter).y;
			lVertex[i + 2] = (*lVIter).z;
		}

		Lepra::uint8* lVertexColor = 0;
		if (lVertexColorList.IsEmpty() == false)
		{
			lVertexColor = new Lepra::uint8[lVertexColorList.GetCount() * 3];
			for (lVIter = lVertexColorList.First(), i = 0; lVIter != lVertexColorList.End(); ++lVIter, i+=3)
			{
				lVertexColor[i + 0] = (Lepra::uint8)((*lVIter).x * 255.0f);
				lVertexColor[i + 1] = (Lepra::uint8)((*lVIter).y * 255.0f);
				lVertexColor[i + 2] = (Lepra::uint8)((*lVIter).z * 255.0f);
			}
		}

		float** lUVData = 0;
		if (lNumUVSets > 0)
		{
			lUVData = new float*[lNumUVSets];
			for (i = 0; i < lNumUVSets; i++)
			{
				lUVData[i] = new float[lUVList[i].GetCount() * 2];
				std::list<Lepra::Vector2DF>::Iterator lUVIter;
				int j;
				for (lUVIter = lUVList[i].First(), j = 0; lUVIter != lUVList[i].End(); ++lUVIter, j+=2)
				{
					lUVData[i][j + 0] = (*lUVIter).x;
					lUVData[i][j + 1] = (*lUVIter).y;
				}
			}
		}

		float* lUVData = 0;
		if (lUVData != 0)
			lUVData = lUVData[0];
		TriangleBasedGeometry* lGeometry = new TriangleBasedGeometry(lVertex, 0, lUVData, lVertexColor, Tbc::GeometryBase::COLOR_RGB, lIndex, lVertexList.GetCount(), lFaceList->GetCount(), Tbc::GeometryBase::TRIANGLES, Tbc::GeometryBase::GEOM_STATIC);

		for (i = 1; i < lNumUVSets; i++)
		{
			lGeometry->AddUVSet(lUVData[i]);
		}

		pGeometryList.PushBack(lGeometry);

		delete[] lVertex;
		delete[] lIndex;
		if (lVertexColor != 0)
		{
			delete[] lVertexColor;
		}
		if (lNumUVSets > 0)
		{
			for (i = 0; i < lNumUVSets; i++)
			{
				delete[] lUVData[i];
			}
			delete[] lUVData;
		}
		
		if (lUVList != 0)
		{
			delete[] lUVList;
		}
		delete lFaceList;
	}

	delete[] lFace;
}

void ASELoader::Mesh::SetupFaceListTable(Face* pFace, FaceListTable& pFaceListTable)
{
	// Setup all faces and put them in the list that corresponds to their material.
	int i;
	int lNumUVSets = mMappingChannelList.GetCount();
	for (i = 0; i < mNumFaces; i++)
	{
		pFace[i].mVertex[0].mVIndex = mFace[i * 3 + 0];
		pFace[i].mVertex[1].mVIndex = mFace[i * 3 + 1];
		pFace[i].mVertex[2].mVIndex = mFace[i * 3 + 2];
		pFace[i].mSubMaterial = mSubMaterial[i];

		if (mNumCVFaces > 0)
		{
			pFace[i].mVertex[0].mCIndex = mCFace[i * 3 + 0];
			pFace[i].mVertex[1].mCIndex = mCFace[i * 3 + 1];
			pFace[i].mVertex[2].mCIndex = mCFace[i * 3 + 2];
		}

		if (lNumUVSets > 0)
		{
			// Allocate memory for all uv sets.
			pFace[i].mVertex[0].SetNumUVSets(lNumUVSets);
			pFace[i].mVertex[1].SetNumUVSets(lNumUVSets);
			pFace[i].mVertex[2].SetNumUVSets(lNumUVSets);

			int j;
			std::list<MappingChannel*>::Iterator lIter = mMappingChannelList.First();
			for (j = 0; j < lNumUVSets; ++j, ++lIter)
			{
				MappingChannel* lMappingChannel = *lIter;
				pFace[i].mVertex[0].mTIndex[j] = lMappingChannel->mTVFace[i * 3 + 0];
				pFace[i].mVertex[1].mTIndex[j] = lMappingChannel->mTVFace[i * 3 + 1];
				pFace[i].mVertex[2].mTIndex[j] = lMappingChannel->mTVFace[i * 3 + 2];
			}
		}

		FaceListTable::Iterator lX = pFaceListTable.Find(pFace[i].mSubMaterial);
		if (lX == pFaceListTable.End())
		{
			FaceList* lFaceList = new FaceList;
			lFaceList->PushBack(&pFace[i]);
			pFaceListTable.Insert(pFace[i].mSubMaterial, lFaceList);
		}
		else
		{
			(*lX)->PushBack(&pFace[i]);
		}
	}
}

ASELoader::Mesh::Face::Face()
{
}

ASELoader::Mesh::Face::~Face()
{
}

ASELoader::Mesh::FaceVertex::FaceVertex() :
	mVIndex(0),
	mCIndex(0),
	mTIndex(0),
	mNumUVSets(0)
{
}

	ASELoader::Mesh::FaceVertex::FaceVertex(const FaceVertex& pOther)
{
	*this = pOther;
}

ASELoader::Mesh::FaceVertex::~FaceVertex()
{
	if (mTIndex != 0)
	{
		delete[] mTIndex;
	}
}

void ASELoader::Mesh::FaceVertex::SetNumUVSets(int pNumUVSets)
{
	if (mTIndex != 0)
	{
		delete[] mTIndex;
		mTIndex = 0;
	}
	
	mNumUVSets = pNumUVSets;

	if (mNumUVSets > 0)
	{
		mTIndex = new int[mNumUVSets];
	}
}

size_t ASELoader::Mesh::FaceVertex::GetHashCode(const FaceVertex& pKey)
{
	return (size_t)(pKey.mVIndex * 100 + pKey.mCIndex);
}

bool ASELoader::Mesh::FaceVertex::operator== (const FaceVertex& pOther) const
{
	bool lOk = (mVIndex == pOther.mVIndex && mCIndex == pOther.mCIndex && mNumUVSets == pOther.mNumUVSets);
	if (lOk)
	{
		for (int i = 0; lOk && i < mNumUVSets; i++)
		{
			lOk = (mTIndex[i] == pOther.mTIndex[i]);
		}
	}
	return lOk;
}

ASELoader::Mesh::FaceVertex& ASELoader::Mesh::FaceVertex::operator= (const FaceVertex& pOther)
{
	mVIndex = pOther.mVIndex;
	mCIndex = pOther.mCIndex;

	if (mTIndex != 0)
	{
		delete[] mTIndex;
		mTIndex = 0;
	}
	
	mNumUVSets = pOther.mNumUVSets;

	if (mNumUVSets > 0)
	{
		mTIndex = new int[mNumUVSets];
		for (int i = 0; i < mNumUVSets; i++)
		{
			mTIndex[i] = pOther.mTIndex[i];
		}
	}

	return *this;
}



ASELoader::GeomObject::GeomObject() :
	mNodeName(""),
	mComment(""),
	mMotionBlur(false),
	mCastShadow(false),
	mRecvShadow(false),
	mTMAnimation(0),
	mMaterialRef(0)
{
}

ASELoader::GeomObject::~GeomObject()
{
	Init();
}

void ASELoader::GeomObject::Init()
{
	mNodeName = "";
	mComment = "";
	mNodeTMList.DeleteAll();
	mMotionBlur = false;
	mCastShadow = false;
	mRecvShadow = false;
	if (mTMAnimation != 0)
	{
		delete mTMAnimation;
		mTMAnimation = 0;
	}
	mMaterialRef = 0;

	mMeshList.DeleteAll();
}

void ASELoader::GeomObject::AllocTMAnimation()
{
	if (mTMAnimation == 0)
	{
		mTMAnimation = new TMAnimation;
	}
	else
	{
		mTMAnimation->Init();
	}
}



ASELoader::CameraSettings::CameraSettings() :
	mTimeValue(0),
	mNear(0),
	mFar(0),
	mFOV(0),
	mTDist(0)
{
}

ASELoader::CameraSettings::~CameraSettings()
{
}

void ASELoader::CameraSettings::Init()
{
	mTimeValue = 0;
	mNear = 0;
	mFar = 0;
	mFOV = 0;
	mTDist = 0;
}

ASELoader::CameraObject::CameraObject() :
	mNodeName(""),
	mComment(""),
	mCameraType(""),
	mSettings(0),
	mTMAnimation(0)
{
}

ASELoader::CameraObject::~CameraObject()
{
	Init();
}

void ASELoader::CameraObject::Init()
{
	mNodeName = "";
	mComment = "";
	mCameraType = "";
	mNodeTMList.DeleteAll();

	if (mSettings != 0)
	{
		delete mSettings;
		mSettings = 0;
	}

	if (mTMAnimation != 0)
	{
		delete mTMAnimation;
		mTMAnimation = 0;
	}
}

void ASELoader::CameraObject::AllocCameraSettings()
{
	if (mSettings == 0)
	{
		mSettings = new CameraSettings;
	}
	else
	{
		mSettings->Init();
	}
}

void ASELoader::CameraObject::AllocTMAnimation()
{
	if (mTMAnimation == 0)
	{
		mTMAnimation = new TMAnimation;
	}
	else
	{
		mTMAnimation->Init();
	}
}



ASELoader::LightSettings::LightSettings() :
	mTimeValue(0),
	mIntens(0),
	mAspect(0),
	mHotSpot(0),
	mFalloff(0),
	mTDist(0),
	mMapBias(0),
	mMapRange(0),
	mMapSize(0),
	mRayBias(0)
{
	mColor[0] = 0;
	mColor[1] = 0;
	mColor[2] = 0;
}

ASELoader::LightSettings::~LightSettings()
{
}

void ASELoader::LightSettings::Init()
{
	mTimeValue = 0;
	mColor[0] = 0;
	mColor[1] = 0;
	mColor[2] = 0;
	mIntens = 0;
	mAspect = 0;
	mHotSpot = 0;
	mFalloff = 0;
	mTDist = 0;
	mMapBias = 0;
	mMapRange = 0;
	mMapSize = 0;
	mRayBias = 0;
}

ASELoader::LightObject::LightObject() :
	mNodeName(""),
	mComment(""),
	mLightType(""),
	mShadows(""),
	mUseLight(false),
	mSpotShape(""),
	mUseGlobal(false),
	mAbsMapBias(false),
	mOverShoot(false),
	mSettings(0),
	mTMAnimation(0)
{
}

ASELoader::LightObject::~LightObject()
{
	Init();
}

void ASELoader::LightObject::Init()
{
	mNodeName = "";
	mComment = "";
	mLightType = "";
	mNodeTMList.DeleteAll();

	mShadows = "";
	mUseLight = false;
	mSpotShape = "";
	mUseGlobal = false;
	mAbsMapBias = false;
	mOverShoot = false;

	if (mSettings != 0)
	{
		delete mSettings;
		mSettings = 0;
	}

	if (mTMAnimation != 0)
	{
		delete mTMAnimation;
		mTMAnimation = 0;
	}
}

void ASELoader::LightObject::AllocLightSettings()
{
	if (mSettings == 0)
	{
		mSettings = new LightSettings;
	}
	else
	{
		mSettings->Init();
	}
}

void ASELoader::LightObject::AllocTMAnimation()
{
	if (mTMAnimation == 0)
	{
		mTMAnimation = new TMAnimation;
	}
	else
	{
		mTMAnimation->Init();
	}
}



ASELoader::ASEData::ASEData() :
	m3DSMaxAsciiExport(0),
	mComment(""),
	mScene(0)
{
}

ASELoader::ASEData::~ASEData()
{
	Init();
}

void ASELoader::ASEData::Init()
{
	m3DSMaxAsciiExport = 0;
	mComment = "";
	if (mScene != 0)
	{
		delete mScene;
		mScene = 0;
	}

	mMaterialList.DeleteAll();
	mGeomList.DeleteAll();
	mCamList.DeleteAll();
	mLightList.DeleteAll();
}


}
