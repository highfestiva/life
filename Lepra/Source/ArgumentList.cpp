/*
	Class:  ArgumentList
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Righteous Games
*/

#include "../Include/ArgumentList.h"
#include "../Include/String.h"
#include <list>

#pragma warning(disable: 4996)	// Deprecated functions are not to be used, as they may be removed in future versions. Circumvent problem for example in strings by using LepraString instead.

namespace Lepra
{

ArgumentList::ArgumentList()
{
}

ArgumentList::ArgumentList(const tchar* pArguments)
{
	mArgumentVector = StringUtility::BlockSplit(pArguments, _T(" \t\v\r\n"), false);
}

ArgumentList::ArgumentList(const tchar* argv[], int argc)
{
	for (int x = 0; x < argc; ++x)
	{
		mArgumentVector.push_back(argv[x]);
	}
}

size_t ArgumentList::GetNumArgs() const
{
	return (mArgumentVector.size());
}

int ArgumentList::GetArgumentIndex(const String& pArgument, size_t pStartIndex) const
{
	for (size_t i = pStartIndex; i < mArgumentVector.size(); i++)
	{
		if (pArgument == mArgumentVector[i])
		{
			return ((int)i);
		}
	}

	return -1;
}

const String& ArgumentList::GetValue(const String& pArgument, size_t pStartIndex) const
{
	int i = GetArgumentIndex(pArgument, pStartIndex);
	if (i >= 0 && i < (int)mArgumentVector.size() - 1)
	{
		return (mArgumentVector[i + 1]);
	}
	else
	{
		return (EmptyString);
	}
}

const String& ArgumentList::GetArgument(size_t pArgumentIndex) const
{
	if (pArgumentIndex >= 0 && pArgumentIndex < mArgumentVector.size())
	{
		return (mArgumentVector[pArgumentIndex]);
	}
	else
	{
		return (EmptyString);
	}
}

bool ArgumentList::IsFlag(size_t pArgumentIndex)
{
	return (mArgumentVector[pArgumentIndex][0] == '-' || 
			mArgumentVector[pArgumentIndex][0] == '+');
}

/*StringUtility::StringVector ArgumentList::GetArgumentList(size_t pArgumentIndex) const
{
	StringUtility::StringVector lArgumentVector;
	if (pArgumentIndex >= 0 && pArgumentIndex < mArgumentVector.size())
	{
		lArgumentVector = do some type of "vector splice": mArgumentVector.splice(pArgumentIndex);
	}
	return (lArgumentVector);
}*/

/*void ArgumentList::GetFiles(const String& pArgument,
				const String& pFileExtension,
				bool pAcceptNoExtension,
				ArgumentList& pDest)
{
	int lStartIndex = GetArgumentIndex(pArgument);
	if (lStartIndex == -1)
	{
		return;
	}

	size_t lExtLength = pFileExtension.length();

	for (size_t i = lStartIndex + 1; i < mArgumentVector.size(); i++)
	{
		// Check if this argument is a flag.
		if (IsFlag(i) == true)
		{
			break;
		}
		else
		{
			size_t lLength = mArgumentVector[i].length();
			if (lLength > lExtLength &&
				StringUtility::CompareIgnoreCase(StringUtility::Right(mArgumentVector[i], lExtLength), pFileExtension) == 0)
			{
				pDest.AppendArg(mArgumentVector[i]);
			}
			else if(pAcceptNoExtension == true &&
				mArgumentVector[i].find('.', 0) == -1)
			{
				pDest.AppendArg(mArgumentVector[i]);
			}
		}
	}
}*/



}
