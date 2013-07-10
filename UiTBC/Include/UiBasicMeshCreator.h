/*
	Class:  BasicMeshCreator
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#ifndef UIBASICMESHCREATOR_H
#define UIBASICMESHCREATOR_H

namespace UiTbc
{

class AnimatedGeometry;
class TriangleBasedGeometry;

class BasicMeshCreator
{
public:
	static TriangleBasedGeometry* CreateFlatBox(float pXSize, float pYSize, float pZSize,
						unsigned pXSegments = 1,
						unsigned pYSegments = 1,
						unsigned pZSegments = 1);

	static TriangleBasedGeometry* CreateEllipsoid(float pXRadius, 
						      float pYRadius, 
						      float pZRadius,
						      unsigned pNumLatitudeSegments,
						      unsigned pNumLongitudeSegments);
	static TriangleBasedGeometry* CreateCone(float pBaseRadius, 
						 float pHeight, 
						 unsigned pNumSegments);
	static TriangleBasedGeometry* CreateCylinder(float pBaseRadius, 
						     float pTopRadius, 
						     float pHeight,
						     unsigned pNumSegments);
	static TriangleBasedGeometry* CreateTorus(float pRadius, 
						  float pRingRadiusX,
						  float pRingRadiusY,
						  unsigned pNumSegments,
						  unsigned pNumRingSegments);

	static void CreateYBonedSkin(float pStartY, float pEndY, const TriangleBasedGeometry* pGeometry,
		AnimatedGeometry* pSkin, int pBoneCount, float pBoneStiffness = 1.1);

	// TODO: Implement more. Capsule, tube
};

} // End namespace.

#endif