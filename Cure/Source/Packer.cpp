
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games

// TODO: check if IEEE-doubles are stored in network byte order by the FPU.
//       If not - add ntohll() on each long long (__int64).



#include <assert.h>
#include "../../Lepra/Include/Endian.h"
#include "../Include/Packer.h"

#pragma warning(disable: 4127)	// Conditional expression is constant - for sizeof(wchar_t).



namespace Cure
{



int PackerTransformation::Pack(Lepra::uint8* pDestination, const Lepra::TransformationF& pSource)
{
	// TODO: improve packer.
	typedef Lepra::TransformationF::BaseType _T;
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

int PackerTransformation::Unpack(Lepra::TransformationF& pDestination, const Lepra::uint8* pSource, int pSize)
{
	typedef Lepra::TransformationF::BaseType _T;
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



int PackerVector::Pack(Lepra::uint8* pDestination, const Lepra::Vector3DF& pSource)
{
	// TODO: improve packer.
	typedef Lepra::Vector3DF::BaseType _T;
	int lOffset = 0;
	*(_T*)&pDestination[lOffset] = Lepra::Endian::HostToBig(pSource.x);	lOffset += sizeof(_T);
	*(_T*)&pDestination[lOffset] = Lepra::Endian::HostToBig(pSource.y);	lOffset += sizeof(_T);
	*(_T*)&pDestination[lOffset] = Lepra::Endian::HostToBig(pSource.z);	lOffset += sizeof(_T);
	return (lOffset);
}

int PackerVector::Unpack(Lepra::Vector3DF& pDestination, const Lepra::uint8* pSource, int pSize)
{
	typedef Lepra::Vector3DF::BaseType _T;
	int lOffset = 0;
	if (pSize >= (int)sizeof(_T)*3)
	{
		pDestination.x = Lepra::Endian::BigToHost(*(_T*)&pSource[lOffset]);	lOffset += sizeof(_T);
		pDestination.y = Lepra::Endian::BigToHost(*(_T*)&pSource[lOffset]);	lOffset += sizeof(_T);
		pDestination.z = Lepra::Endian::BigToHost(*(_T*)&pSource[lOffset]);	lOffset += sizeof(_T);
	}
	else
	{
		lOffset = -1;
	}
	return (lOffset);
}



int PackerInt32::Pack(Lepra::uint8* pDestination, Lepra::int32 pSource)
{
	// TODO: fix byte order.
	*(Lepra::uint32*)pDestination = Lepra::Endian::HostToBig(pSource);
	return (sizeof(pSource));
}

int PackerInt32::Unpack(Lepra::int32& pDestination, const Lepra::uint8* pSource, int pSize)
{
	// TODO: fix byte order.
	int lSize = -1;
	if (pSize >= (int)sizeof(pDestination))
	{
		lSize = sizeof(pDestination);
		pDestination = Lepra::Endian::BigToHost(*(Lepra::uint32*)pSource);
	}
	else
	{
		lSize = -1;
	}
	return (lSize);
}



int PackerInt16::Pack(Lepra::uint8* pDestination, int pSource)
{
	*(Lepra::int16*)pDestination = Lepra::Endian::HostToBig((Lepra::int16)pSource);
	return (sizeof(Lepra::int16));
}

int PackerInt16::Unpack(int& pDestination, const Lepra::uint8* pSource, int pSize)
{
	// TODO: fix byte order.
	int lSize = -1;
	if (pSize >= (int)sizeof(Lepra::int16))
	{
		lSize = sizeof(Lepra::int16);
		pDestination = Lepra::Endian::BigToHost(*(Lepra::int16*)pSource);
	}
	else
	{
		lSize = -1;
	}
	return (lSize);
}



int PackerReal::Pack(Lepra::uint8* pDestination, float pSource)
{
	*(float*)pDestination = Lepra::Endian::HostToBig(pSource);
	return (sizeof(pSource));
}

int PackerReal::Unpack(float& pDestination, const Lepra::uint8* pSource, int pSize)
{
	int lSize;
	if (pSize >= (int)sizeof(pDestination))
	{
		lSize = sizeof(pDestination);
		pDestination = Lepra::Endian::BigToHost(*(float*)pSource);
	}
	else
	{
		lSize = -1;
	}
	return (lSize);
}



int PackerOctetString::Pack(Lepra::uint8* pDestination, const Lepra::uint8* pSource, unsigned pLength)
{
	pDestination[0] = (Lepra::uint8)pLength;
	pDestination[1] = (Lepra::uint8)(pLength>>8);
	::memcpy(pDestination+2, pSource, pLength);
	return (2+pLength);
}

int PackerOctetString::Unpack(Lepra::uint8* pDestination, const Lepra::uint8* pSource, int pSize)
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



int PackerUnicodeString::Pack(Lepra::uint8* pDestination, const Lepra::UnicodeString& pSource)
{
	// TODO: improve Unicode packer. Could be optimized, and implemented to support
	// horrid UTF-16 characters (which are 24 and 32 bits in length).
	size_t lCharCount = pSource.length()+1;
	pDestination[0] = (Lepra::uint8)lCharCount;
	pDestination[1] = (Lepra::uint8)(lCharCount>>8);
	if (sizeof(wchar_t) == 2)	// UTF-16 (Windows)
	{
		::memcpy(pDestination+2, pSource.c_str(), (lCharCount-1)*2);
	}
	else if (sizeof(wchar_t) == 4)	// UTF-32 (Posix)
	{
		for (size_t x = 0; x < lCharCount-1; ++x)
		{
			*(Lepra::uint16*)pDestination[2+x*2] = (Lepra::uint16)pSource[x];
		}
	}
	else
	{
		assert(false);
	}
	*(Lepra::uint16*)&pDestination[2+(lCharCount-1)*2] = 0;
	return (2+(int)lCharCount*2);
}

int PackerUnicodeString::Unpack(Lepra::UnicodeString* pDestination, const Lepra::uint8* pSource, int pSize)
{
	int lSize = -1;
	if (pSize >= 3)
	{
		const unsigned pCharCount = pSource[0]|(((unsigned)pSource[1])<<8);
		if (pCharCount >= 1 && (int)pCharCount*2+2 <= pSize)
		{
			const Lepra::uint16* lSource = (const Lepra::uint16*)&pSource[2];
			// Check that the string is non-null everywhere but on the terminating zero.
			for (unsigned x = 0; x < pCharCount-1; ++x)
			{
				if (lSource[x] == _T('\0'))
				{
					return (-1);	// TRICKY: optimized and simplified code.
				}
			}
			if (lSource[pCharCount-1] == _T('\0'))
			{
				lSize = 2+pCharCount*2;
				if (pDestination)
				{
					if (sizeof(wchar_t) == 2)	// UTF-16 (Windows)
					{
						pDestination->assign((wchar_t*)lSource, pCharCount-1);
					}
					else if (sizeof(wchar_t) == 4)	// UTF-32 (Posix)
					{
						wchar_t lBuffer[1024];	// TODO: fix buffer overrun exploit.
						for (size_t x = 0; x < pCharCount-1; ++x)
						{
							lBuffer[x] = pSource[x];
						}
						pDestination->assign(lBuffer, pCharCount-1);
					}
					else
					{
						assert(false);
					}
				}
			}
		}
	}
	return (lSize);
}



}
