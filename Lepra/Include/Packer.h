
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

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
	static int Pack(uint8* pDestination, const TransformationF& pSource);
	static int Unpack(TransformationF& pDestination, const uint8* pSource, int pSize);
};

class PackerVector
{
public:
	static int Pack(uint8* pDestination, const Vector3DF& pSource);
	static int Unpack(Vector3DF& pDestination, const uint8* pSource, int pSize);
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
	static int Pack(uint8* pDestination, const wstr& pSource);
	static int UnpackRaw(wstr* pDestination, const uint8* pSource, int pSize);	// Destination may be NULL.
	static int Unpack(wstr& pDestination, const uint8* pSource, int pSize);
	static int Unpack(astr& pDestination, const uint8* pSource, int pSize);
};



}
