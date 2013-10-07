
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "SystemUtil.h"



namespace Life
{



void SystemUtil::SaveRtvar(Cure::RuntimeVariableScope* pScope, const str& pRtvarName)
{
	if (!pScope->IsDefined(pRtvarName))
	{
		return;
	}
#ifdef LEPRA_IOS
	const str lValue = pScope->GetDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, pRtvarName);
	NSString* key = [MacLog::Encode(pRtvarName) retain];
	NSString* object = [MacLog::Encode(lValue) retain];
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:object forKey:key];
	[key release];
	[object release];
#endif // iOS
}

void SystemUtil::LoadRtvar(Cure::RuntimeVariableScope* pScope, const str& pRtvarName)
{
	(void)pRtvarName;
#ifdef LEPRA_IOS
	NSString* key = [MacLog::Encode(pRtvarName) retain];
	NSString* value = [defaults stringForKey:key];
	const str lValue = MacLog::Decode(value);
	pScope->SetValue(Cure::RuntimeVariable::USAGE_NORMAL, pRtvarName, lValue);
	[key release];
#endif // iOS
}



}
