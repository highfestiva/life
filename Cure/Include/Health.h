
// Author: Jonas Byström
// Copyright (c) 2002-2009, Righteous Games

#pragma once



namespace Cure
{


class ContextObject;
class FloatAttribute;



class Health
{
public:
	static FloatAttribute* GetAttribute(const ContextObject* pObject);
	static float Get(const ContextObject* pObject);
	static float Add(ContextObject* pObject, float pDelta, bool pLowLimit);
	static void Set(ContextObject* pObject, float pValue);
	static void DeleteAttribute(ContextObject* pObject);
};



}
