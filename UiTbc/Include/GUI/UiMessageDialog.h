
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine



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