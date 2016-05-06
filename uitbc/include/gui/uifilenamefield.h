
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "uitextfield.h"



namespace uitbc {



class PopupList;



class FileNameField: public TextField {
	typedef TextField Parent;
public:
	// Read more about the constructor paramters in TextField.h.
	FileNameField(Component* top_parent);
	FileNameField(Component* top_parent, unsigned border_style, int border_width, const Color& color);
	FileNameField(Component* top_parent, unsigned border_style, int border_width, Painter::ImageID image_id);
	FileNameField(Component* top_parent, const Color& color);
	FileNameField(Component* top_parent, Painter::ImageID image_id);
	virtual ~FileNameField();

	// If no extensions are added, all files will be listed.
	// Extensions are given without the dot. "txt" for .txt files,
	// for instance.
	void AddFileExtension(const str& extension);

	virtual bool OnKeyDown(uilepra::InputManager::KeyCode key_code);

	virtual bool NotifyDoubleClick(PopupList* _list, int mouse_x, int mouse_y);

protected:
	virtual PopupList* CreatePopupList();

	void ValidatePath(str& path);

	// Searches the path backwards for slash (or backslash) number n.
	// n = 0 gives the last slash.
	size_t FindSlash(str& path, int n);

	void FinalizeSelection();

private:
	class FileInfo {
	public:
		FileInfo() :
			dir_(false) {
		}

		FileInfo(const str& name, bool dir) :
			name_(name),
			dir_(dir) {
		}

		// Used for sorting.
		bool operator < (const FileInfo& b) const { return (name_ < b.name_); }

		str name_;
		bool dir_;
	};

	typedef std::list<str> StringList;
	StringList file_extension_list_;
};



}
