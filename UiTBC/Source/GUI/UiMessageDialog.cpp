
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



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
		AddButton(100, _T("OK"), true);
	}
	Parent::Repaint(pPainter);
}



}
