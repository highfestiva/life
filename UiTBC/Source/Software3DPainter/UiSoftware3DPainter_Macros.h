/*
	Lepra::File:   UiSoftware3DPainter_Macros.h
	Class:  Software3DPainter
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand

	NOTES:

	Use macros for code used frequently to make life easier changing things.
	Most of the macro code once came from the triangle rendering functions.
	But I rewrote things like this to be able to implement an optimization
	of the triangle edges later on. Two neighbour triangles sharing the same
	edge should not require the edge to be initialized from scratch.

	The code to keep track on which edge that is the shared one got quite
	complicated though, and I gave it up for now. I don't think it will result
	in any dramatic speedup anyway.

	And to help you from the difficulty finding out what edge to use when rendering
	triangle strips, I want to say that it switches between TBC::Edge2 and Edge1,
	starting with TBC::Edge2.
*/

// lEdge0 = The edge between vertex 0 and 1
// lEdge1 = The edge between vertex 1 and 2
// lEdge2 = The edge between vertex 2 and 0
#define MACRO_SORT_EDGES(a, b, c, projectedVertex) \
{ \
	double lY0 = projectedVertex[a].GetY(); \
	double lY1 = projectedVertex[b].GetY(); \
	double lY2 = projectedVertex[c].GetY(); \
 \
	if (lY0 < lY1) \
	{ \
		if (lY2 < lY0) \
		{ \
			lTopToBottomEdge    = lEdge1; \
			lTopToMiddleEdge    = lEdge2; \
			lMiddleToBottomEdge = lEdge0; \
			lMiddleIsLeft = false; \
		} \
		else \
		{ \
			if (lY1 < lY2) \
			{ \
				lTopToBottomEdge    = lEdge2; \
				lTopToMiddleEdge    = lEdge0; \
				lMiddleToBottomEdge = lEdge1; \
				lMiddleIsLeft = false; \
			} \
			else \
			{ \
				lTopToBottomEdge    = lEdge0; \
				lTopToMiddleEdge    = lEdge2; \
				lMiddleToBottomEdge = lEdge1; \
				lMiddleIsLeft = true; \
			} \
		} \
	} \
	else \
	{ \
		if (lY2 < lY1) \
		{ \
			lTopToBottomEdge    = lEdge2; \
			lTopToMiddleEdge    = lEdge1; \
			lMiddleToBottomEdge = lEdge0; \
			lMiddleIsLeft = true; \
		} \
		else \
		{ \
 \
			if (lY0 < lY2) \
			{ \
				lTopToBottomEdge    = lEdge1; \
				lTopToMiddleEdge    = lEdge0; \
				lMiddleToBottomEdge = lEdge2; \
				lMiddleIsLeft = true; \
			} \
			else \
			{ \
				lTopToBottomEdge    = lEdge0; \
				lTopToMiddleEdge    = lEdge1; \
				lMiddleToBottomEdge = lEdge2; \
				lMiddleIsLeft = false; \
			} \
		} \
	} \
} \

#define MACRO_BLUR(r, g, b, prevK, KK, pr, pg, pb, kBuf) \
	unsigned int lK = prevK; \
	unsigned int lOneMinusK = 65535 - lK; \
	unsigned int lNewK = *kBuf; \
	if (lNewK < prevK) \
	{ \
		prevK = lNewK; \
	} \
	else \
	{ \
		prevK = ((prevK * KK + lNewK * (65535 - KK)) >> 16); \
	} \
 \
	pr = ((pr * lK + r * lOneMinusK) >> 16); \
	pg = ((pg * lK + g * lOneMinusK) >> 16); \
	pb = ((pb * lK + b * lOneMinusK) >> 16); \

