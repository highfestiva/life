
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "systemutil.h"
#include "../cure/include/runtimevariable.h"
#include "../lepra/include/posix/maclog.h"



namespace life {



void SystemUtil::SaveRtvar(cure::RuntimeVariableScope* scope, const str& rtvar_name) {
	if (!scope->IsDefined(rtvar_name)) {
		return;
	}
#ifdef LEPRA_IOS
	const str __value = scope->GetUntypedDefaultValue(cure::RuntimeVariableScope::kReadOnly, rtvar_name);
	NSString* key = MacLog::Encode(rtvar_name);
	NSString* object = MacLog::Encode(__value);
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:object forKey:key];
#endif // iOS
}

void SystemUtil::LoadRtvar(cure::RuntimeVariableScope* scope, const str& rtvar_name) {
	(void)scope;
	(void)rtvar_name;
#ifdef LEPRA_IOS
	NSString* key = MacLog::Encode(rtvar_name);
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	NSString* value = [defaults stringForKey:key];
	if (value != nil) {
		const str __value = MacLog::Decode(value);
		scope->SetUntypedValue(cure::RuntimeVariable::kUsageSysOverride, rtvar_name, __value);
	}
#endif // iOS
}



}
