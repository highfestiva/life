
// Author: Jonas Byström
// Copyright (c) 2002-2010, Righteous Games



#include "../../Include/GUI/UiMessageDialog.h"



namespace UiTbc
{



MessageDialog::MessageDialog(Component* pParent, Action pOk, const str& pMessage):
	Parent(pParent, pOk)
{

	SetPreferredSize(PixelCoord(310, 170));
	SetSize(GetPreferredSize());
	SetQueryLabel(pMessage, FontManager::INVALID_FONTID);
}

MessageDialog::~MessageDialog()
{
}



void MessageDialog::Repaint(Painter* pPainter)
{
	if (mButtonList.empty())
	{
		AddButton(100, _T("OK"));
	}
	Parent::Repaint(pPainter);
}



}
