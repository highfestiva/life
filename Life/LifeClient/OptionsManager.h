
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "../../lepra/include/hirestimer.h"
#include "../../lepra/include/log.h"
#include "../../uilepra/include/uiinput.h"
#include "../life.h"



namespace cure {
class RuntimeVariableScope;
}



namespace life {
namespace options {



struct OptionsManager {
	typedef std::vector<float*> ValueArray;

	OptionsManager(cure::RuntimeVariableScope* variable_scope, int priority);

	void RefreshConfiguration();
	virtual void DoRefreshConfiguration();
	bool UpdateInput(uilepra::InputManager::KeyCode key_code, bool active);
	virtual bool UpdateInput(uilepra::InputElement* element);
	void ResetToggles();
	bool IsToggleConsole() const;

	ValueArray* GetValuePointers(const str& key, bool& is_any_steering_value);

protected:
	typedef std::pair<const str, float*> KeyValue;

	virtual bool SetDefault(int priority);

	static const str ConvertToString(uilepra::InputManager::KeyCode key_code);
	bool SetValue(const str& key, float value, bool add);
	void SetValuePointers(const KeyValue entries[], size_t entry_count);

	struct InputEntry {
		bool is_any_steering_value_;
		ValueArray value_pointer_array_;
	};
	typedef std::unordered_map<str, InputEntry> InputMap;

	cure::RuntimeVariableScope* variable_scope_;
	float console_toggle_;
	HiResTimer last_config_refresh_;
	InputMap input_map_;

	void operator=(const OptionsManager&);

	logclass();
};



}
}
