
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#pragma once
#include "uidialog.h"



namespace uitbc {



class MessageDialog: public Dialog {
	typedef Dialog Parent;
public:
	MessageDialog(Component* parent, Action ok, const wstr& message);
	virtual ~MessageDialog();

private:
	virtual void Repaint(Painter* painter);
};



}
