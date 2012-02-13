
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#pragma once
#include "UiDialog.h"



namespace UiTbc
{



class MessageDialog: public Dialog
{
	typedef Dialog Parent;
public:
	MessageDialog(Component* pParent, Action pOk, const str& pMessage);
	virtual ~MessageDialog();

private:
	virtual void Repaint(Painter* pPainter);
};



}
