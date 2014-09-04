
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "pch.h"
#include "../../Include/GUI/UiRadioButton.h"



namespace UiTbc
{



RadioButton::RadioButton(const Color& pBodyColor, const str& pName):
	Parent(pBodyColor, pName)
{
	SetBaseColor(GetColor());
}

RadioButton::~RadioButton()
{
}

void RadioButton::SetBaseColor(const Color& pColor)
{
	Parent::SetBaseColor(pColor);
	mPressColor = mHooverColor * 1.6f;
}

bool RadioButton::OnLButtonUp(int pMouseX, int pMouseY)
{
	if (IsOver(pMouseX, pMouseY) == true)
	{
		Component* lParent = GetParent();
		Layout* lLayout = lParent->GetLayout();

		// Update all the other radio buttons.
		Component* lChild = lLayout->GetFirst();
		while (lChild != 0)
		{
			if (lChild != this && lChild->GetType() == Component::RADIOBUTTON)
			{
				RadioButton* lButton = (RadioButton*)lChild;
				lButton->SetPressed(false);
			}

			lChild = lLayout->GetNext();
		}

		SetPressed(true);
		Click(true);
	}
	else
	{
		// Go back to previous state.
		switch(GetState())
		{
		case RELEASING:
			SetState(PRESSED);
			break;
		case PRESSING:
			SetState(RELEASED);
			break;
		default:
			break;
		}
	}

	ReleaseMouseFocus();
	return true;
}

Component::Type RadioButton::GetType() const
{
	return Component::RADIOBUTTON;
}



}
