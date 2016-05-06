
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

// TODO: check if IEEE-doubles are stored in network byte order by the FPU.
//       If not - add ntohll() on each long long (__int64).



#include "pch.h"
#include "../include/packer.h"
#include "../include/lepraassert.h"
#include "../include/endian.h"



namespace lepra {



int PackerTransformation::Pack(uint8* destination, const xform& source) {
	typedef uint32 _T;
	int offset = 0;
	*(_T*)&destination[offset] = Endian::HostToBigF(source.GetOrientation().a);	offset += sizeof(_T);
	*(_T*)&destination[offset] = Endian::HostToBigF(source.GetOrientation().b);	offset += sizeof(_T);
	*(_T*)&destination[offset] = Endian::HostToBigF(source.GetOrientation().c);	offset += sizeof(_T);
	*(_T*)&destination[offset] = Endian::HostToBigF(source.GetOrientation().d);	offset += sizeof(_T);
	*(_T*)&destination[offset] = Endian::HostToBigF(source.GetPosition().x);		offset += sizeof(_T);
	*(_T*)&destination[offset] = Endian::HostToBigF(source.GetPosition().y);		offset += sizeof(_T);
	*(_T*)&destination[offset] = Endian::HostToBigF(source.GetPosition().z);		offset += sizeof(_T);
	return (offset);
}

int PackerTransformation::Unpack(xform& destination, const uint8* source, int size) {
	typedef uint32 _T;
	int offset = 0;
	if (size >= (int)sizeof(_T)*7) {
		float data[4];
		data[0] = Endian::BigToHostF(*(_T*)&source[offset]);				offset += sizeof(_T);
		data[1] = Endian::BigToHostF(*(_T*)&source[offset]);				offset += sizeof(_T);
		data[2] = Endian::BigToHostF(*(_T*)&source[offset]);				offset += sizeof(_T);
		data[3] = Endian::BigToHostF(*(_T*)&source[offset]);				offset += sizeof(_T);
		destination.GetOrientation().Set(data[0], data[1], data[2], data[3]);
		destination.GetPosition().x = Endian::BigToHostF(*(_T*)&source[offset]);	offset += sizeof(_T);
		destination.GetPosition().y = Endian::BigToHostF(*(_T*)&source[offset]);	offset += sizeof(_T);
		destination.GetPosition().z = Endian::BigToHostF(*(_T*)&source[offset]);	offset += sizeof(_T);
	} else {
		offset = -1;
	}
	return (offset);
}



int PackerVector::Pack(uint8* destination, const vec3& source) {
	// TODO: improve packer.
	typedef uint32 _T;
	int offset = 0;
	*(_T*)&destination[offset] = Endian::HostToBigF(source.x);	offset += sizeof(_T);
	*(_T*)&destination[offset] = Endian::HostToBigF(source.y);	offset += sizeof(_T);
	*(_T*)&destination[offset] = Endian::HostToBigF(source.z);	offset += sizeof(_T);
	return (offset);
}

int PackerVector::Unpack(vec3& destination, const uint8* source, int size) {
	typedef uint32 _T;
	int offset = 0;
	if (size >= (int)sizeof(_T)*3) {
		destination.x = Endian::BigToHostF(*(_T*)&source[offset]);	offset += sizeof(_T);
		destination.y = Endian::BigToHostF(*(_T*)&source[offset]);	offset += sizeof(_T);
		destination.z = Endian::BigToHostF(*(_T*)&source[offset]);	offset += sizeof(_T);
	} else {
		offset = -1;
	}
	return (offset);
}



int PackerInt32::Pack(uint8* destination, int32 source) {
	// TODO: fix byte order.
	*(uint32*)destination = Endian::HostToBig(source);
	return (sizeof(source));
}

int PackerInt32::Unpack(int32& destination, const uint8* source, int size) {
	// TODO: fix byte order.
	int _size = -1;
	if (size >= (int)sizeof(destination)) {
		_size = sizeof(destination);
		destination = Endian::BigToHost(*(uint32*)source);
	} else {
		_size = -1;
	}
	return (_size);
}



int PackerInt16::Pack(uint8* destination, int source) {
	*(int16*)destination = Endian::HostToBig((int16)source);
	return (sizeof(int16));
}

int PackerInt16::Unpack(int& destination, const uint8* source, int size) {
	// TODO: fix byte order.
	int _size = -1;
	if (size >= (int)sizeof(int16)) {
		_size = sizeof(int16);
		destination = Endian::BigToHost(*(int16*)source);
	} else {
		_size = -1;
	}
	return (_size);
}



int PackerUInt16::Pack(uint8* destination, int source) {
	*(uint16*)destination = Endian::HostToBig((uint16)source);
	return (sizeof(uint16));
}

int PackerUInt16::Unpack(int& destination, const uint8* source, int size) {
	int _size = -1;
	if (size >= (int)sizeof(uint16)) {
		_size = sizeof(uint16);
		destination = Endian::BigToHost(*(uint16*)source);
	} else {
		_size = -1;
	}
	return (_size);
}



int PackerReal::Pack(uint8* destination, float source) {
	*(uint32*)destination = Endian::HostToBigF(source);
	return (sizeof(source));
}

int PackerReal::Unpack(float& destination, const uint8* source, int size) {
	int _size;
	if (size >= (int)sizeof(destination)) {
		_size = sizeof(destination);
		destination = Endian::BigToHostF(*(uint32*)source);
	} else {
		_size = -1;
	}
	return (_size);
}



int PackerOctetString::Pack(uint8* destination, const uint8* source, unsigned _length) {
	destination[0] = (uint8)_length;
	destination[1] = (uint8)(_length>>8);
	::memcpy(destination+2, source, _length);
	return (2+_length);
}

int PackerOctetString::Unpack(uint8* destination, const uint8* source, int size) {
	int _size = -1;
	if (size >= 3) {
		unsigned __length = source[0]|(((unsigned)source[1])<<8);
		if (__length >= 1 && (int)__length+2 <= size) {
			_size = 2+__length;
			if (destination) {
				::memcpy(destination, source+2, __length);
			}
		}
	}
	return (_size);
}



int PackerUnicodeString::Pack(uint8* destination, const str& source) {
	const str utf8 = source;
	const size_t char_count = utf8.length()+1;
	if (destination) {
		destination[0] = (uint8)char_count;
		destination[1] = (uint8)(char_count>>8);
		::memcpy(destination+2, utf8.c_str(), char_count-1);
		destination[2+char_count-1] = '\0';
	}
	return ((2+(int)char_count+3) & (~3));
}

int PackerUnicodeString::UnpackRaw(str* destination, const uint8* source, int size) {
	int _size = -1;
	if (size >= 3) {
		const int char_count = source[0]|(((unsigned)source[1])<<8);
		if (char_count >= 1 && char_count+2 <= size && source[2+char_count-1] == '\0') {
			_size = (2+char_count+3) & (~3);
			// TODO: catch UTF-8 encoding errors (might be DoS attempts).
			const str conversion = (const char*)(source+2);
			if (destination) {
				*destination = conversion;
			}
		}
	}
	deb_assert(_size > 0);
	return (_size);
}

int PackerUnicodeString::Unpack(str& destination, const uint8* source, int size) {
	return (UnpackRaw(&destination, source, size));
}



}
