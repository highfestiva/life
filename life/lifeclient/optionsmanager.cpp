
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "optionsmanager.h"
#include "../../lepra/include/lepraassert.h"
#include "../../cure/include/runtimevariable.h"
#include "../../lepra/include/cyclicarray.h"
#include "../../lepra/include/math.h"
#include "rtvar.h"



namespace life {
namespace options {



OptionsManager::OptionsManager(cure::RuntimeVariableScope* variable_scope, int priority):
	variable_scope_(variable_scope),
	console_toggle_(0) {
	SetDefault(priority);
	DoRefreshConfiguration();
}



void OptionsManager::RefreshConfiguration() {
	if (last_config_refresh_.QueryTimeDiff() > 5.0) {
		last_config_refresh_.ClearTimeDiff();
		DoRefreshConfiguration();
	}
}

void OptionsManager::DoRefreshConfiguration() {
	const KeyValue _entries[] =
	{
		KeyValue(kRtvarCtrlUiContoggle, &console_toggle_),
	};
	SetValuePointers(_entries, LEPRA_ARRAY_COUNT(_entries));
}

bool OptionsManager::UpdateInput(uilepra::InputManager::KeyCode key_code, bool active) {
	const str input_element_name = ConvertToString(key_code);
	return (SetValue(input_element_name, active? 1.0f : 0.0f, false));
}

bool OptionsManager::UpdateInput(uilepra::InputElement* element) {
	(void)element;
	return false;
}

void OptionsManager::ResetToggles() {
	console_toggle_ = 0;
}

bool OptionsManager::IsToggleConsole() const {
	return console_toggle_ >= 0.5f;
}

OptionsManager::ValueArray* OptionsManager::GetValuePointers(const str& key, bool& is_any_steering_value) {
	InputMap::iterator x = input_map_.find(key);
	if (x == input_map_.end()) {
		return 0;
	}
	InputEntry& entry = x->second;
	is_any_steering_value = entry.is_any_steering_value_;
	return &entry.value_pointer_array_;
}



bool OptionsManager::SetDefault(int) {
	v_override(variable_scope_, kRtvarCtrlUiContoggle, "Key.F11");
	return (true);
}

const str OptionsManager::ConvertToString(uilepra::InputManager::KeyCode key_code) {
	return ("Key."+uilepra::InputManager::GetKeyName(key_code));
}

bool OptionsManager::SetValue(const str& key, float value, bool add) {
	//log_volatile(log_.Tracef("Got input %s: %g", key.c_str(), value));

	bool _is_any_steering_value;
	ValueArray* value_pointers = GetValuePointers(key, _is_any_steering_value);
	if (!value_pointers) {
		return false;
	}

	bool input_changed = false;
	for (ValueArray::const_iterator x = value_pointers->begin(); x != value_pointers->end(); ++x) {
		if (add) {
			input_changed = true;
			*(*x) += Math::Clamp(value, -1.0f, 1.0f);
		} else if (!Math::IsEpsEqual(*(*x), value, 0.06f)) {
			input_changed = true;
			*(*x) = value;
		}
	}
	return (_is_any_steering_value && input_changed);
}

void OptionsManager::SetValuePointers(const KeyValue entries[], size_t entry_count) {
	input_map_.clear();
	for (size_t x = 0; x < entry_count; ++x) {
		const str keys = variable_scope_->GetDefaultValue(cure::RuntimeVariableScope::kReadOnly, entries[x].first);
		typedef strutil::strvec SV;
		SV key_array = strutil::Split(keys, ", \t");
		for (SV::iterator y = key_array.begin(); y != key_array.end(); ++y) {
			const bool is_steering_value = (entries[x].first.find("Steer") != str::npos);
			const str& _key = *y;
			InputMap::iterator z = input_map_.find(_key);
			if (z == input_map_.end()) {
				InputEntry entry;
				entry.is_any_steering_value_ = is_steering_value;
				entry.value_pointer_array_.push_back(entries[x].second);
				input_map_.insert(InputMap::value_type(_key, entry));
			} else {
				InputEntry& entry = z->second;
				entry.is_any_steering_value_ |= is_steering_value;
				entry.value_pointer_array_.push_back(entries[x].second);
			}
		}
	}
}



void OptionsManager::operator=(const OptionsManager&) {
	deb_assert(false);
}



loginstance(kUiInput, OptionsManager);



}
}
