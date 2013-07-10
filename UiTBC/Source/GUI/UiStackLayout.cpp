/*
	Class:  StackLayout
	Author: Jonas Byström
	Copyright (c) Pixel Doctrine
*/

#include "../../Include/GUI/UiStackLayout.h"
#include "../../Include/GUI/UiComponent.h"

namespace UiTbc
{

StackLayout::StackLayout(int pLevels) :
	Layout(),
	mChild(0),
	mNumLevels(pLevels)
{
	if (mNumLevels > 0)
	{
		mChild = new Component*[mNumLevels];
		for (int i = 0; i < mNumLevels; i++)
		{
			mChild[i] = 0;
		}
	}
}

StackLayout::~StackLayout()
{
	if (mNumLevels > 0)
	{
		delete[] mChild;
	}
}

Layout::Type StackLayout::GetType() const
{
	return Layout::STACKLAYOUT;
}

void StackLayout::Add(Component* pComponent, int pParam1, int /*pParam2*/)
{
	if (pParam1 >= 0 && pParam1 < mNumLevels)
	{
		while (pParam1 < mNumLevels && mChild[pParam1] != 0)
		{
			pParam1++;
		}

		if (pParam1 < mNumLevels)
		{
			mChild[pParam1] = pComponent;
			mNumComponents++;
		}
	}
}

void StackLayout::Remove(Component* pComponent)
{
	for (int i = 0; i < mNumLevels; i++)
	{
		if (mChild[i] == pComponent)
		{
			mChild[i] = 0;
			mNumComponents--;
		}
	}
}

Component* StackLayout::Find(const str& pComponentName)
{
	for (int i = 0; i < mNumLevels; i++)
	{
		if (mChild[i] != 0 && mChild[i]->GetName() == pComponentName)
		{
			return mChild[i];
		}
	}

	return 0;
}

int StackLayout::GetNumComponents() const
{
	return mNumComponents;
}

Component* StackLayout::GetFirst()
{
	for (mCurrentLevel = 0; 
		mCurrentLevel < mNumLevels; 
		mCurrentLevel++)
	{
		if (mChild[mCurrentLevel] != 0)
		{
			return mChild[mCurrentLevel];
		}
	}

	return 0;
}

Component* StackLayout::GetNext()
{
	for (++mCurrentLevel;
		mCurrentLevel >= 0 && mCurrentLevel < mNumLevels;
		mCurrentLevel++)
	{
		if (mChild[mCurrentLevel] != 0)
		{
			return mChild[mCurrentLevel];
		}
	}

	return 0;
}

Component* StackLayout::GetLast()
{
	for (mCurrentLevel = mNumLevels - 1; 
		mCurrentLevel >= 0;
		mCurrentLevel--)
	{
		if (mChild[mCurrentLevel] != 0)
		{
			return mChild[mCurrentLevel];
		}
	}

	return 0;
}

Component* StackLayout::GetPrev()
{
	for (--mCurrentLevel;
		mCurrentLevel >= 0 && mCurrentLevel < mNumLevels;
		mCurrentLevel--)
	{
		if (mChild[mCurrentLevel] != 0)
		{
			return mChild[mCurrentLevel];
		}
	}

	return 0;
}

void StackLayout::UpdateLayout()
{
	PixelCoord lOwnerSize(GetOwner()->GetSize());

	for (int i = 0; i < mNumLevels; i++)
	{
		if (mChild[i] != 0)
		{
			mChild[i]->SetPos(0, 0);
			mChild[i]->SetSize(lOwnerSize);
		}
	}
}

PixelCoord StackLayout::GetPreferredSize(bool pForceAdaptive)
{
	PixelCoord lSize(0, 0);
	for (int i = 0; i < mNumLevels; i++)
	{
		if (mChild[i] != 0)
		{
			PixelCoord lTemp(mChild[i]->GetPreferredSize(pForceAdaptive));
			if (lTemp.x > lSize.x)
			{
				lSize.x = lTemp.x;
			}
			if (lTemp.y > lSize.y)
			{
				lSize.y = lTemp.y;
			}
		}
	}

	return lSize;
}

PixelCoord StackLayout::GetMinSize() const
{
	PixelCoord lSize(0, 0);
	for (int i = 0; i < mNumLevels; i++)
	{
		if (mChild[i] != 0)
		{
			PixelCoord lTemp(mChild[i]->GetMinSize());
			if (lTemp.x > lSize.x)
			{
				lSize.x = lTemp.x;
			}
			if (lTemp.y > lSize.y)
			{
				lSize.y = lTemp.y;
			}
		}
	}

	return lSize;
}

PixelCoord StackLayout::GetContentSize() const
{
	PixelCoord lSize(0, 0);
	for (int i = 0; i < mNumLevels; i++)
	{
		if (mChild[i] != 0)
		{
			PixelCoord lTemp(mChild[i]->GetPreferredSize());
			if (lTemp.x > lSize.x)
			{
				lSize.x = lTemp.x;
			}
			if (lTemp.y > lSize.y)
			{
				lSize.y = lTemp.y;
			}
		}
	}

	return lSize;
}

} // End namespace.
