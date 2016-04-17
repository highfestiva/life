/*
	Class:  ASELoader
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	ASE = Ascii Scene Export.

	ASE is a common 3D file format which is native to 3D Studio MAX, but
	supported by most 3D-rendering software, except Lightwave perhaps. 
	Maya, ActorX and UnrealEd, just to mention a few.

	Two sites that cover parts of the ASE file format are (2007-07-24):
	http://www.unrealwiki.com/wiki/ASE_File_Format
	http://www.solosnake.com/main/ase.htm

	The best way to understand this file format is to learn how 3DSMAX 
	works. The data stored in an ASE-file (and the data structures 
	within) seems to reflect the internal datastructures of 3DSMAX
	quite well.

	Obviously, this format is not well suited for games. The data needs to
	be converted into something more appropriate. The loading procedure is 
	therefore divided into two parts:

	1. First parse the file, and read the data without modification 
	into a bunch of data structures (see the subclasses of ASELoader). 
	TODO: There are more tags to implement. Bones for instance...

	2. The user can choose between interpreting this data himself or 
	to call the default conversion methods.
*/

#pragma once

#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/HashTable.h"
#include "../../Lepra/Include/Reader.h"
#include "../../Lepra/Include/Writer.h"
#include "../../Lepra/Include/RotationMatrix.h"
#include "../../Lepra/Include/Vector3D.h"
#include "../../Lepra/Include/Transformation.h"
#include "UiRenderer.h"
#include <list>

namespace UiTbc
{

class TriangleBasedGeometry;
class Texture;

class ASELoader
{
public:
	class ASEData;

	ASELoader();

	bool Load(ASEData& pASEData, const Lepra::String& pFileName);
	bool Load(ASEData& pASEData, Lepra::Reader& pReader);
	// bool Save(const GeometryList& pGeometryList, const Lepra::AnsiString& pFileName);

	class Scene
	{
	public:
		Scene();
		~Scene();

		void Init();

		// All fields in this struct may be ignored.

		Lepra::AnsiString mFilename; // Orignal .max file name that this ASE was generated from.
		Lepra::AnsiString mComment;
		int mFirstFrame;	// First frame of animation?
		int mLastFrame;	// Last frame of animation?
		int mFrameSpeed;	// FPS?
		int mTicksPerFrame;	// Unknown.
		// Color vectors.
		double mBackgroundStatic[3]; // Background color.
		double mAmbientStatic[3]; // Ambient light.
	};

	class Map
	{
	public:
		Map();
		~Map();

		void Init();

		Lepra::AnsiString mName;
		Lepra::AnsiString mClass;
		Lepra::AnsiString mComment;

		int mSubNo; // Unknown.

		double mAmount; // Unknown. Presumed to be a global alpha value for this map.

		Lepra::AnsiString mBitmapPath; // Path to texture file.

		Lepra::AnsiString mType; // Unknown. Observed value is "Screen" (without quotes).

		double mUOffset;
		double mVOffset;
		double mUTiling;
		double mVTiling;
		double mUVWAngle;
		double mUVWBlur;
		double mUVWBlurOffset;
		double mUVWNoiseAmt;
		double mUVWNoiseSize;
		int mUVWNoiseLevel;
		double mUVWNoisePhase;

		Lepra::AnsiString mBitmapFilter; // Ignore. Observer value is "Pyramidal" (without quotes).
	};

	class Material
	{
	public:
		Material();
		~Material();

		void Init();

		bool LoadAsTexture(Texture& pTexture);
		Renderer::MaterialType GetRecommendedMaterial();

		Lepra::AnsiString mName;  // Ignore.
		Lepra::AnsiString mClass; // Ignore.
		Lepra::AnsiString mComment;

		// Ambient glow. Unknown if this adds to or overrides *SCENE_AMBIENT_STATIC.
		double mAmbient[3];
		double mDiffuse[3]; // Diffuse color of the material.
		double mSpecular[3]; // Specular color... Should always be the same as the diffuse.
		double mShine; // Specifies how focused the specular highlight is. Ignore.
		double mShineStrength; // Unknown. Ignore.
		double mTransparency;
		double mWireSize;

