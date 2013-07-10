
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once



namespace Cure
{


class ContextObject;
class FloatAttribute;



class Health
{
public:
	static FloatAttribute* GetAttribute(const ContextObject* pObject);
	static float Get(const ContextObject* pObject, float pDefault = 0);
	static float Add(ContextObject* pObject, float pDelta, bool pLowLimit);
	static void Set(ContextObject* pObject, float pValue);
	static void DeleteAttribute(ContextObject* pObject);
};



}
