/*
	Class:  GeometrySeparator
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games

	NOTES:

	This class only implements one function. This function used to
	be a member of TBC::GeometryBase, but since this was considered
	"erroneous design" due to its dependency to 
	UiTbc::TriangleBasedGeometry, it has been implemented this way
	instead. 
	
	The function itself may be quite useless, unless you are writing
	some kind of editor, but throwing away working code isn't something
	that I like to do.
*/

namespace TBC
{
class GeometryBase;
}

namespace UiTbc
{

class TriangleBasedGeometry;

class UiGeometrySeparator
{
public:
	// Splits pGeom into an array of TriangleBasedGeometry.
	// Each instance of TriangleBasedGeometry is an island in pGeom
	// (disconnected triangles).
	static TriangleBasedGeometry* GetSeparateObjects(TBC::GeometryBase& pGeom, int& pNumObjects);
};

} // End namespace.
