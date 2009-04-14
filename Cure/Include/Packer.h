
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games

// TODO: implement compression.

// All packers' Pack() method return the number of bytes streamed.
// All packers' Unpack() method return the number of bytes consumed.
// Only even byte boundaries are used at present.



#pragma once

#include "../../Lepra/Include/String.h"
#include "../../Lepra/Include/Transformation.h"
#include "../../Lepra/Include/Vector3D.h"



namespace Cure
{



class PackerTransformation
{
public:
	static int Pack(Lepra::uint8* pDestination, const Lepra::TransformationF& pSource);
	static int Unpack(Lepra::TransformationF& pDestination, const Lepra::uint8* pSource, int pSize);
};

class PackerVector
{
public:
	static int Pack(Lepra::uint8* pDestination, const Lepra::Vector3DF& pSource);
	static int Unpack(Lepra::Vector3DF& pDestination, const Lepra::uint8* pSource, int pSize);
};

class PackerInt32	// Single int32.
{
public:
	static int Pack(Lepra::uint8* pDestination, Lepra::int32 pSource);
	static int Unpack(Lepra::int32& pDestination, const Lepra::uint8* pSource, int pSize);
};

class PackerInt16	// Single int16 (even though "int" used as parameter).
{
public:
	static int Pack(Lepra::uint8* pDestination, int pSource);
	static int Unpack(int& pDestination, const Lepra::uint8* pSource, int pSize);
};

class PackerReal
{
public:
	static int Pack(Lepra::uint8* pDestination, float pSource);
	static int Unpack(float& pDestination, const Lepra::uint8* pSource, int pSize);
};
/*class PackerRealNormalized	// Single normalized float [0,1).
{
public:
	static int Pack(Lepra::uint8* pDestination, const float* pSource);
	static int Unpack(float* pDestination, const Lepra::uint8* pSource, int pSize);
};*/

class PackerOctetString	// Byte array; also sufficient for ANSI strings.
{
public:
	static int Pack(Lepra::uint8* pDestination, const Lepra::uint8* pSource, unsigned pLength);
	static int Unpack(Lepra::uint8* pDestination, const Lepra::uint8* pSource, int pSize);
};

class PackerUnicodeString
{
public:
	static int Pack(Lepra::uint8* pDestination, const Lepra::UnicodeString& pSource);
	static int Unpack(Lepra::UnicodeString* pDestination, const Lepra::uint8* pSource, int pSize);	// Destination may be NULL.
};



}
