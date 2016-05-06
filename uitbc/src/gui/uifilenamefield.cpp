/*
	Class:  FileNameField
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "pch.h"
#include "../../../lepra/include/diskfile.h"
#include "../../../lepra/include/path.h"
#include "../../include/gui/uifilenamefield.h"
#include "../../include/gui/uipopuplist.h"
#include "../../include/gui/uidesktopwindow.h"
#include <algorithm>

namespace uitbc {

FileNameField::FileNameField(Component* top_parent):
	TextField(top_parent) {
}

FileNameField::FileNameField(Component* top_parent, unsigned border_style, int border_width, const Color& color):
	TextField(top_parent, border_style, border_width, color) {
}

FileNameField::FileNameField(Component* top_parent, unsigned border_style, int border_width, Painter::ImageID image_id):
	TextField(top_parent, border_style, border_width, image_id) {
}

FileNameField::FileNameField(Component* top_parent, const Color& color):
	TextField(top_parent, color) {
}

FileNameField::FileNameField(Component* top_parent, Painter::ImageID image_id):
	TextField(top_parent, image_id) {
}

FileNameField::~FileNameField() {
}

PopupList* FileNameField::CreatePopupList() {
	str search_string = strutil::Encode(GetText());
	search_string += "*";

	Painter* painter = 0;
	DesktopWindow* desktop_window = (DesktopWindow*)GetParentOfType(kDesktopwindow);
	if (desktop_window != 0) {
		painter = desktop_window->GetPainter();
	}

	std::list<FileInfo> file_list;
	DiskFile::FindData find_data;
	bool ok = DiskFile::FindFirst(search_string, find_data);
	while (ok) {
		bool file_extension_ok = true;

		if (file_extension_list_.empty() == false && find_data.IsSubDir() == false) {
			str _extension = Path::GetExtension(find_data.GetName());
			strutil::ToLower(_extension);
			file_extension_ok = std::find(file_extension_list_.begin(), file_extension_list_.end(), _extension) != file_extension_list_.end();
		}

		if (file_extension_ok == true) {
			file_list.push_back(FileInfo(find_data.GetName(), find_data.IsSubDir()));
		}
		ok = DiskFile::FindNext(find_data);
	}

	PopupList* popup_list = 0;

	if (!file_list.empty()) {
		popup_list = new PopupList(kBorderSunken, 3, WHITE);
		popup_list->SetStyle(ListControl::kSingleSelect);

		file_list.sort();

		std::list<FileInfo>::iterator iter;
		int label_height = 0;
		for (iter = file_list.begin(); iter != file_list.end(); ++iter) {
			Label* label = new Label(GetTextColor(), wstrutil::Encode((*iter).name_));
			label->SetPreferredSize(0, 12);
			popup_list->AddChild(label);

			if (iter == file_list.begin()) {
				label_height = label->GetPreferredHeight(true);
			}
		}

		popup_list->SetPreferredHeight(label_height * (int)std::min(popup_list->GetNumChildren(), 10));
	}

	return popup_list;
}

bool FileNameField::OnKeyDown(uilepra::InputManager::KeyCode key_code) {
	Parent::OnKeyDown(key_code);
	switch (key_code) {
		case uilepra::InputManager::kInKbdEnter: {
			FinalizeSelection();
		} break;
		default: break;
	}
	return (false);
}

bool FileNameField::NotifyDoubleClick(PopupList*, int mouse_x, int mouse_y) {
	bool return_value = true;

	Component* item = GetPopupList()->GetFirstSelectedItem();

	if (item != 0 && item->IsOver(mouse_x, mouse_y)) {
		FinalizeSelection();
		return_value = false;
	}

	return return_value;
}

void FileNameField::ValidatePath(str& path) {
	str normalized_path;
	if (!Path::NormalizePath(path, normalized_path)) {
		// TRICKY: nothing to do - this is not a path!
		return;
	}

	DiskFile::FindData find_data;
	bool ok = DiskFile::FindFirst(normalized_path, find_data);
	if (ok == true) {
		if (find_data.IsSubDir() && strutil::Right(normalized_path, 1) != "/") {
			path += '/';
		}
	} else {
		// TODO: verify code! How can the path exist if FindFirst did not find the path? Contradicting, to say the least.
		ok = DiskFile::PathExists(normalized_path);
		if (ok && strutil::Right(normalized_path, 2) == "..") {
			normalized_path += '/';
		}
	}
	path = normalized_path;
}

size_t FileNameField::FindSlash(str& path, int n) {
	size_t index = path.length();
	if (index > 0) {
		int i = 0;
		do {
			int index1 = (int)path.rfind('/', index-1);
			int index2 = (int)path.rfind('\\', index-1);
			index = index1 > index2 ? index1 : index2;

			i++;
		} while(i < n);
	}
	return index;
}

void FileNameField::FinalizeSelection() {
	if (GetPopupList() != 0) {
		str text = strutil::Encode(GetText());

		if (!text.empty()) {
			// We have to remove everything from the last '/' or '\\'.
			// Why?
			// 1. If the user wrote "C:\Program" and then selected
			//    "Program Files", "Program Files" should replace "Program".
			// 2. If the user wrote "C:\Program Files\", nothing is removed.
			size_t index = FindSlash(text, 0);
			text.erase(index + 1, text.length() - (index + 1));
		}

		ValidatePath(text);
		Label* selected_item = (Label*)GetPopupList()->GetFirstSelectedItem();
		text += strutil::Encode(selected_item->GetText());

		ValidatePath(text);
		const wstr wide_text = wstrutil::Encode(text);
		SetText(wide_text);

		DestroyPopupList();
		SetMarkerPos(wide_text.length());
		SetKeyboardFocus();
	}
}

void FileNameField::AddFileExtension(const str& extension) {
	str _extension(extension);
	strutil::ToLower(_extension);
	file_extension_list_.push_back(_extension);
}

}
