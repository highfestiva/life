
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once

#include "cure.h"
#include "contextobjectattribute.h"



namespace cure {



class IntAttribute: public ContextObjectAttribute {
	typedef ContextObjectAttribute Parent;
public:
	IntAttribute(ContextObject* context_object, const str& name, int value);
	virtual ~IntAttribute();

	int GetValue() const;
	void SetValue(int value);

private:
	virtual int QuerySend() const;	// Returns number of bytes it needs to send.
	virtual int Pack(uint8* destination);
	virtual int Unpack(const uint8* source, int max_size);	// Retuns number of bytes unpacked, or -1.

	bool is_updated_;
	int value_;

	void operator=(const IntAttribute&);
};



}
