/*
	Class:  FileNameField
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../../Lepra/Include/DiskFile.h"
#include "../../../Lepra/Include/Path.h"
#include "../../Include/GUI/UiFileNameField.h"
#include "../../Include/GUI/UiPopupList.h"
#include "../../Include/GUI/UiDesktopWindow.h"
#include <algorithm>

namespace UiTbc
{

FileNameField::FileNameField(Component* pTopParent):
	TextField(pTopParent)
{
}

FileNameField::FileNameField(Component* pTopParent, unsigned pBorderStyle, int pBorderWidth, const Color& pColor):
	TextField(pTopParent, pBorderStyle, pBorderWidth, pColor)
{
}

FileNameField::FileNameField(Component* pTopParent, unsigned pBorderStyle, int pBorderWidth, Painter::ImageID pImageID):
	TextField(pTopParent, pBorderStyle, pBorderWidth, pImageID)
{
}

FileNameField::FileNameField(Component* pTopParent, const Color& pColor):
	TextField(pTopParent, pColor)
{
}

FileNameField::FileNameField(Component* pTopParent, Painter::ImageID pImageID):
	TextField(pTopParent, pImageID)
{
}

FileNameField::~FileNameField()
{
}

PopupList* FileNameField::CreatePopupList()
{
	str lSearchString = strutil::Encode(GetText());
	lSearchString += "*";

	Painter* lPainter = 0;
	DesktopWindow* lDesktopWindow = (DesktopWindow*)GetParentOfType(DESKTOPWINDOW);
	if (lDesktopWindow != 0)
	{
		lPainter = lDesktopWindow->GetPainter();
	}

	std::list<FileInfo> lFileList;
	DiskFile::FindData lFindData;
	bool lOk = DiskFile::FindFirst(lSearchString, lFindData);
	while (lOk)
	{
		bool lFileExtensionOk = true;

		if (mFileExtensionList.empty() == false && lFindData.IsSubDir() == false)
		{
			str lExtension = Path::GetExtension(lFindData.GetName());
			strutil::ToLower(lExtension);
			lFileExtensionOk = std::find(mFileExtensionList.begin(), mFileExtensionList.end(), lExtension) != mFileExtensionList.end();
		}
		
		if (lFileExtensionOk == true)
		{
			lFileList.push_back(FileInfo(lFindData.GetName(), lFindData.IsSubDir()));
		}
		lOk = DiskFile::FindNext(lFindData);
	}

	PopupList* lPopupList = 0;

	if (!lFileList.empty())
	{
		lPopupList = new PopupList(BORDER_SUNKEN, 3, WHITE);
		lPopupList->SetStyle(ListControl::SINGLE_SELECT);

		lFileList.sort();

		std::list<FileInfo>::iterator lIter;
		int lLabelHeight = 0;
		for (lIter = lFileList.begin(); lIter != lFileList.end(); ++lIter)
		{
			Label* lLabel = new Label(GetTextColor(), wstrutil::Encode((*lIter).mName));
			lLabel->SetPreferredSize(0, 12);
			lPopupList->AddChild(lLabel);

			if (lIter == lFileList.begin())
			{
				lLabelHeight = lLabel->GetPreferredHeight(true);
			}
		}

		lPopupList->SetPreferredHeight(lLabelHeight * (int)std::min(lPopupList->GetNumChildren(), 10));
	}

	return lPopupList;
}

bool FileNameField::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	Parent::OnKeyDown(pKeyCode);
	switch (pKeyCode)
	{
		case UiLepra::InputManager::IN_KBD_ENTER:
		{
			FinalizeSelection();
		}
		break;
		default: break;
	}
	return (false);
}

bool FileNameField::NotifyDoubleClick(PopupList*, int pMouseX, int pMouseY)
{
	bool lReturnValue = true;

	Component* lItem = GetPopupList()->GetFirstSelectedItem();

	if (lItem != 0 && lItem->IsOver(pMouseX, pMouseY))
	{
		FinalizeSelection();
		lReturnValue = false;
	}

	return lReturnValue;
}

void FileNameField::ValidatePath(str& pPath)
{
	str lNormalizedPath;
	if (!Path::NormalizePath(pPath, lNormalizedPath))
	{
		// TRICKY: nothing to do - this is not a path!
		return;
	}

	DiskFile::FindData lFindData;
	bool lOk = DiskFile::FindFirst(lNormalizedPath, lFindData);
	if (lOk == true)
	{
		if (lFindData.IsSubDir() && strutil::Right(lNormalizedPath, 1) != "/")
		{
			pPath += '/';
		}
	}
	else
	{
		// TODO: verify code! How can the path exist if FindFirst did not find the path? Contradicting, to say the least.
		lOk = DiskFile::PathExists(lNormalizedPath);
		if (lOk && strutil::Right(lNormalizedPath, 2) == "..")
		{
			lNormalizedPath += '/';
		}
	}
	pPath = lNormalizedPath;
}

size_t FileNameField::FindSlash(str& pPath, int n)
{
	size_t lIndex = pPath.length();
	if (lIndex > 0)
	{
		int i = 0;
		do
		{
			int lIndex1 = (int)pPath.rfind('/', lIndex-1);
			int lIndex2 = (int)pPath.rfind('\\', lIndex-1);
			lIndex = lIndex1 > lIndex2 ? lIndex1 : lIndex2;

			i++;
		} while(i < n);
	}
	return lIndex;
}

void FileNameField::FinalizeSelection()
{
	if (GetPopupList() != 0)
	{
		str lText = strutil::Encode(GetText());

		if (!lText.empty())
		{
			// We have to remove everything from the last '/' or '\\'.
			// Why? 
			// 1. If the user wrote "C:\Program" and then selected
			//    "Program Files", "Program Files" should replace "Program".
			// 2. If the user wrote "C:\Program Files\", nothing is removed.
			size_t lIndex = FindSlash(lText, 0);
			lText.erase(lIndex + 1, lText.length() - (lIndex + 1));
		}		

		ValidatePath(lText);
		Label* lSelectedItem = (Label*)GetPopupList()->GetFirstSelectedItem();
		lText += strutil::Encode(lSelectedItem->GetText());

		ValidatePath(lText);
		const wstr lWideText = wstrutil::Encode(lText);
		SetText(lWideText);

		DestroyPopupList();
		SetMarkerPos(lWideText.length());
		SetKeyboardFocus();
	}
}

void FileNameField::AddFileExtension(const str& pExtension)
{
	str lExtension(pExtension);
	strutil::ToLower(lExtension);
	mFileExtensionList.push_back(lExtension);
}

} // End namespace.
