
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../Include/Health.h"
#include "../Include/ContextObject.h"
#include "../Include/FloatAttribute.h"

#define HEALTH_NAME	_T("float_health")



namespace Cure
{



FloatAttribute* Health::GetAttribute(const ContextObject* pObject)
{
	return (FloatAttribute*)pObject->GetAttribute(HEALTH_NAME);
}

float Health::Get(const ContextObject* pObject, float pDefault)
{
	FloatAttribute* lHealth = GetAttribute(pObject);
	if (lHealth)
	{
		return lHealth->GetValue();
	}
	return pDefault;
}

float Health::Add(ContextObject* pObject, float pDelta, bool pLowLimit)
{
	float lValue = Get(pObject) + pDelta;
	if (pLowLimit)
	{
		lValue = std::max(0.0f, lValue);
	}
	Set(pObject, lValue);
	return lValue;
}

void Health::Set(ContextObject* pObject, float pValue)
{
	FloatAttribute* lHealth = GetAttribute(pObject);
	if (lHealth)
	{
		lHealth->SetValue(pValue);
	}
	else
	{
		new FloatAttribute(pObject, HEALTH_NAME, pValue);
	}
}

void Health::DeleteAttribute(ContextObject* pObject)
{
	pObject->DeleteAttribute(HEALTH_NAME);
}



}
