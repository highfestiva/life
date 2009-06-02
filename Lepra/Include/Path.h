
// Author: Jonas Byström
// Copyright (c) 2002-2008, Righteous Games



#pragma once



#include "String.h"



namespace Lepra
{



class Path
{
public:
	static String GetDirectory(const String& pPath);
	static String GetFileBase(const String& pPath);
	static String GetExtension(const String& pPath);
	// Returns "filebase.ext" or "filebase" (if ext="").
	static String GetCompositeFilename(const String& pPath);
	// Splits path into three units: directory, file and extension.
	static StringUtility::StringVector SplitPath(const String& pPath);
	static void SplitPath(const String& pPath, String& pDirectory, String& pFilename);
	// Splits directory path into subdirectories: "C:\\a\\b" -> ["C:","a","b"].
	static StringUtility::StringVector SplitNodes(const String& pDirectory, bool pExcludeLeadingDirectory = true, bool pExcludeTrailingDirectory = true);
	// Joins a directory, filename and extension to full/filename.ext (if ext != "").
	static String JoinPath(const String& pDirectory, const String& pFileBase, const String& pExtension);
	static bool NormalizePath(const String& pInputPath, String& pOutputPath);
	static bool IsPathSeparator(const tchar pCharacter);
};


}
