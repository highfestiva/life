
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../Include/Path.h"



namespace Lepra
{



str Path::GetDirectory(const str& pPath)
{
	strutil::strvec lPathParts = SplitPath(pPath);
	return (lPathParts[0]);
}

str Path::GetFileBase(const str& pPath)
{
	strutil::strvec lPathParts = SplitPath(pPath);
	return (lPathParts[1]);
}

str Path::GetExtension(const str& pPath)
{
	strutil::strvec lPathParts = SplitPath(pPath);
	return (lPathParts[2]);
}

str Path::GetCompositeFilename(const str& pPath)
{
	str lDirectory;
	str lCompositeFilename;
	SplitPath(pPath, lDirectory, lCompositeFilename);
	return (lCompositeFilename);
}

strutil::strvec Path::SplitPath(const str& pPath)
{
	strutil::strvec lPathParts(3);

	// Search for extension.
	int x;
	for (x = (int)pPath.length()-1; x >= 0; --x)
	{
		if (pPath[x] == '.')
		{
			lPathParts[2] = pPath.substr(x+1);	// Ext.
			--x;
			break;
		}
		else if (IsPathSeparator(pPath[x]))
		{
			// No extension found; extension = "".
			x = (int)pPath.length()-1;
			break;
		}
	}

	// Search for directory and file.
	int lFileEnd = x;
	for (; x >= 0; --x)
	{
		if (IsPathSeparator(pPath[x]))
		{
			// No extension found. Extension = "".
			lPathParts[0] = pPath.substr(0, x+1);	// Dir.
			lPathParts[1] = pPath.substr(x+1, lFileEnd-x);	// File.
			break;
		}
	}

	// Translate "aaa/.bbb" as file=".bbb", ext="" (not file="", ext=".bbb").
	// Translate "aaa.bbb" as dir="", file="aaa", ext="bbb".
	{
		if (lPathParts[0].empty() && lPathParts[1].empty() && !lPathParts[2].empty())
		{
			lPathParts[1] = pPath.substr(0, lFileEnd+1);
		}
		else if (lPathParts[1].empty() && !lPathParts[2].empty())
		{
			lPathParts[1] = "."+lPathParts[2];
			lPathParts[2].clear();
		}
	}

	return (lPathParts);
}

void Path::SplitPath(const str& pPath, str& pDirectory, str& pFilename)
{
	strutil::strvec lPathParts = SplitPath(pPath);
	pDirectory = lPathParts[0];
	pFilename = JoinPath("", lPathParts[1], lPathParts[2]);
}

strutil::strvec Path::SplitNodes(const str& pDirectory, bool pExcludeLeadingDirectory, bool pExcludeTrailingDirectory)
{
#ifdef LEPRA_WINDOWS
	str lDirectory;
	// Include support for Win32 UNC names.
	if (pDirectory.substr(0, 4) == "\\\\.\\")
	{
		lDirectory = "\\\\.\\"+strutil::ReplaceAll(pDirectory.substr(4), '\\', '/');
	}
	else if (pDirectory.substr(0, 2) == "\\\\")
	{
		lDirectory = "\\\\"+strutil::ReplaceAll(pDirectory.substr(2), '\\', '/');
	}
	else
	{
		lDirectory = strutil::ReplaceAll(pDirectory, '\\', '/');
	}
#else // !LEPRA_WINDOWS
	str lDirectory = pDirectory;
#endif // LEPRA_WINDOWS / !LEPRA_WINDOWS
	if (pExcludeLeadingDirectory && !lDirectory.empty() && IsPathSeparator(lDirectory[0]))
	{
		lDirectory = lDirectory.substr(1);
	}
	if (pExcludeTrailingDirectory && !lDirectory.empty() && IsPathSeparator(lDirectory[lDirectory.length()-1]))
	{
		lDirectory.resize(lDirectory.length()-1);
	}
	return (strutil::Split(lDirectory, "/"));
}

str Path::JoinPath(const str& pDirectory, const str& pFileBase, const str& pExtension)
{
	str lFullFilename;
	if (!pDirectory.empty())
	{
		lFullFilename = pDirectory;
		if (!IsPathSeparator(lFullFilename[lFullFilename.length()-1]))
		{
			lFullFilename += '/';
		}
	}
	lFullFilename += pFileBase;
	if (!pExtension.empty())	// Only add extension if present (since "file." is different from "file" on some systems).
	{
		lFullFilename += "."+pExtension;
	}
	return (lFullFilename);
}

str Path::GetParentDirectory(const str& pDirectory)
{
	strutil::strvec lNodes = SplitNodes(pDirectory, false, true);
	if (lNodes.size() >= 1)
	{
		lNodes[lNodes.size()-1].clear();
	}
	return strutil::Join(lNodes, "/");
}

bool Path::NormalizePath(const str& pInputPath, str& pOutputPath)
{
	strutil::strvec lDirectoryArray = Path::SplitNodes(pInputPath, false, false);
	// Drop irrelevant path info such as "./" and "../" (not leading of course).
	strutil::strvec::iterator y = lDirectoryArray.begin();
	for (int x = 0; y != lDirectoryArray.end(); ++x)
	{
		if (x >= 1 && *y == ".")	// Only drops non-leading "./".
		{
			y = lDirectoryArray.erase(y);
			--x;
		}
		else if (x >= 1 && x < (int)lDirectoryArray.size()-1 && *y == "")	// Only drops non-leading+non-ending "/".
		{
			y = lDirectoryArray.erase(y);
			--x;
		}
		else if (x >= 1 && *y == "..")	// Only drops non-leading "../".
		{
			if (x == 1 && lDirectoryArray[0] == ".")
			{
				lDirectoryArray.erase(lDirectoryArray.begin());	// "./../" -> "../".
				y = lDirectoryArray.begin();
				x = -1;
			}
			else
			{
				strutil::strvec::iterator z = y-1;
				if (*z == "")	// "/../" is illegal.
				{
					return false;
				}
				if (*z != "..")	// "../../" should remain the same.
				{
					lDirectoryArray.erase(z, ++y);	// Drop this and parent directory ("a/../" -> "").
					x -= 2;
					y = lDirectoryArray.begin() + (x + 1);
				}
				else
				{
					++y;
				}
			}
		}
		else
		{
			++y;
		}
	}
	if (lDirectoryArray.size() == 1 && lDirectoryArray[0] == "")
	{
		pOutputPath = "/";
	}
	else
	{
		pOutputPath = strutil::Join(lDirectoryArray, "/");
	}
	return (true);
}

bool Path::IsPathSeparator(const char pCharacter)
{
	return (pCharacter == '/'
#ifdef LEPRA_WINDOWS
		|| pCharacter == '\\'
#endif // LEPRA_WINDOWS
		);
}

Path::Wildcard Path::CreateWildcard(const str& pWildcard)
{
	return strutil::Split(pWildcard, "*");
}

bool Path::IsWildcardMatch(const str& pWildcard, const str& pFilename, bool pScan)
{
	return IsWildcardMatch(CreateWildcard(pWildcard), pFilename, pScan);
}

bool Path::IsWildcardMatch(const Wildcard& pWildcard, const str& pFilename, bool pScan)
{
	bool lHasWildcard = false;
	Wildcard::const_iterator y = pWildcard.begin();
	for (; y != pWildcard.end(); ++y)
	{
		if (y->empty())
		{
			pScan = true;
			lHasWildcard = true;
			continue;
		}
		size_t x = 0;
		if (!pScan)
		{
			if (pFilename.compare(x, y->size(), *y) == 0)
			{
				size_t z = x + y->size();
				return IsWildcardMatch(Wildcard(++y, pWildcard.end()), pFilename.substr(z), true);
			}
			return false;
		}
		for (; x < pFilename.size(); ++x)
		{
			if (pFilename.compare(x, y->size(), *y) == 0)
			{
				size_t z = x + y->size();
				Wildcard::const_iterator u = y;
				if (IsWildcardMatch(Wildcard(++u, pWildcard.end()), pFilename.substr(z), true))
				{
					return true;
				}
			}
			if (pFilename[x] == '/')
			{
				return false;
			}
		}
		return false;
	}
	return pFilename.empty() || (lHasWildcard && pFilename.find('/') == str::npos);
}



}
