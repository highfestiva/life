
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../include/path.h"



namespace lepra {



str Path::GetDirectory(const str& path) {
	strutil::strvec path_parts = SplitPath(path);
	return (path_parts[0]);
}

str Path::GetFileBase(const str& path) {
	strutil::strvec path_parts = SplitPath(path);
	return (path_parts[1]);
}

str Path::GetExtension(const str& path) {
	strutil::strvec path_parts = SplitPath(path);
	return (path_parts[2]);
}

str Path::GetCompositeFilename(const str& path) {
	str _directory;
	str composite_filename;
	SplitPath(path, _directory, composite_filename);
	return (composite_filename);
}

strutil::strvec Path::SplitPath(const str& path) {
	strutil::strvec path_parts(3);

	// Search for extension.
	int x;
	for (x = (int)path.length()-1; x >= 0; --x) {
		if (path[x] == '.') {
			path_parts[2] = path.substr(x+1);	// Ext.
			--x;
			break;
		} else if (IsPathSeparator(path[x])) {
			// No extension found; extension = "".
			x = (int)path.length()-1;
			break;
		}
	}

	// Search for directory and file.
	int file_end = x;
	for (; x >= 0; --x) {
		if (IsPathSeparator(path[x])) {
			// No extension found. Extension = "".
			path_parts[0] = path.substr(0, x+1);	// Dir.
			path_parts[1] = path.substr(x+1, file_end-x);	// File.
			break;
		}
	}

	// Translate "aaa/.bbb" as file=".bbb", ext="" (not file="", ext=".bbb").
	// Translate "aaa.bbb" as dir="", file="aaa", ext="bbb".
	{
		if (path_parts[0].empty() && path_parts[1].empty() && !path_parts[2].empty()) {
			path_parts[1] = path.substr(0, file_end+1);
		} else if (path_parts[1].empty() && !path_parts[2].empty()) {
			path_parts[1] = "."+path_parts[2];
			path_parts[2].clear();
		}
	}

	return (path_parts);
}

void Path::SplitPath(const str& path, str& directory, str& filename) {
	strutil::strvec path_parts = SplitPath(path);
	directory = path_parts[0];
	filename = JoinPath("", path_parts[1], path_parts[2]);
}

strutil::strvec Path::SplitNodes(const str& directory, bool exclude_leading_directory, bool exclude_trailing_directory) {
#ifdef LEPRA_WINDOWS
	str _directory;
	// Include support for Win32 UNC names.
	if (directory.substr(0, 4) == "\\\\.\\") {
		_directory = "\\\\.\\"+strutil::ReplaceAll(directory.substr(4), '\\', '/');
	} else if (directory.substr(0, 2) == "\\\\") {
		_directory = "\\\\"+strutil::ReplaceAll(directory.substr(2), '\\', '/');
	} else {
		_directory = strutil::ReplaceAll(directory, '\\', '/');
	}
#else // !LEPRA_WINDOWS
	str _directory = directory;
#endif // LEPRA_WINDOWS / !LEPRA_WINDOWS
	if (exclude_leading_directory && !_directory.empty() && IsPathSeparator(_directory[0])) {
		_directory = _directory.substr(1);
	}
	if (exclude_trailing_directory && !_directory.empty() && IsPathSeparator(_directory[_directory.length()-1])) {
		_directory.resize(_directory.length()-1);
	}
	return (strutil::Split(_directory, "/"));
}

str Path::JoinPath(const str& directory, const str& file_base, const str& extension) {
	str full_filename;
	if (!directory.empty()) {
		full_filename = directory;
		if (!IsPathSeparator(full_filename[full_filename.length()-1])) {
			full_filename += '/';
		}
	}
	full_filename += file_base;
	if (!extension.empty()) {	// Only add extension if present (since "file." is different from "file" on some systems).
		full_filename += "."+extension;
	}
	return (full_filename);
}

str Path::GetParentDirectory(const str& directory) {
	strutil::strvec nodes = SplitNodes(directory, false, true);
	if (nodes.size() >= 1) {
		nodes[nodes.size()-1].clear();
	}
	return strutil::Join(nodes, "/");
}

bool Path::NormalizePath(const str& input_path, str& output_path) {
	strutil::strvec directory_array = Path::SplitNodes(input_path, false, false);
	// Drop irrelevant path info such as "./" and "../" (not leading of course).
	strutil::strvec::iterator y = directory_array.begin();
	for (int x = 0; y != directory_array.end(); ++x) {
		if (x >= 1 && *y == ".") {	// Only drops non-leading "./".
			y = directory_array.erase(y);
			--x;
		} else if (x >= 1 && x < (int)directory_array.size()-1 && *y == "") {	// Only drops non-leading+non-ending "/".
			y = directory_array.erase(y);
			--x;
		} else if (x >= 1 && *y == "..") {	// Only drops non-leading "../".
			if (x == 1 && directory_array[0] == ".") {
				directory_array.erase(directory_array.begin());	// "./../" -> "../".
				y = directory_array.begin();
				x = -1;
			} else {
				strutil::strvec::iterator z = y-1;
				if (*z == "") {	// "/../" is illegal.
					return false;
				}
				if (*z != "..") {	// "../../" should remain the same.
					directory_array.erase(z, ++y);	// Drop this and parent directory ("a/../" -> "").
					x -= 2;
					y = directory_array.begin() + (x + 1);
				} else {
					++y;
				}
			}
		} else {
			++y;
		}
	}
	if (directory_array.size() == 1 && directory_array[0] == "") {
		output_path = "/";
	} else {
		output_path = strutil::Join(directory_array, "/");
	}
	return (true);
}

bool Path::IsPathSeparator(const char character) {
	return (character == '/'
#ifdef LEPRA_WINDOWS
		|| character == '\\'
#endif // LEPRA_WINDOWS
		);
}

Path::Wildcard Path::CreateWildcard(const str& wildcard) {
	return strutil::Split(wildcard, "*");
}

bool Path::IsWildcardMatch(const str& wildcard, const str& filename, bool scan) {
	return IsWildcardMatch(CreateWildcard(wildcard), filename, scan);
}

bool Path::IsWildcardMatch(const Wildcard& wildcard, const str& filename, bool scan) {
	bool has_wildcard = false;
	Wildcard::const_iterator y = wildcard.begin();
	for (; y != wildcard.end(); ++y) {
		if (y->empty()) {
			scan = true;
			has_wildcard = true;
			continue;
		}
		size_t x = 0;
		if (!scan) {
			if (filename.compare(x, y->size(), *y) == 0) {
				size_t z = x + y->size();
				return IsWildcardMatch(Wildcard(++y, wildcard.end()), filename.substr(z), true);
			}
			return false;
		}
		for (; x < filename.size(); ++x) {
			if (filename.compare(x, y->size(), *y) == 0) {
				size_t z = x + y->size();
				Wildcard::const_iterator u = y;
				if (IsWildcardMatch(Wildcard(++u, wildcard.end()), filename.substr(z), true)) {
					return true;
				}
			}
			if (filename[x] == '/') {
				return false;
			}
		}
		return false;
	}
	return filename.empty() || (has_wildcard && filename.find('/') == str::npos);
}



}
