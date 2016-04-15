
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "String.h"



namespace Lepra
{



class Path
{
public:
	typedef strutil::strvec Wildcard;

	static str GetDirectory(const str& pPath);
	static str GetFileBase(const str& pPath);
	static str GetExtension(const str& pPath);
	// Returns "filebase.ext" or "filebase" (if ext="").
	static str GetCompositeFilename(const str& pPath);
	// Splits path into three units: directory, file and extension.
	static strutil::strvec SplitPath(const str& pPath);
	static void SplitPath(const str& pPath, str& pDirectory, str& pFilename);
	// Splits directory path into subdirectories: "C:\\a\\b" -> ["C:","a","b"].
	static strutil::strvec SplitNodes(const str& pDirectory, bool pExcludeLeadingDirectory = true, bool pExcludeTrailingDirectory = true);
	// Joins a directory, filename and extension to full/filename.ext (if ext != "").
	static str JoinPath(const str& pDirectory, const str& pFileBase, const str& pExtension = str());
	static str GetParentDirectory(const str& pDirectory);
	static bool NormalizePath(const str& pInputPath, str& pOutputPath);
	static bool IsPathSeparator(const char pCharacter);
	static Wildcard CreateWildcard(const str& pWildcard);
	static bool IsWildcardMatch(const str& pWildcard, const str& pFilename, bool pScan = false);
	static bool IsWildcardMatch(const Wildcard& pWildcard, const str& pFilename, bool pScan = false);
};


}
