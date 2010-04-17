
// Author: Alexander Hugestrand
// Copyright (c) 2002-2009, Righteous Games

// This class extends TBC::Model with one or more geometries of different types.
// It also provides functionality for the determination of the LOD-level.



#pragma once

#include "../../TBC/Include/Model.h"
#include "UiRenderer.h"



namespace UiTbc
{



class AnimatedGeometry;
class TriangleBasedGeometry;
class ProgressiveTriangleGeometry;



class GraphicalModel: public TBC::Model
{
public:

	// The geometry handler is responsible for uploading the geometry to the
	// renderer and updating LOD and animations. It gives the user the 
	// possibility to manage resources his/her own way. 
	class GeometryHandler
	{
	public:
		friend class GraphicalModel;

		GeometryHandler() :
			mTransformAnimator(0)
		{
		}
		virtual ~GeometryHandler()
		{
		}

		// The update function may reload the geometry, update the LOD
		// or animate the object.
		virtual void UpdateGeometry(float pLODLevel) = 0;
		virtual TBC::GeometryBase* GetGeometry() = 0;
	private:
		void SetTransformAnimator(TBC::BoneAnimator* pTransformAnimator)
		{
			mTransformAnimator = pTransformAnimator;
		}
		TBC::BoneAnimator* GetTransformAnimator() const
		{
			return mTransformAnimator;
		}
		TBC::BoneAnimator* mTransformAnimator;
	};

	GraphicalModel();
	virtual ~GraphicalModel();

	// Takes ownership of the geometry handler (takes care of destruction).
	// pTransformAnimator = The animator (see TBC::Model and BoneAnimator) to use for transform animations.
	void AddGeometry(const str& pName, GeometryHandler* pGeometry, const str& pTransformAnimator = _T(""));

	TBC::GeometryBase* GetGeometry(const str& pName);

	// Overloaded from TBC::Model where it is declared as public.
	void Update(double pDeltaTime);

	// Calculates the detail level depending on the camera and calls Update(double).
	// void Update(double pDeltaTime, Camera* pCamera);

	// Level of detail in the range [0, 1] where 0 equals the lowest level and 1 the highest.
	// This is automatically set through Update(double, Camera*).
	void SetDetailLevel(double pLevelOfDetail);

	void SetLastFrameVisible(unsigned int pLastFrameVisible);
	unsigned int GetLastFrameVisible() const;
	void SetAlwaysVisible(bool pAlwaysVisible);
	bool GetAlwaysVisible();

private:

	enum GeomType
	{
		GEOM_STATIC = 0,
		GEOM_PROGRESSIVE,
		GEOM_ANIMATED_STATIC,
		GEOM_ANIMATED_PROGRESSIVE
	};

	struct GeometryData
	{
		GeomType mGeomType;
		int mCurrentLODLevel;
		int mNumLODLevels;
		TBC::GeometryBase* mGeometry;
		Renderer::MaterialType mMaterialType;
		Renderer::GeometryID mGeomID;
		Renderer::TextureID* mTextureID;
		int mNumTextures;
		Renderer::Shadows mShadows;
		str mName;
		TBC::BoneAnimator* mTransformAnimator;
	};

	int CalcLODIndex(int pMaxIndex);

	typedef HashTable<str, GeometryHandler*, std::hash<str>, 8> GeometryTable;

	GeometryTable mGeometryTable;
	double mLevelOfDetail;
};



class DefaultStaticGeometryHandler : public GraphicalModel::GeometryHandler
{
public:
	DefaultStaticGeometryHandler(TriangleBasedGeometry* pGeometry,
				     int pNumLODLevels,
				     Renderer::TextureID* pTextureID,
				     int pNumTextures,
				     Renderer::MaterialType pMaterial, 
				     Renderer::Shadows pShadows,
				     Renderer* pRenderer);

	void UpdateGeometry(float pLODLevel);
	TBC::GeometryBase* GetGeometry();

private:
	TriangleBasedGeometry* mGeometry;
	int mNumLODLevels;
	Renderer::TextureID* mTextureID;
	int mNumTextures;
	Renderer::MaterialType mMaterial;
	Renderer::Shadows mShadows;

	int mCurrentLODLevel;
	Renderer::GeometryID mGeomID;
	Renderer* mRenderer;
};



class DefaultProgressiveGeometryHandler : public GraphicalModel::GeometryHandler
{
public:
	DefaultProgressiveGeometryHandler(ProgressiveTriangleGeometry* pGeometry, 
					  Renderer::MaterialType pMaterial,
					  Renderer::TextureID* pTextureID,
					  int pNumTextures,
					  Renderer::Shadows pShadows,
					  Renderer* pRenderer);

	void UpdateGeometry(float pLODLevel);
	TBC::GeometryBase* GetGeometry();

private:
	ProgressiveTriangleGeometry* mGeometry;

	Renderer::GeometryID mGeomID;
	Renderer* mRenderer;
};



class DefaultAnimatedStaticGeometryHandler : public GraphicalModel::GeometryHandler
{
public:
	DefaultAnimatedStaticGeometryHandler(AnimatedGeometry* pGeometry,
					     int pNumLODLevels,
					     Renderer::TextureID* pTextureID,
					     int pNumTextures,
					     Renderer::MaterialType pMaterial, 
					     Renderer::Shadows pShadows,
					     Renderer* pRenderer);

	void UpdateGeometry(float pLODLevel);
	TBC::GeometryBase* GetGeometry();
private:
	AnimatedGeometry* mGeometry;
	int mNumLODLevels;
	Renderer::TextureID* mTextureID;
	int mNumTextures;
	Renderer::MaterialType mMaterial;
	Renderer::Shadows mShadows;

	int mCurrentLODLevel;
	Renderer::GeometryID mGeomID;
	Renderer* mRenderer;
};



// This class assumes that AnimatedGeometry points to a progressive mesh.
// If it doesn't, program failure can be expected.
class DefaultAnimatedProgressiveGeometryHandler : public GraphicalModel::GeometryHandler
{
public:
	DefaultAnimatedProgressiveGeometryHandler(AnimatedGeometry* pGeometry,
						  Renderer::TextureID* pTextureID,
						  int pNumTextures,
						  Renderer::MaterialType pMaterial, 
						  Renderer::Shadows pShadows,
						  Renderer* pRenderer);

	void UpdateGeometry(float pLODLevel);
	TBC::GeometryBase* GetGeometry();
private:
	AnimatedGeometry* mGeometry;
	Renderer::TextureID* mTextureID;
	int mNumTextures;
	Renderer::MaterialType mMaterial;
	Renderer::Shadows mShadows;

	Renderer::GeometryID mGeomID;
	Renderer* mRenderer;
};



}
