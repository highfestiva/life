
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// The base class TerrainFunction should be sub-classed; this is where the actual implementation goes.
// All terrain functions inherit a behavior where the range of the function is restricted by two radii.
// Within the InnerRadius, the function is unrestricted. In the area between the InnerRadius and the
// OuterRadius, the function must scale (typically linear) down to 0. Outside OuterRadius, the function
// must be 0.

// The method AddPoint() adds the value of the function to a given 3D point within a grid.

// The TerrainFunction base class calls the virtual method AddPoint() in the sub-class for each point
// it affects in the grid. The x and y parameters for AddPoint() have been scaled down to range [0,1],
// where the extents of the function intersect the axis-aligned square with side OuterRaduis*2.



#pragma once

#include "../../lepra/include/cubicdecasteljauspline.h"
#include "../../lepra/include/vector2d.h"
#include "../../lepra/include/vector3d.h"
#include "terrainpatch.h"
#include "tbc.h"



namespace tbc {

// The underlying function does generally not generate a hightfield, but instead a plain mesh.
// The whole terrain system is built around meshes, not hightfields.
class TerrainFunction : public TerrainPatch::Modifier {
public:
	TerrainFunction(float amplitude, const vec2& position, float inner_radius, float outer_radius);
	virtual ~TerrainFunction();

	void AddFunction(TerrainPatch& patch) const;

	float GetAmplitude() const;
	const vec2& GetPosition() const;
	float GetInnerRadius() const;
	float GetOuterRadius() const;

	// From TerrainPatch::Modifier
	void ModifyVertex(const vec2& world_flat_pos, vec3& vertex) const;

	// Parameters relative_normalized_x, relative_normalized_y and scale have been scaled to inclusive range [0,1].
	virtual void AddPoint(float relative_normalized_x, float relative_normalized_y, float scale, float absolute_xy_distance, vec3& point) const = 0;

protected:
	float amplitude_;
	vec2 position_;
	float inner_radius_;
	float outer_radius_;
};

class TerrainFunctionGroup : public TerrainPatch::Modifier {
public:
	// TODO: Add a linked list version?
	TerrainFunctionGroup(TerrainFunction** tf_array, int count,
		DataPolicy array_policy = kFullCopy,
		SubDataPolicy tf_policy = kKeepReference);
	~TerrainFunctionGroup();

	// Adds all functions at once.
	void AddFunctions(TerrainPatch& patch) const;

	void ModifyVertex(const vec2& world_flat_pos, vec3& vertex) const;

private:
	TerrainFunction** tf_array_;
	int count_;
	DataPolicy array_policy_;
	SubDataPolicy tf_policy_;
};

// Generates a cone without hang over (hehe), but using strech.
class TerrainConeFunction: public TerrainFunction {
public:
	TerrainConeFunction(float amplitude, const vec2& position, float inner_radius, float outer_radius);

private:
	void AddPoint(float relative_normalized_x, float relative_normalized_y, float scale, float absolute_xy_distance, vec3& point) const;

	float radius_differance_;
	float slope_length_;
	float profile_length_;
	float plateau_part_;
};

// Generates a hemisphere without hang over, but using strech.
class TerrainHemisphereFunction: public TerrainFunction {
public:
	TerrainHemisphereFunction(float amplitude, const vec2& position, float inner_radius, float outer_radius);

private:
	void AddPoint(float relative_normalized_x, float relative_normalized_y, float scale, float absolute_xy_distance, vec3& point) const;
};

// Generates a dune (from -X to +X) which is really a spline profile (in the XZ-plane).
class TerrainDuneFunction: public TerrainFunction {
public:
	TerrainDuneFunction(float width_proportion, float curvature, float amplitude, const vec2& position, float inner_radius, float outer_radius);
	virtual ~TerrainDuneFunction();

private:
	void AddPoint(float relative_normalized_x, float relative_normalized_y, float scale, float absolute_xy_distance, vec3& point) const;

	CubicDeCasteljauSpline<vec2, float>* profile_spline_;
	float width_proportion_;
};

// Decorator design pattern. Used for inheritance.
class TerrainDecorator {
public:
	TerrainDecorator(TerrainFunction* function);
	virtual ~TerrainDecorator();

protected:
	TerrainFunction* function_;
};

// Decorator design pattern. Multiplies with the underlying function (from -Y to +Y) with a one-dimensional spline (in the YZ-plane).
class TerrainAmplitudeFunction: public TerrainFunction, protected TerrainDecorator {
public:
	TerrainAmplitudeFunction(const float* amplitude_vector, unsigned amplitude_vector_length, TerrainFunction* function);
	virtual ~TerrainAmplitudeFunction();

private:
	void AddPoint(float relative_normalized_x, float relative_normalized_y, float scale, float absolute_xy_distance, vec3& point) const;

	CubicDeCasteljauSpline<float, float, float>* amplitude_spline_;
};

// Decorator design pattern. Compresses/streches the decorated function along the X-axis.
class TerrainWidthFunction: public TerrainFunction, protected TerrainDecorator {
public:
	TerrainWidthFunction(float width_factor, TerrainFunction* function);

private:
	void AddPoint(float relative_normalized_x, float relative_normalized_y, float scale, float absolute_xy_distance, vec3& point) const;

	float width_factor_;
};

// Decorator design pattern. Multiplies with the underlying function (from -Y to +Y) with a one-dimensional spline (in the YZ-plane).
class TerrainPushFunction: public TerrainFunction, protected TerrainDecorator {
public:
	TerrainPushFunction(const float* push_vector, unsigned push_vector_length, TerrainFunction* function);
	virtual ~TerrainPushFunction();

private:
	void AddPoint(float relative_normalized_x, float relative_normalized_y, float scale, float absolute_xy_distance, vec3& point) const;

	CubicDeCasteljauSpline<float, float, float>* push_spline_;
};

// Decorator design pattern. Transforms the underlying function (in the XY-plane) along a two-dimensional spline (in the XY-plane).
class TerrainRotateFunction: public TerrainFunction, protected TerrainDecorator {
public:
	TerrainRotateFunction(float angle, TerrainFunction* function);

private:
	void AddPoint(float relative_normalized_x, float relative_normalized_y, float scale, float absolute_xy_distance, vec3& point) const;

	float angle_;
};



}
