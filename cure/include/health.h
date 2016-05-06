
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#pragma once



namespace cure {


class ContextObject;
class FloatAttribute;



class Health {
public:
	static FloatAttribute* GetAttribute(const ContextObject* object);
	static float Get(const ContextObject* object, float _default = 0);
	static float Add(ContextObject* object, float delta, bool low_limit);
	static void Set(ContextObject* object, float value);
	static void DeleteAttribute(ContextObject* object);
};



}
