
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// TODO: implement compression.

// All packers' Pack() method return the number of bytes streamed.
// All packers' Unpack() method return the number of bytes consumed.
// Only even byte boundaries are used at present.



#pragma once

#include "String.h"
#include "Transformation.h"
#include "Vector3D.h"



namespace Lepra
{



class PackerTransformation
{
public:
	static int Pack(uint8* pDestination, const xform& pSource);
	static int Unpack(xform& pDestination, const uint8* pSource, int pSize);
};

class PackerVector
{
public:
	static int Pack(uint8* pDestination, const vec3& pSource);
	static int Unpack(vec3& pDestination, const uint8* pSource, int pSize);
};

class PackerInt32	// Single int32.
{
public:
	static int Pack(uint8* pDestination, int32 pSource);
	static int Unpack(int32& pDestination, const uint8* pSource, int pSize);
};

class PackerInt16	// Single int16 (even though "int" used as parameter).
{
public:
	static int Pack(uint8* pDestination, int pSource);
	static int Unpack(int& pDestination, const uint8* pSource, int pSize);
};

class PackerUInt16	// Single int16 (even though "int" used as parameter).
{
public:
	static int Pack(uint8* pDestination, int pSource);
	static int Unpack(int& pDestination, const uint8* pSource, int pSize);
};

class PackerReal
{
public:
	static int Pack(uint8* pDestination, float pSource);
	static int Unpack(float& pDestination, const uint8* pSource, int pSize);
};
/*class PackerRealNormalized	// Single normalized float [0,1).
{
public:
	static int Pack(uint8* pDestination, const float* pSource);
	static int Unpack(float* pDestination, const uint8* pSource, int pSize);
};*/

class PackerOctetString	// Byte array; also sufficient for ANSI strings.
{
public:
	static int Pack(uint8* pDestination, const uint8* pSource, unsigned pLength);
	static int Unpack(uint8* pDestination, const uint8* pSource, int pSize);
};

class PackerUnicodeString
{
public:
	static int Pack(uint8* pDestination, const str& pSource);
	static int UnpackRaw(str* pDestination, const uint8* pSource, int pSize);	// Destination may be NULL.
	static int Unpack(str& pDestination, const uint8* pSource, int pSize);
};



}