		typedef std::list<Material*> MaterialList;
		MaterialList mSubMaterialList;

		Lepra::AnsiString mShading; // Blinn or Phong. More possibilities (not observed though) could be
					 // Anisotropoc, Metal, Multi-Layer, Oren-Nayar-Blinn, Strauss, Sat or None.
		double mXPFalloff; // Some kind of exponential falloff. But of what?
		double mSelfIllum; // Self illumination.

		// Unknown. Observed value is "In" (without quotes).
		Lepra::AnsiString mFalloff; 

		// Unknown. Observed value is "Filter" (without quotes).
		Lepra::AnsiString mXPType;

		// The maps are stored in lists since a test scene showed that several
		// refract maps can occur in the same material. So to keep things
		// consistent, all maps are stored in lists. The common case is that
		// each list contain zero or one map each.
		typedef std::list<Map*> MapList;
		MapList mAmbientMapList;
		MapList mDiffuseMapList; // Standard texture.
		MapList mSpecularMapList;// Specular color. Ignore.
		MapList mShineMapList;   // Specularity per texel.
		MapList mShineStrengthMapList;
		MapList mSelfIllumMapList;
		MapList mOpacityMapList;
		MapList mFilterColorMapList;
		MapList mBumpMapList;
		MapList mReflectMapList;
		MapList mRefractMapList;
	};

	// TM = Transformation Matrix.
	class NodeTM
	{
	public:
		NodeTM();
		~NodeTM();

		void Init();
		void GetRotationMatrix(Lepra::RotationMatrixF& pRotMtx);
		void GetRotationMatrix(Lepra::RotationMatrixD& pRotMtx);
		void GetPosition(Lepra::Vector3DF& pPos);
		void GetPosition(Lepra::Vector3DD& pPos);
		void GetTransformation(Lepra::TransformationF& pTransform);
		void GetTransformation(Lepra::TransformationD& pTransform);

		Lepra::AnsiString mNodeName; // Same as GeomObject::mNodeName.
		Lepra::AnsiString mComment;
		int mInheritPos[3]; // Unknown. May be ignored.
		int mInheritRot[3]; // Unknown. May be ignored.
		int mInheritScl[3]; // Unknown. May be ignored.
		double mTMRow0[3]; // First row of transform matrix.
		double mTMRow1[3]; // Second row...
		double mTMRow2[3]; // Third row...
		double mTMRow3[3]; // Fourth row, same as mTMPos.
		double mTMPos[3]; // The position.
                // Another way to represent rotation:
		double mTMRotAxis[3]; // Axis of rotation. Not needed if you use the matrix.
		double mTMRotAngle; // Angle of rotation around the rotation axis. 

		double mTMScale[3]; // Scale of the transformation. This info is already 
		                     // embedded in the matrix. May be ignored.
		double mTMScaleAxis[3]; // Axis of scaling, whatever that is. Ignore this
					 // and just go with the matrix.
		double mTMScaleAxisAng; // Ditto.
	};

	class ControlPosSample
	{
	public:
		ControlPosSample();
		~ControlPosSample();

		void Init();

		int mTimeValue;
		double mPos[3];
	};

	class ControlRotSample
	{
	public:
		ControlRotSample();
		~ControlRotSample();

		void Init();

		int mTimeValue;
		double mRot[4];
	};

	class TMAnimation
	{
	public:
		TMAnimation();
		~TMAnimation();

		void Init();

		Lepra::AnsiString mNodeName; // Same as GeomObject::mNodeName.
		Lepra::AnsiString mComment;

		typedef std::list<ControlPosSample*> ControlPosSampleList;
		typedef std::list<ControlRotSample*> ControlRotSampleList;
		ControlPosSampleList mControlPosTrack;
		ControlRotSampleList mControlRotTrack;
	};

	class MappingChannel
	{
	public:
		MappingChannel();
		~MappingChannel();

