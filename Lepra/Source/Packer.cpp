
// Author: Jonas Bystr�m
// Copyright (c) 2002-2009, Righteous Games

// TODO: check if IEEE-doubles are stored in network byte order by the FPU.
//       If not - add ntohll() on each long long (__int64).



#include "../Include/Packer.h"
#include <assert.h>
#include "../Include/Endian.h"



namespace Lepra
{



int PackerTransformation::Pack(uint8* pDestination, const TransformationF& pSource)
{
	// TODO: improve packer.
	typedef TransformationF::BaseType _T;
	int lOffset = 0;
	*(_T*)&pDestination[lOffset] = pSource.GetOrientation().GetA();	lOffset += sizeof(_T);
	*(_T*)&pDestination[lOffset] = pSource.GetOrientation().GetB();	lOffset += sizeof(_T);
	*(_T*)&pDestination[lOffset] = pSource.GetOrientation().GetC();	lOffset += sizeof(_T);
	*(_T*)&pDestination[lOffset] = pSource.GetOrientation().GetD();	lOffset += sizeof(_T);
	*(_T*)&pDestination[lOffset] = pSource.GetPosition().x;	lOffset += sizeof(_T);
	*(_T*)&pDestination[lOffset] = pSource.GetPosition().y;	lOffset += sizeof(_T);
	*(_T*)&pDestination[lOffset] = pSource.GetPosition().z;	lOffset += sizeof(_T);
	return (lOffset);
}

int PackerTransformation::Unpack(TransformationF& pDestination, const uint8* pSource, int pSize)
{
	typedef TransformationF::BaseType _T;
	int lOffset = 0;
	if (pSize >= (int)sizeof(_T)*7)
	{
		pDestination.GetOrientation().Set(*(_T*)&pSource[lOffset+0*sizeof(_T)], *(_T*)&pSource[lOffset+1*sizeof(_T)],
			*(_T*)&pSource[lOffset+2*sizeof(_T)], *(_T*)&pSource[lOffset+3*sizeof(_T)]);
		lOffset += sizeof(_T)*4;
		pDestination.GetPosition().x = *(_T*)&pSource[lOffset];	lOffset += sizeof(_T);
		pDestination.GetPosition().y = *(_T*)&pSource[lOffset];	lOffset += sizeof(_T);
		pDestination.GetPosition().z = *(_T*)&pSource[lOffset];	lOffset += sizeof(_T);
	}
	else
	{
		lOffset = -1;
	}
	return (lOffset);
}



int PackerVector::Pack(uint8* pDestination, const Vector3DF& pSource)
{
	// TODO: improve packer.
	typedef Vector3DF::BaseType _T;
	int lOffset = 0;
	*(_T*)&pDestination[lOffset] = Endian::HostToBig(pSource.x);	lOffset += sizeof(_T);
	*(_T*)&pDestination[lOffset] = Endian::HostToBig(pSource.y);	lOffset += sizeof(_T);
	*(_T*)&pDestination[lOffset] = Endian::HostToBig(pSource.z);	lOffset += sizeof(_T);
	return (lOffset);
}

int PackerVector::Unpack(Vector3DF& pDestination, const uint8* pSource, int pSize)
{
	typedef Vector3DF::BaseType _T;
	int lOffset = 0;
	if (pSize >= (int)sizeof(_T)*3)
	{
		pDestination.x = Endian::BigToHost(*(_T*)&pSource[lOffset]);	lOffset += sizeof(_T);
		pDestination.y = Endian::BigToHost(*(_T*)&pSource[lOffset]);	lOffset += sizeof(_T);
		pDestination.z = Endian::BigToHost(*(_T*)&pSource[lOffset]);	lOffset += sizeof(_T);
	}
	else
	{
		lOffset = -1;
	}
	return (lOffset);
}



int PackerInt32::Pack(uint8* pDestination, int32 pSource)
{
	// TODO: fix byte order.
	*(uint32*)pDestination = Endian::HostToBig(pSource);
	return (sizeof(pSource));
}

int PackerInt32::Unpack(int32& pDestination, const uint8* pSource, int pSize)
{
	// TODO: fix byte order.
	int lSize = -1;
	if (pSize >= (int)sizeof(pDestination))
	{
		lSize = sizeof(pDestination);
		pDestination = Endian::BigToHost(*(uint32*)pSource);
	}
	else
	{
		lSize = -1;
	}
	return (lSize);
}



int PackerInt16::Pack(uint8* pDestination, int pSource)
{
	*(int16*)pDestination = Endian::HostToBig((int16)pSource);
	return (sizeof(int16));
}

int PackerInt16::Unpack(int& pDestination, const uint8* pSource, int pSize)
{
	// TODO: fix byte order.
	int lSize = -1;
	if (pSize >= (int)sizeof(int16))
	{
		lSize = sizeof(int16);
		pDestination = Endian::BigToHost(*(int16*)pSource);
	}
	else
	{
		lSize = -1;
	}
	return (lSize);
}



int PackerReal::Pack(uint8* pDestination, float pSource)
{
	*(float*)pDestination = Endian::HostToBig(pSource);
	return (sizeof(pSource));
}

int PackerReal::Unpack(float& pDestination, const uint8* pSource, int pSize)
{
	int lSize;
	if (pSize >= (int)sizeof(pDestination))
	{
		lSize = sizeof(pDestination);
		pDestination = Endian::BigToHost(*(float*)pSource);
	}
	else
	{
		lSize = -1;
	}
	return (lSize);
}



int PackerOctetString::Pack(uint8* pDestination, const uint8* pSource, unsigned pLength)
{
	pDestination[0] = (uint8)pLength;
	pDestination[1] = (uint8)(pLength>>8);
	::memcpy(pDestination+2, pSource, pLength);
	return (2+pLength);
}

int PackerOctetString::Unpack(uint8* pDestination, const uint8* pSource, int pSize)
{
	int lSize = -1;
	if (pSize >= 3)
	{
		unsigned lLength = pSource[0]|(((unsigned)pSource[1])<<8);
		if (lLength >= 1 && (int)lLength+2 <= pSize)
		{
			lSize = 2+lLength;
			if (pDestination)
			{
				::memcpy(pDestination, pSource+2, lLength);
			}
		}
	}
	return (lSize);
}



int PackerUnicodeString::Pack(uint8* pDestination, const wstr& pSource)
{
	const astr lUtf8 = astrutil::ToOwnCode(pSource);
	const size_t lCharCount = lUtf8.length()+1;
	if (pDestination)
	{
		pDestination[0] = (uint8)lCharCount;
		pDestination[1] = (uint8)(lCharCount>>8);
		::memcpy(pDestination+2, lUtf8.c_str(), lCharCount-1);
		pDestination[2+lCharCount-1] = '\0';
	}
	return ((2+(int)lCharCount+3) & (~3));
}

int PackerUnicodeString::Unpack(wstr* pDestination, const uint8* pSource, int pSize)
{
	int lSize = -1;
	if (pSize >= 3)
	{
		const int lCharCount = pSource[0]|(((unsigned)pSource[1])<<8);
		if (lCharCount >= 1 && lCharCount+2 <= pSize && pSource[2+lCharCount-1] == '\0')
		{
			lSize = (2+lCharCount+3) & (~3);
			// TODO: catch UTF-8 encoding errors (might be DoS attempts).
			const wstr lConversion = wstrutil::ToOwnCode((const char*)pSource+2);
			if (pDestination)
			{
				*pDestination = lConversion;
			}
		}
	}
	return (lSize);
}



}