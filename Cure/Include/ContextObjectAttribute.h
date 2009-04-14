
// Author: Jonas Byström
// Copyright (c) 2002-2007, Righteous Games



#pragma once

namespace Cure
{



class ContextObject;



class ContextObjectAttribute
{
public:
	enum Type
	{
		TYPE_ENGINE,
	};

	ContextObjectAttribute(ContextObject* pContextObject);
	virtual ~ContextObjectAttribute();

	virtual Type GetType() const = 0;

protected:
	ContextObject* GetContextObject() const;

private:
	ContextObject* mContextObject;
};



}
