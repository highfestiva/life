
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once

#include "../../UiCure/Include/UiCppContextObject.h"
#include "../Life.h"



namespace UiTbc
{
class GeometryBatch;
}



namespace Life
{



class MassObject: public UiCure::CppContextObject
{
	typedef UiCure::CppContextObject Parent;
public:
	MassObject(Cure::ResourceManager* pResourceManager, const str& pClassResourceName,
		UiCure::GameUiManager* pUiManager, TBC::PhysicsManager::BodyID pTerrainBodyId, size_t pInstanceCount,
		float pSideLength);
	virtual ~MassObject();

	void SetRender(bool pRender);
	virtual void UiMove();

private:
	void PositionToGrid(const Vector3DF& pPosition, int& pX, int& pY) const;
	void GridToPosition(int pX, int pY, Vector3DF& pPosition) const;
	void MoveToSquare(int pX, int pY);
	void CreateSquare(size_t pX, size_t pY);
	bool GetObjectPlacement(Vector3DF& pPosition) const;

	class Square
	{
	public:
		Square(uint32 pSeed, const MeshArray& pResourceArray,
			const std::vector<TransformationF>& pDisplacementArray, UiTbc::Renderer* pRenderer);
		~Square();
		void SetRender(bool pRender, float pAlpha);

	private:
		typedef std::pair<UiTbc::GeometryBatch*, UiTbc::Renderer::GeometryID> MassMeshPair;
		typedef std::vector<MassMeshPair> MassMeshArray;
		MassMeshArray mMassMeshArray;
		UiTbc::Renderer* mRenderer;
		bool mDisableTransparency;
	};

	enum
	{
		SQUARE_MID_TO_CORNER	= 3,
		SQUARE_SIDE		= 7,
		SQUARE_COUNT		= SQUARE_SIDE*SQUARE_SIDE,
	};

	TBC::PhysicsManager::BodyID mTerrainBodyId;
	size_t mSquareInstanceCount;
	Square* mSquareArray[SQUARE_COUNT];
	const float mVisibleAddTerm;
	float mFullyVisibleDistance;
	float mVisibleDistanceFactor;
	int mSquareSideLength;
	int mMiddleSquareX;
	int mMiddleSquareY;

	LOG_CLASS_DECLARE();
};



}
