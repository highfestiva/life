/*
	Class:  CenterLayout
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/GUI/UiCenterLayout.h"
#include "../../Include/GUI/UiComponent.h"

namespace UiTbc
{

CenterLayout::CenterLayout() :
	mChild(0)
{
}

CenterLayout::~CenterLayout()
{
}

Layout::Type CenterLayout::GetType()
{
	return Layout::CENTERLAYOUT;
}

void CenterLayout::Add(Component* pComponent, int /*pParam1*/, int /*pParam2*/)
{
	if (mChild == 0)
	{
		mChild = pComponent;
	}
}

void CenterLayout::Remove(Component* pComponent)
{
	if (mChild == pComponent)
	{
		mChild = 0;
	}
}

Component* CenterLayout::Find(const Lepra::String& pComponentName)
{
	if (mChild && mChild->GetName() == pComponentName)
	{
		return mChild;
	}

	return 0;
}

int CenterLayout::GetNumComponents() const
{
	return (mChild == 0) ? 0 : 1;
}

Component* CenterLayout::GetFirst()
{
	return mChild;
}

Component* CenterLayout::GetNext()
{
	return 0;
}

Component* CenterLayout::GetLast()
{
	return mChild;
}

Component* CenterLayout::GetPrev()
{
	return 0;
}

void CenterLayout::UpdateLayout()
{
	if (mChild != 0)
	{
		Lepra::PixelCoords lOwnerSize(GetOwner()->GetSize());
		Lepra::PixelCoords lChildSize(mChild->GetPreferredSize());

		if (lChildSize.x > lOwnerSize.x)
		{
			lChildSize.x = lOwnerSize.x;
		}
		if (lChildSize.y > lOwnerSize.y)
		{
			lChildSize.y = lOwnerSize.y;
		}

		mChild->SetPos((lOwnerSize.x - lChildSize.x) / 2,
						 (lOwnerSize.y - lChildSize.y) / 2);
		mChild->SetSize(lChildSize);
	}
}

Lepra::PixelCoords CenterLayout::GetPreferredSize(bool pForceAdaptive)
{
	if (mChild != 0)
	{
		return mChild->GetPreferredSize(pForceAdaptive);
	}
	else
	{
		return Lepra::PixelCoords(0, 0);
	}
}

Lepra::PixelCoords CenterLayout::GetMinSize()
{
	if (mChild != 0)
	{
		return mChild->GetMinSize();
	}
	else
	{
		return Lepra::PixelCoords(0, 0);
	}
}

Lepra::PixelCoords CenterLayout::GetContentSize()
{
	if (mChild != 0)
	{
		return mChild->GetPreferredSize();
	}
	else
	{
		return Lepra::PixelCoords(0, 0);
	}
}

} // End namespace.