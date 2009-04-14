
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once



#include "LepraTypes.h"
#include "String.h"



namespace Lepra
{



class ArgumentList
{
public:
	ArgumentList();
	ArgumentList(const tchar* pArgs);	// Arguments are split on whitespaces.
	ArgumentList(const tchar* argv[], int argc);

	size_t GetNumArgs() const;

	// Returns the argument index if the parameter was found.
	// -1 if not found.
	int GetArgumentIndex(const String& pArgument, size_t pStartIndex = 0) const;

	// Returns the argument following pParam, if it exists.
	const String& GetValue(const String& pArgument, size_t pStartIndex = 0) const;

	const String& GetArgument(size_t pArgumentIndex) const;
	bool IsFlag(size_t pArgumentIndex);

	// Returns the argument list starting at pArgumentIndex.
	//StringUtility::StringVector GetArgumentList(size_t pArgumentIndex) const;

	// From the first occurence of pArgument, this function appends all
	// file names with the extension pFileExtension to pDest.
	//void GetFiles(const String& pArgument, const String& pFileExtension, StringUtility::StringVector& pDest);

private:
	StringUtility::StringVector mArgumentVector;
};



}