		void Init();

		void AllocVertexArray();
		void AllocFaceArray();

		Lepra::AnsiString mComment;
		// Number of texture coordinates. Doesn't equal mNumVertex.
		// Texture coordinates are per triangle, not per vertex.
		int mNumTVertex; 
		float* mTVertex; // Texture coordinates UVW. U and V are the only ones needed.

		int mNumTVFaces; // Must equal mNumFaces.
		int* mTVFace; // Stores indices into mTVertex.
	};

	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		void Init();
		MappingChannel* GetDefaultMappingChannel();
		void AllocVertexArray();
		void AllocFaceArray();
		void AllocCVertexArray();
		void AllocCFaceArray();
		void AllocNormalArrays();

		void GetGeometry(std::list<TriangleBasedGeometry*>& pGeometryList);

		Lepra::AnsiString mComment;
		int mTimeValue; // Time tag of animation.
		int mNumVertex;
		int mNumFaces;
		float* mVertex;
		int* mFace; // Only stores indices.
		bool* mFaceEdgeVisible; // 3 bools per face, telling if the edge should
					  // be rendered or not. Ignore this.

		// Smoothing groups per face.
		std::list<int>* mSmoothingGroupList;
		int* mSubMaterial; // Sub material per face.

		// All UV-sets.
		bool mHaveDefaultUVSet;
		typedef std::list<MappingChannel*> MappingChannelList;
		MappingChannelList mMappingChannelList;

		float* mFaceNormals;
		float* mVertexNormals;

		int mNumCVertex; // Number of vertex colors.
		float* mCVertex; // Vertex colors.
		int mNumCVFaces; // Must equal mNumFaces.
		int* mCFace; // Vertex colors per face.
	private:
		// Used in GetGeometry().
		class FaceVertex
		{
		public:
			int mVIndex; // Vertex index.
			int mCIndex; // Color index.
			int* mTIndex; // Texture coordinate index per UV-set.
			int mNumUVSets;

			FaceVertex();
			FaceVertex(const FaceVertex& pOther);
			~FaceVertex();

			void SetNumUVSets(int pNumUVSets);
			static size_t GetHashCode(const FaceVertex& pKey);
			bool operator== (const FaceVertex& pOther) const;
			FaceVertex& operator= (const FaceVertex& pOther);
		};

		class Face
		{
		public:
			Face();
			~Face();
			int mSubMaterial;
			FaceVertex mVertex[3];
		};

		typedef std::list<Face*> FaceList;
		typedef Lepra::HashTable<int, FaceList*> FaceListTable;
		void SetupFaceListTable(Face* pFaceArray, FaceListTable& pFaceListTable);
	};

	class GeomObject
	{
	public:
		GeomObject();
		~GeomObject();

		void Init();
		void AllocTMAnimation();

		void GetGeometry(std::list<TriangleBasedGeometry*>& pGeometryList);

		Lepra::AnsiString mNodeName;
		// TODO: Add Lepra::AnsiString mNodeParent;
		Lepra::AnsiString mComment;
		typedef std::list<NodeTM*> NodeTMList;
		typedef std::list<Mesh*> MeshList;
		NodeTMList mNodeTMList;
		MeshList mMeshList;
		bool mMotionBlur; // Ignore.
		bool mCastShadow;
		bool mRecvShadow; // Ignore.
		TMAnimation* mTMAnimation;
		int mMaterialRef; // Default material index for those faces (triangles)
				    // that doesn't have a MTLID tag.
	};

	class CameraSettings
	{
	public:
		CameraSettings();
		~CameraSettings();

		void Init();

		int mTimeValue;
		double mNear;
		double mFar;
		double mFOV;
		double mTDist;
	};

	class CameraObject
	{
	public:
		CameraObject();
		~CameraObject();

		void Init();
		void AllocCameraSettings();
		void AllocTMAnimation();

