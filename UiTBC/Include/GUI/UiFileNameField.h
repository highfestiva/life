
// Author: Alexander Hugestrand
// Copyright (c) 2002-2008, Righteous Games



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
	FileNameField(Component* pTopParent, const Lepra::String& pName);
	FileNameField(Component* pTopParent, 
		      unsigned pBorderStyle, int pBorderWidth, const Lepra::Color& pColor,
		      const Lepra::String& pName);
	FileNameField(Component* pTopParent, 
		      unsigned pBorderStyle, int pBorderWidth, Painter::ImageID pImageID,
		      const Lepra::String& pName);
	FileNameField(Component* pTopParent, 
		      const Lepra::Color& pColor, const Lepra::String& pName);
	FileNameField(Component* pTopParent, 
		      Painter::ImageID pImageID, const Lepra::String& pName);
	virtual ~FileNameField();

	// If no extensions are added, all files will be listed.
	// Extensions are given without the dot. "txt" for .txt files, 
	// for instance.
	void AddFileExtension(const Lepra::String& pExtension);

	virtual bool OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode);

	virtual bool NotifyDoubleClick(PopupList* pList, int pMouseX, int pMouseY);

protected:
	virtual PopupList* CreatePopupList();

	void ValidatePath(Lepra::String& pPath);

	// Searches the path backwards for slash (or backslash) number n.
	// n = 0 gives the last slash.
	size_t FindSlash(Lepra::String& pPath, int n);

	void FinalizeSelection();

private:
	class FileInfo
	{
	public:
		FileInfo() :
			mDir(false)
		{
		}

		FileInfo(const Lepra::String& pName, bool pDir) :
			mName(pName),
			mDir(pDir)
		{
		}

		// Used for sorting.
		bool operator < (const FileInfo& b) const { return (mName < b.mName); }

		Lepra::String mName;
		bool mDir;
	};

	typedef std::list<Lepra::String> StringList;
	StringList mFileExtensionList;
};



}
