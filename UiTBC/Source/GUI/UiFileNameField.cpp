/*
	Class:  FileNameField
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../../Lepra/Include/DiskFile.h"
#include "../../../Lepra/Include/Path.h"
#include "../../Include/GUI/UiFileNameField.h"
#include "../../Include/GUI/UiPopupList.h"
#include "../../Include/GUI/UiDesktopWindow.h"
#include <algorithm>

namespace UiTbc
{

FileNameField::FileNameField(Component* pTopParent, const Lepra::String& pName) :
	TextField(pTopParent, pName)
{
}

FileNameField::FileNameField(Component* pTopParent, 
			     unsigned pBorderStyle, int pBorderWidth, const Lepra::Color& pColor,
			     const Lepra::String& pName) :
	TextField(pTopParent, pBorderStyle, pBorderWidth, pColor, pName)
{
}

FileNameField::FileNameField(Component* pTopParent, 
			     unsigned pBorderStyle, int pBorderWidth, Painter::ImageID pImageID,
			     const Lepra::String& pName) :
	TextField(pTopParent, pBorderStyle, pBorderWidth, pImageID, pName)
{
}

FileNameField::FileNameField(Component* pTopParent, 
			     const Lepra::Color& pColor, const Lepra::String& pName) :
	TextField(pTopParent, pColor, pName)
{
}

FileNameField::FileNameField(Component* pTopParent, 
			     Painter::ImageID pImageID, const Lepra::String& pName) :
	TextField(pTopParent, pImageID, pName)
{
}

FileNameField::~FileNameField()
{
}

PopupList* FileNameField::CreatePopupList()
{
	Lepra::String lSearchString(GetText());
	lSearchString += _T("*");

	Painter* lPainter = 0;
	DesktopWindow* lDesktopWindow = (DesktopWindow*)GetParentOfType(DESKTOPWINDOW);
	if (lDesktopWindow != 0)
	{
		lPainter = lDesktopWindow->GetInternalPainter();
	}

	std::list<FileInfo> lFileList;
	Lepra::DiskFile::FindData lFindData;
	bool lOk = Lepra::DiskFile::FindFirst(lSearchString, lFindData);
	while (lOk)
	{
		bool lFileExtensionOk = true;

		if (mFileExtensionList.empty() == false && lFindData.IsSubDir() == false)
		{
			Lepra::String lExtension = Lepra::Path::GetExtension(lFindData.GetName());
			Lepra::StringUtility::ToLower(lExtension);
			lFileExtensionOk = std::find(mFileExtensionList.begin(), mFileExtensionList.end(), lExtension) != mFileExtensionList.end();
		}
		
		if (lFileExtensionOk == true)
		{
			lFileList.push_back(FileInfo(lFindData.GetName(), lFindData.IsSubDir()));
		}
		lOk = Lepra::DiskFile::FindNext(lFindData);
	}

	PopupList* lPopupList = 0;

	if (!lFileList.empty())
	{
		lPopupList = new PopupList(BORDER_SUNKEN, 3, Lepra::WHITE);
		lPopupList->SetStyle(ListControl::SINGLE_SELECT);

		lFileList.sort();

		std::list<FileInfo>::iterator lIter;
		int lLabelHeight = 0;
		for (lIter = lFileList.begin(); lIter != lFileList.end(); ++lIter)
		{
			Label* lLabel = new Label(Lepra::WHITE, Lepra::LIGHT_BLUE);
			lLabel->SetText((*lIter).mName, 
					 GetFontId(), 
					 GetTextColor(),
					 Lepra::WHITE,
					 Lepra::BLACK,
					 Lepra::LIGHT_BLUE,
					 GetTextBlendFunc(),
					 GetTextAlpha(),
					 lPainter);
			lLabel->SetPreferredSize(0, 12);
			lPopupList->AddChild(lLabel);

			if (lIter == lFileList.begin())
			{
				lLabelHeight = lLabel->GetPreferredHeight(true);
			}
		}

		lPopupList->SetPreferredHeight(lLabelHeight * min(lPopupList->GetNumChildren(), 10));
	}

	return lPopupList;
}

bool FileNameField::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	Parent::OnKeyDown(pKeyCode);
	switch (pKeyCode)
	{
		case UiLepra::InputManager::KEY_ENTER:
		{
			FinalizeSelection();
		}
		break;
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

void FileNameField::ValidatePath(Lepra::String& pPath)
{
	Lepra::String lNormalizedPath;
	if (!Lepra::Path::NormalizePath(pPath, lNormalizedPath))
	{
		// TRICKY: nothing to do - this is not a path!
		return;
	}

	Lepra::DiskFile::FindData lFindData;
	bool lOk = Lepra::DiskFile::FindFirst(lNormalizedPath, lFindData);
	if (lOk == true)
	{
		if (lFindData.IsSubDir() && Lepra::StringUtility::Right(lNormalizedPath, 1) != _T("/"))
		{
			pPath += _T('/');
		}
	}
	else
	{
		// TODO: verify code! How can the path exist if FindFirst did not find the path? Contradicting, to say the least.
		lOk = Lepra::DiskFile::PathExists(lNormalizedPath);
		if (lOk && Lepra::StringUtility::Right(lNormalizedPath, 2) == _T(".."))
		{
			lNormalizedPath += _T('/');
		}
	}
	pPath = lNormalizedPath;
}

size_t FileNameField::FindSlash(Lepra::String& pPath, int n)
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
		Lepra::String lText(GetText());

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
		lText += lSelectedItem->GetText().c_str();

		ValidatePath(lText);
		SetText(lText);

		DestroyPopupList();
		SetMarkerPos(lText.length());
		SetKeyboardFocus();
	}
}

void FileNameField::AddFileExtension(const Lepra::String& pExtension)
{
	Lepra::String lExtension(pExtension);
	Lepra::StringUtility::ToLower(lExtension);
	mFileExtensionList.push_back(lExtension);
}

} // End namespace.
