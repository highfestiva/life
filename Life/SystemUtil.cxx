
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "SystemUtil.h"
#include "../Cure/Include/RuntimeVariable.h"
#include "../Lepra/Include/Posix/MacLog.h"



namespace Life
{



void SystemUtil::SaveRtvar(Cure::RuntimeVariableScope* pScope, const str& pRtvarName)
{
	if (!pScope->IsDefined(pRtvarName))
	{
		return;
	}
#ifdef LEPRA_IOS
	const str lValue = pScope->GetUntypedDefaultValue(Cure::RuntimeVariableScope::READ_ONLY, pRtvarName);
	NSString* key = MacLog::Encode(pRtvarName);
	NSString* object = MacLog::Encode(lValue);
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:object forKey:key];
#endif // iOS
}

void SystemUtil::LoadRtvar(Cure::RuntimeVariableScope* pScope, const str& pRtvarName)
{
	(void)pScope;
	(void)pRtvarName;
#ifdef LEPRA_IOS
	NSString* key = MacLog::Encode(pRtvarName);
	NSUserDefaults* defaults = [NSUserDefaults standardUserDefaults];
	NSString* value = [defaults stringForKey:key];
	if (value != nil)
	{
		const str lValue = MacLog::Decode(value);
		pScope->SetUntypedValue(Cure::RuntimeVariable::USAGE_SYS_OVERRIDE, pRtvarName, lValue);
	}
#endif // iOS
}



}
