
// Author: Jonas Byström
// Copyright (c) 2002-2006, Righteous Games



#include "../Include/Path.h"



namespace Lepra
{



String Path::GetDirectory(const String& pPath)
{
	StringUtility::StringVector lPathParts = SplitPath(pPath);
	return (lPathParts[0]);
}

String Path::GetFileBase(const String& pPath)
{
	StringUtility::StringVector lPathParts = SplitPath(pPath);
	return (lPathParts[1]);
}

String Path::GetExtension(const String& pPath)
{
	StringUtility::StringVector lPathParts = SplitPath(pPath);
	return (lPathParts[2]);
}

String Path::GetCompositeFilename(const String& pPath)
{
	String lDirectory;
	String lCompositeFilename;
	SplitPath(pPath, lDirectory, lCompositeFilename);
	return (lCompositeFilename);
}

StringUtility::StringVector Path::SplitPath(const String& pPath)
{
	StringUtility::StringVector lPathParts(3);

	// Search for extension.
	int x;
	for (x = (int)pPath.length()-1; x >= 0; --x)
	{
		if (pPath[x] == _T('.'))
		{
			lPathParts[2] = pPath.substr(x+1);	// Ext.
			--x;
			break;
		}
		else if (IsPathSeparator(pPath[x]))
		{
			// No extension found; extension = "".
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
			lPathParts[1] = pPath.substr(0, lFileEnd);
		}
		else if (lPathParts[1].empty() && !lPathParts[2].empty())
		{
			lPathParts[1] = _T(".")+lPathParts[2];
			lPathParts[2].clear();
		}
	}

	return (lPathParts);
}

void Path::SplitPath(const String& pPath, String& pDirectory, String& pFilename)
{
	StringUtility::StringVector lPathParts = SplitPath(pPath);
	pDirectory = lPathParts[0];
	pFilename = JoinPath(_T(""), lPathParts[1], lPathParts[2]);
}

StringUtility::StringVector Path::SplitNodes(const String& pDirectory, bool pExcludeLeadingDirectory, bool pExcludeTrailingDirectory)
{
#ifdef LEPRA_WINDOWS
	String lDirectory;
	// Include support for Win32 UNC names.
	if (pDirectory.substr(0, 4) == _T("\\\\.\\"))
	{
		lDirectory = _T("\\\\.\\")+StringUtility::ReplaceAll(pDirectory.substr(4), _T('\\'), _T('/'));
	}
	else if (pDirectory.substr(0, 2) == _T("\\\\"))
	{
		lDirectory = _T("\\\\")+StringUtility::ReplaceAll(pDirectory.substr(2), _T('\\'), _T('/'));
	}
	else
	{
		lDirectory = StringUtility::ReplaceAll(pDirectory, _T('\\'), _T('/'));
	}
#else // !LEPRA_WINDOWS
	String lDirectory = pDirectory;
#endif // LEPRA_WINDOWS / !LEPRA_WINDOWS
	if (pExcludeLeadingDirectory && !lDirectory.empty() && IsPathSeparator(lDirectory[0]))
	{
		lDirectory = lDirectory.substr(1);
	}
	if (pExcludeTrailingDirectory && !lDirectory.empty() && IsPathSeparator(lDirectory[lDirectory.length()-1]))
	{
		lDirectory.resize(lDirectory.length()-1);
	}
	return (StringUtility::Split(lDirectory, _T("/")));
}

String Path::JoinPath(const String& pDirectory, const String& pFileBase, const String& pExtension)
{
	String lFullFilename;
	if (!pDirectory.empty())
	{
		lFullFilename = pDirectory;
		if (!IsPathSeparator(lFullFilename[lFullFilename.length()-1]))
		{
			lFullFilename += _T('/');
		}
	}
	lFullFilename += pFileBase;
	if (!pExtension.empty())	// Only add extension if present (since "file." is different from "file" on some systems).
	{
		lFullFilename += _T(".")+pExtension;
	}
	return (lFullFilename);
}

bool Path::NormalizePath(const String& pInputPath, String& pOutputPath)
{
	Lepra::StringUtility::StringVector lDirectoryArray = Lepra::Path::SplitNodes(pInputPath, false, false);
	// Drop irrelevant path info such as "./" and "../" (not leading of course).
	Lepra::StringUtility::StringVector::iterator y = lDirectoryArray.begin();
	for (int x = 0; y != lDirectoryArray.end(); ++x)
	{
		if (x >= 1 && *y == _T("."))	// Only drops non-leading "./".
		{
			y = lDirectoryArray.erase(y);
			--x;
		}
		else if (x >= 1 && x < (int)lDirectoryArray.size()-1 && *y == _T(""))	// Only drops non-leading+non-ending "/".
		{
			y = lDirectoryArray.erase(y);
			--x;
		}
		else if (x >= 1 && *y == _T(".."))	// Only drops non-leading "../".
		{
			if (x == 1 && lDirectoryArray[0] == _T("."))
			{
				lDirectoryArray.erase(lDirectoryArray.begin());	// "./../" -> "../".
				--x;
			}
			else
			{
				Lepra::StringUtility::StringVector::iterator z = y;
				--z;
				if (*z == _T(""))	// "/../" is illegal.
				{
					// TRICKY: returning here is much easier; path parsing is complex enough anyway.
					return (false);
				}
				if (*z != _T(".."))	// "../../" should remain the same.
				{
					y = lDirectoryArray.erase(z, ++y);	// Drop this and parent directory ("a/../" -> "").
					x -= 2;
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
	if (lDirectoryArray.size() == 1 && lDirectoryArray[0] == _T(""))
	{
		pOutputPath = _T("/");
	}
	else
	{
		pOutputPath = Lepra::StringUtility::Join(lDirectoryArray, _T("/"));
	}
	return (true);
}

bool Path::IsPathSeparator(const tchar pCharacter)
{
	return (pCharacter == _T('/')
#ifdef LEPRA_WINDOWS
		|| pCharacter == _T('\\')
#endif // LEPRA_WINDOWS
		);
}



}