		Lepra::AnsiString mNodeName;
		Lepra::AnsiString mComment;
		Lepra::AnsiString mCameraType; // Observed values: Target...
		typedef std::list<NodeTM*> NodeTMList;
		NodeTMList mNodeTMList;
		TMAnimation* mTMAnimation;
		CameraSettings* mSettings;
	};

	class LightSettings
	{
	public:
		LightSettings();
		~LightSettings();

		void Init();

		int mTimeValue;
		double mColor[3];
		double mIntens;
		double mAspect;
		double mHotSpot;
		double mFalloff;
		double mTDist;
		double mMapBias;
		double mMapRange;
		double mMapSize;
		double mRayBias;
	};

	class LightObject
	{
	public:
		LightObject();
		~LightObject();

		void Init();
		void AllocLightSettings();
		void AllocTMAnimation();

		Lepra::AnsiString mNodeName;
		Lepra::AnsiString mComment;
		Lepra::AnsiString mLightType; // Observed values: Target...
		typedef std::list<NodeTM*> NodeTMList;
		NodeTMList mNodeTMList;

		Lepra::AnsiString mShadows;
		bool mUseLight;
		Lepra::AnsiString mSpotShape;
		bool mUseGlobal;
		bool mAbsMapBias;
		bool mOverShoot;
		LightSettings* mSettings;
		TMAnimation* mTMAnimation;
	};

	class ASEData
	{
	public:
		ASEData();
		~ASEData();

		void Init();

		int m3DSMaxAsciiExport; // = 200
		Lepra::AnsiString mComment;
	
		Scene* mScene;

		typedef std::list<Material*> MaterialList;
		typedef std::list<GeomObject*> GeomObjectList;
		typedef std::list<CameraObject*> CameraObjectList;
		typedef std::list<LightObject*> LightObjectList;
		MaterialList mMaterialList;
		GeomObjectList mGeomList;
		CameraObjectList mCamList;
		LightObjectList mLightList;
	};

private:

	bool Parse(ASEData& pASEData, const Lepra::AnsiString& pDataString);

	bool ReadQuotedString(Lepra::AnsiString& pString, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadUnquotedString(Lepra::AnsiString& pString, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadInt(int& pInt, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadIntVec(int* pInt, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadFloat(double& pFloat, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadFloatVec(double* pFloat, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadBool(bool& pBool, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);

	// Vertex lists look the same, and all "lightweight" face lists 
	// (MESH_FACE_LIST is the only exception) look the same. Thus, 
	// we don't need to implement a unique function for each type.
	bool ReadVertexList(float* pVertex, int pNumVertex, char* pVertexIdentifier, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadFaceList(int* pFace, int pNumFaces, char* pFaceIdentifier, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);

	bool ReadSCENE(ASEData& pASEData, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadMATERIAL_LIST(ASEData& pASEData, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadMATERIAL(Material* pMaterial, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadMAP(std::list<Map*>& pMapList, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadGEOMOBJECT(GeomObject* pGeomObj, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadCAMERAOBJECT(CameraObject* pCamObj, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadLIGHTOBJECT(LightObject* pLightObj, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadCAMERA_SETTINGS(CameraSettings* pCamSettings, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadLIGHT_SETTINGS(LightSettings* pLightSettings, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadNODE_TM(NodeTM* pNodeTM, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadTM_ANIMATION(TMAnimation* pTMAnimation, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadCONTROL_POS_TRACK(TMAnimation* pTMAnimation, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadCONTROL_ROT_TRACK(TMAnimation* pTMAnimation, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadCONTROL_POS_SAMPLE(ControlPosSample* pControlPosSample, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadCONTROL_ROT_SAMPLE(ControlRotSample* pControlRotSample, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadMESH(Mesh* pMesh, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadMESH_FACE_LIST(Mesh* pMesh, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadMESH_NORMALS(Mesh* pMesh, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
	bool ReadMESH_MAPPINGCHANNEL(Mesh* pMesh, const Lepra::AnsiString& pDataString, int& pIndex, const Lepra::AnsiString& pSpecialTokens);
};

}
