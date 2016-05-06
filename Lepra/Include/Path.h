
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once



#include "string.h"



namespace lepra {



class Path {
public:
	typedef strutil::strvec Wildcard;

	static str GetDirectory(const str& path);
	static str GetFileBase(const str& path);
	static str GetExtension(const str& path);
	// Returns "filebase.ext" or "filebase" (if ext="").
	static str GetCompositeFilename(const str& path);
	// Splits path into three units: directory, file and extension.
	static strutil::strvec SplitPath(const str& path);
	static void SplitPath(const str& path, str& directory, str& filename);
	// Splits directory path into subdirectories: "C:\\a\\b" -> ["C:","a","b"].
	static strutil::strvec SplitNodes(const str& directory, bool exclude_leading_directory = true, bool exclude_trailing_directory = true);
	// Joins a directory, filename and extension to full/filename.ext (if ext != "").
	static str JoinPath(const str& directory, const str& file_base, const str& extension = str());
	static str GetParentDirectory(const str& directory);
	static bool NormalizePath(const str& input_path, str& output_path);
	static bool IsPathSeparator(const char character);
	static Wildcard CreateWildcard(const str& wildcard);
	static bool IsWildcardMatch(const str& wildcard, const str& filename, bool scan = false);
	static bool IsWildcardMatch(const Wildcard& wildcard, const str& filename, bool scan = false);
};


}
