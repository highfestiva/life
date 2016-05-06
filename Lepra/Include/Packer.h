
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// TODO: implement compression.

// All packers' Pack() method return the number of bytes streamed.
// All packers' Unpack() method return the number of bytes consumed.
// Only even byte boundaries are used at present.



#pragma once

#include "string.h"
#include "transformation.h"
#include "vector3d.h"



namespace lepra {



class PackerTransformation {
public:
	static int Pack(uint8* destination, const xform& source);
	static int Unpack(xform& destination, const uint8* source, int size);
};

class PackerVector {
public:
	static int Pack(uint8* destination, const vec3& source);
	static int Unpack(vec3& destination, const uint8* source, int size);
};

class PackerInt32 {	// Single int32.
public:
	static int Pack(uint8* destination, int32 source);
	static int Unpack(int32& destination, const uint8* source, int size);
};

class PackerInt16 {	// Single int16 (even though "int" used as parameter).
public:
	static int Pack(uint8* destination, int source);
	static int Unpack(int& destination, const uint8* source, int size);
};

class PackerUInt16 {	// Single int16 (even though "int" used as parameter).
public:
	static int Pack(uint8* destination, int source);
	static int Unpack(int& destination, const uint8* source, int size);
};

class PackerReal {
public:
	static int Pack(uint8* destination, float source);
	static int Unpack(float& destination, const uint8* source, int size);
};
/*class PackerRealNormalized {	// Single normalized float [0,1).
public:
	static int Pack(uint8* destination, const float* source);
	static int Unpack(float* destination, const uint8* source, int size);
};*/

class PackerOctetString {	// Byte array; also sufficient for ANSI strings.
public:
	static int Pack(uint8* destination, const uint8* source, unsigned length);
	static int Unpack(uint8* destination, const uint8* source, int size);
};

class PackerUnicodeString {
public:
	static int Pack(uint8* destination, const str& source);
	static int UnpackRaw(str* destination, const uint8* source, int size);	// Destination may be NULL.
	static int Unpack(str& destination, const uint8* source, int size);
};



}
