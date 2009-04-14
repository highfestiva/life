/*
	Class:  FloatingLayout
	Author: Alexander Hugestrand
	Copyright (c) 2002-2006, Alexander Hugestrand
*/

#include "../../Include/GUI/UiFloatingLayout.h"
#include "../../Include/GUI/UiWindow.h"

namespace UiTbc
{

FloatingLayout::FloatingLayout() :
	Layout(),
	mX(POSOFFSET),
	mY(POSOFFSET),
	mPosCount(0)
{
}

FloatingLayout::~FloatingLayout()
{
}

Layout::Type FloatingLayout::GetType()
{
	return Layout::FLOATINGLAYOUT;
}

void FloatingLayout::Add(Component* pComponent, int pParam1, int pParam2)
{
	if (mChildList.empty() == false)
	{
		Component* lChild = *(--mChildList.end());
		if (lChild->GetType() == Component::WINDOW)
		{
			((Window*)lChild)->SetActive(false);
		}
	}

	mChildList.push_back(pComponent);
	mChildList.unique();

	if (pComponent->GetType() == Component::WINDOW)
	{
		((Window*)pComponent)->SetActive(true);
	}

	if (pParam1 != 0 || pParam2 != 0)
	{
		pComponent->SetPos(pParam1, pParam2);
	}
	else
	{
		pComponent->SetPos(mX, mY);
		mX += POSOFFSET;
		mY += POSOFFSET;
		mPosCount++;

		if (mPosCount >= 10)
		{
			mX = POSOFFSET;
			mY = POSOFFSET;
			mPosCount = 0;
		}
	}
}

void FloatingLayout::Remove(Component* pComponent)
{
	mChildList.remove(pComponent);

	if (!mChildList.empty())
	{
		Component* lComp = *(--mChildList.end());
		if (lComp->GetType() == Component::WINDOW)
		{
			((Window*)lComp)->SetActive(true);
		}
	}
}

Component* FloatingLayout::Find(const Lepra::String& pComponentName)
{
	ComponentList::iterator lIter;

	for (lIter = mChildList.begin(); lIter != mChildList.end(); ++lIter)
	{
		if ((*lIter)->GetName() == pComponentName)
		{
			return (*lIter);
		}
	}

	return 0;
}

int FloatingLayout::GetNumComponents() const
{
	return (int)mChildList.size();
}

Component* FloatingLayout::GetFirst()
{
	mIter = mChildList.begin();
	if (mIter != mChildList.end())
	{
		return *mIter;
	}

	return 0;
}

Component* FloatingLayout::GetNext()
{
	++mIter;
	if (mIter != mChildList.end())
	{
		return *mIter;
	}

	return 0;
}

Component* FloatingLayout::GetLast()
{
	if(mChildList.empty())
	{
		mIter = mChildList.begin();
		return 0;
	}
	else
	{
		mIter = --mChildList.end();
		return *mIter;
	}
}

Component* FloatingLayout::GetPrev()
{
	if (mIter != mChildList.begin())
	{
		return *(--mIter);
	}

	return 0;
}

void FloatingLayout::UpdateLayout()
{
	ComponentList::iterator lIter;

	for (lIter = mChildList.begin(); lIter != mChildList.end(); ++lIter)
	{
		Component* lChild = *lIter;
		if (lChild->GetSize() != lChild->GetPreferredSize())
		{
			lChild->SetSize(lChild->GetPreferredSize());
		}
	}
}

Lepra::PixelCoords FloatingLayout::GetPreferredSize(bool /*pForceAdaptive*/)
{
	return Lepra::PixelCoords(0, 0);
}

Lepra::PixelCoords FloatingLayout::GetMinSize()
{
	return Lepra::PixelCoords(0, 0);
}

Lepra::PixelCoords FloatingLayout::GetContentSize()
{
	return Lepra::PixelCoords(0, 0);
}

void FloatingLayout::MoveToTop(Component* pComponent)
{
	if (mChildList.empty() == false)
	{
		Component* lLast = *(--mChildList.end());
		if (lLast->GetType() == Component::WINDOW &&
		   lLast != pComponent)
		{
			((Window*)lLast)->SetActive(false);
		}

		mChildList.remove(pComponent);
		mChildList.push_back(pComponent);
	}
}

} // End namespace.