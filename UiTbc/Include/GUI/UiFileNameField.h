
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "UiTextField.h"



namespace UiTbc
{



class PopupList;



class FileNameField: public TextField
{
	typedef TextField Parent;
public:
	// Read more about the constructor paramters in TextField.h.
	FileNameField(Component* pTopParent, const str& pName);
	FileNameField(Component* pTopParent, 
		      unsigned pBorderStyle, int pBorderWidth, const Color& pColor,
		      const str& pName);
	FileNameField(Component* pTopParent, 
		      unsigned pBorderStyle, int pBorderWidth, Painter::ImageID pImageID,
		      const str& pName);
	FileNameField(Component* pTopParent, 
		      const Color& pColor, const str& pName);
	FileNameField(Component* pTopParent, 
		      Painter::ImageID pImageID, const str& pName);
	virtual ~FileNameField();

	// If no extensions are added, all files will be listed.
	// Extensions are given without the dot. "txt" for .txt files, 
	// for instance.
	void AddFileExtension(const str& pExtension);

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);

	virtual bool NotifyDoubleClick(PopupList* pList, int pMouseX, int pMouseY);

protected:
	virtual PopupList* CreatePopupList();

	void ValidatePath(str& pPath);

	// Searches the path backwards for slash (or backslash) number n.
	// n = 0 gives the last slash.
	size_t FindSlash(str& pPath, int n);

	void FinalizeSelection();

private:
	class FileInfo
	{
	public:
		FileInfo() :
			mDir(false)
		{
		}

		FileInfo(const str& pName, bool pDir) :
			mName(pName),
			mDir(pDir)
		{
		}

		// Used for sorting.
		bool operator < (const FileInfo& b) const { return (mName < b.mName); }

		str mName;
		bool mDir;
	};

	typedef std::list<str> StringList;
	StringList mFileExtensionList;
};



}
