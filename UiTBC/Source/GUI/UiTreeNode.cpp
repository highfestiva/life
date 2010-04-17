/*
	Class:  TreeNode
	Author: Alexander Hugestrand
	Copyright (c) 2002-2009, Righteous Games
*/

#include "../../Include/GUI/UiTreeNode.h"
#include "../../Include/GUI/UiGridLayout.h"
#include "../../Include/GUI/UiCenterLayout.h"
#include "../../Include/GUI/UiListControl.h"
#include "../../Include/UiPainter.h"
#include "../../../Lepra/Include/ListUtil.h"

namespace UiTbc
{

TreeNode::TreeNode(const str& pText, const str& pName) :
	Component(pName, new GridLayout(1, 1)),
	mParentNode(0),
	mChildNodes(0),
	mExpandButton(0),
	mIconLabel(0),
	mCollapseIconID(Painter::INVALID_IMAGEID),
	mExpandIconID(Painter::INVALID_IMAGEID),
	mCollapsedIconID(Painter::INVALID_IMAGEID),
	mExpandedIconID(Painter::INVALID_IMAGEID),
	mIndentationLevel(0),
	mChildIndentation(16)
{
	Init(pText);
}

TreeNode::TreeNode(Painter::ImageID pCollapsedIconID,
		   Painter::ImageID pExpandedIconID,
		   const str& pText, 
		   const str& pName) :
	Component(pName, new GridLayout(1, 1)),
	mParentNode(0),
	mChildNodes(0),
	mExpandButton(0),
	mIconLabel(0),
	mCollapseIconID(Painter::INVALID_IMAGEID),
	mExpandIconID(Painter::INVALID_IMAGEID),
	mCollapsedIconID(pCollapsedIconID),
	mExpandedIconID(pExpandedIconID),
	mIndentationLevel(0),
	mChildIndentation(16)
{
	Init(pText);
}

TreeNode::TreeNode(Painter::ImageID pCollapsedIconID,
		   Painter::ImageID pExpandedIconID,
		   Painter::ImageID pCollapseIconID,
		   Painter::ImageID pExpandeIconID,
		   const str& pText, 
		   const str& pName) :
	Component(pName, new GridLayout(1, 1)),
	mParentNode(0),
	mChildNodes(0),
	mExpandButton(0),
	mIconLabel(0),
	mCollapseIconID(pCollapseIconID),
	mExpandIconID(pExpandeIconID),
	mCollapsedIconID(pCollapsedIconID),
	mExpandedIconID(pExpandedIconID),
	mIndentationLevel(0),
	mChildIndentation(0)
{
	Init(pText);

	GUIImageManager* lIMan = GetImageManager();
	mChildIndentation = lIMan->GetImageSize(pExpandeIconID).x;
}

TreeNode::~TreeNode()
{
	if (mChildNodes != 0)
	{
		// If expanded, the ListControl will delete these.
		if (mExpandButton->GetPressed() == false)
		{
			ListUtil::DeleteAll(*mChildNodes);
		}

		delete mChildNodes;
		mChildNodes = 0;
	}
}

void TreeNode::Init(const str& pText)
{
	ValidatePainterAndIcons();
	ValidateExpandButton();
	ValidateIconRect(pText);

	SetPreferredSize(0, 0, true);
	int lPreferredHeight = 0;

	if (mExpandButton && mExpandButton->GetPreferredHeight() > lPreferredHeight)
	{
		lPreferredHeight = mExpandButton->GetPreferredHeight();
	}

	if (mIconLabel && mIconLabel->GetPreferredHeight() > lPreferredHeight)
	{
		lPreferredHeight = mIconLabel->GetPreferredHeight();
	}

	SetPreferredHeight(lPreferredHeight);
}

void TreeNode::OnExpandButtonUnclicked(Button* /*pButton*/)
{
	SetExpanded(mExpandButton->GetPressed());
}

void TreeNode::AddChildNode(TreeNode* pChildNode)
{
	if (mChildNodes == 0)
	{
		mChildNodes = new NodeList();
		SetNeedsRepaint(true);
	}
	mChildNodes->push_back(pChildNode);

	pChildNode->SetIndentationLevel(GetIndentationLevel() + mChildIndentation);
	ValidateExpandButton();
}

void TreeNode::SetIndentationLevel(int pIndentationLevel)
{
	int lDiff = pIndentationLevel - mIndentationLevel;
	mIndentationLevel = pIndentationLevel;

	if (mChildNodes != 0)
	{
		NodeList::iterator lIter;
		for (lIter = mChildNodes->begin(); lIter != mChildNodes->end(); ++lIter)
		{
			TreeNode* lNode = (TreeNode*)(*lIter);
			lNode->SetIndentationLevel(lNode->GetIndentationLevel() + lDiff);
		}
	}
}

void TreeNode::SetExpanded(bool pExpanded)
{
	ListControl* lListControl = (ListControl*)Parent::GetParentOfType(Parent::LISTCONTROL);
	if (lListControl != 0)
	{
		SetExpanded(pExpanded, lListControl);
	}
}

void TreeNode::SetExpanded(bool pExpanded, ListControl* pListControl)
{
	SetNeedsRepaint(true);

	if (mChildNodes != 0)
	{
		int lHOffset;
		int lVOffset;
		pListControl->GetScrollOffsets(lHOffset, lVOffset);

		if (pExpanded)
		{
			// Insert all the children in the parent's list control.
			pListControl->AddChildrenAfter(*mChildNodes, this, GetIndentationLevel() + mChildIndentation);
			NodeList::iterator lIter;
			for (lIter = mChildNodes->begin(); lIter != mChildNodes->end(); ++lIter)
			{
				TreeNode* lNode = (TreeNode*)(*lIter);
				if (lNode->IsExpandable() && lNode->IsExpanded())
				{
					lNode->SetExpanded(true, pListControl);
				}
			}
		
			mIconLabel->SetIcon(mExpandedIconID);
		}
		else
		{
			// Remove all the children from the parent's list control.
			NodeList::iterator lIter;
			for (lIter = mChildNodes->begin(); lIter != mChildNodes->end(); ++lIter)
			{
				pListControl->RemoveChild(*lIter, 0);
			}
			for (lIter = mChildNodes->begin(); lIter != mChildNodes->end(); ++lIter)
			{
				TreeNode* lNode = (TreeNode*)(*lIter);
				if (lNode->IsExpandable() && lNode->IsExpanded())
				{
					lNode->SetExpanded(false, pListControl);
				}
			}

			mIconLabel->SetIcon(mCollapsedIconID);
		}

		pListControl->UpdateLayout();
		pListControl->SetScrollOffsets(lHOffset, lVOffset);
	}
}

void TreeNode::SetSelected(bool pSelected)
{
	Parent::SetSelected(pSelected);

	if (mIconLabel != 0)
	{
		mIconLabel->SetSelected(pSelected);
	}
}

void TreeNode::SetKeyboardFocus()
{
	if (mIconLabel != 0)
	{
		//GetTopParent()->ReleaseKeyboardFocus(RECURSE_DOWN, mIconLabel);
		mIconLabel->SetKeyboardFocus();
	}
	else
	{
		Parent::SetKeyboardFocus();
	}
}

void TreeNode::ValidatePainterAndIcons()
{
	ResetUserIcons();
	AddDefaultIconIfNeeded(mExpandIconID, smExpandIconID, smIconExpand, 3, 3);
	AddDefaultIconIfNeeded(mCollapseIconID, smCollapseIconID, smIconCollapse, 3, 3);
	AddDefaultIconIfNeeded(mExpandedIconID, smExpandedIconID, smIconExpanded, 2, 3);
	AddDefaultIconIfNeeded(mCollapsedIconID, smCollapsedIconID, smIconCollapsed, 2, 3);
	ResetUserIcons();
}

void TreeNode::AddDefaultIconIfNeeded(Painter::ImageID& pLocalIconID, Painter::ImageID& pStaticIconID, void* pBuffer, int pXOffset, int pYOffset)
{
	if (pLocalIconID == Painter::INVALID_IMAGEID ||
	    pStaticIconID != Painter::INVALID_IMAGEID)
	{
		Color lPalette[256];
		lPalette[0].Set(0, 0, 0, 0); 
		lPalette[1].Set(0, 0, 0, 255);
		lPalette[2].Set(251, 233, 160, 255);
		lPalette[3].Set(255, 255, 255, 255);
		lPalette[4].Set(255, 0, 0, 255);
		lPalette[106].Set(149, 149, 149, 255);
		lPalette[255].Set(0, 0, 0, 255);

		Canvas lCanvas(16, 16, Canvas::BITDEPTH_8_BIT);
		lCanvas.SetPalette(lPalette);
		lCanvas.SetBuffer(pBuffer);

		Canvas lAlpha;
		uint8 lTransparentColor = 0;
		lCanvas.GetAlphaChannel(lAlpha, &lTransparentColor, 1);
		lCanvas.ConvertTo32BitWithAlpha(lAlpha);

		GUIImageManager* lIMan = GetImageManager();
		pStaticIconID = lIMan->AddImage(lCanvas, GUIImageManager::CENTERED, GUIImageManager::ALPHATEST, 128);
		lIMan->SetImageOffset(pStaticIconID, pXOffset, pYOffset);
	}
}

void TreeNode::ResetUserIcons()
{
	//
	// Use the default icons if not set by user.
	//

	if (mExpandIconID == Painter::INVALID_IMAGEID)
	{
		mExpandIconID = smExpandIconID;
	}
	if (mCollapseIconID == Painter::INVALID_IMAGEID)
	{
		mCollapseIconID = smCollapseIconID;
	}
	if (mExpandedIconID == Painter::INVALID_IMAGEID)
	{
		mExpandedIconID = smExpandedIconID;
	}
	if (mCollapsedIconID == Painter::INVALID_IMAGEID)
	{
		mCollapsedIconID = smCollapsedIconID;
	}
}

bool TreeNode::ValidateExpandButton()
{
	bool lRepaint = false;

	if (mChildNodes != 0 && mExpandButton == 0)
	{
		GUIImageManager* lIMan = GetImageManager();

		mExpandButton = new CheckButton(mExpandIconID, mCollapseIconID,
						 mExpandIconID, mCollapseIconID,
						 mCollapseIconID, mExpandIconID,
						 _T("ExpandButton"));

		mExpandButton->SetPreferredSize(lIMan->GetImageSize(mExpandIconID));
		mExpandButton->SetMinSize(mExpandButton->GetPreferredSize());

		mExpandButton->SetOnClick(TreeNode, OnExpandButtonUnclicked);

		// Create a rect with a CenterLayout in order to keep the button centered.
		RectComponent* lRect = new RectComponent(_T("ExpandButtonRect"), new CenterLayout());
		lRect->AddChild(mExpandButton);

		lRect->SetPreferredSize(mExpandButton->GetPreferredSize());
		lRect->SetMinSize(mExpandButton->GetMinSize());

		if (((GridLayout*)GetLayout())->GetNumCols() > 1)
		{
			Component* lComp = ((GridLayout*)GetLayout())->GetComponentAt(0, 0);
			((GridLayout*)GetLayout())->DeleteColumn(0);
			delete lComp;
		}
		((GridLayout*)GetLayout())->InsertColumn(0);
		AddChild(lRect, 0, 0);

		lRepaint = true;
	}
	else if (mChildNodes == 0 && mExpandButton == 0)
	{
		GUIImageManager* lIMan = GetImageManager();

		RectComponent* lRect = new RectComponent(_T("ExpandButtonRect"), new CenterLayout());

		lRect->SetPreferredSize(lIMan->GetImageSize(mExpandIconID));
		lRect->SetMinSize(lRect->GetPreferredSize());

		((GridLayout*)GetLayout())->InsertColumn(0);
		AddChild(lRect, 0, 0);
	}

	return lRepaint;
}

bool TreeNode::ValidateIconRect(const str& pText)
{
	bool lRepaint = false;

	if (mIconLabel == 0)
	{
		GUIImageManager* lIMan = GetImageManager();

		mIconLabel = new Label(smFactoryTextBackgColor, smFactoryTextSelectedBackgColor);
		mIconLabel->SetIcon(mCollapsedIconID);

		mIconLabel->SetText(pText,
			smFactoryTextColor, smFactoryTextBackgColor, 
			smFactoryTextSelectedColor, smFactoryTextSelectedBackgColor);

		int lImageHeight = lIMan->GetImageSize(mCollapsedIconID).y;
		int lFontHeight = lIMan->GetPainter()->GetFontHeight();
		mIconLabel->SetPreferredHeight(lImageHeight > lFontHeight ? lImageHeight : lFontHeight);

		if (mExpandButton)
		{
			// The layout is 1x2.
			AddChild(mIconLabel, 0, 1);
		}
		else
		{
			// The layout is 1x1.
			AddChild(mIconLabel, 0, 0);
		}

		lRepaint = true;
	}

	return lRepaint;
}

bool TreeNode::OnDoubleClick(int pMouseX, int pMouseY)
{
	if (IsExpandable() && mIconLabel->IsOver(pMouseX, pMouseY) == true)
	{
		bool lExpanded = !IsExpanded();
		mExpandButton->SetPressed(lExpanded);
		SetExpanded(lExpanded);
	}
	return (false);
}

bool TreeNode::OnKeyDown(UiLepra::InputManager::KeyCode pKeyCode)
{
	Parent::OnKeyDown(pKeyCode);

	switch(pKeyCode)
	{
		case UiLepra::InputManager::IN_KBD_LEFT:
		{
			if (IsExpandable() && IsExpanded())
			{
				mExpandButton->SetPressed(false);
				SetExpanded(false);
			}
		}
		break;
		case UiLepra::InputManager::IN_KBD_RIGHT:
		{
			if (IsExpandable() && IsExpanded() == false)
			{
				mExpandButton->SetPressed(true);
				SetExpanded(true);
			}
		}
		break;
		default: break;
	}
	return (false);
}


void TreeNode::UseFont(const Color& pTextColor,
		       const Color& pTextBackgColor,
		       const Color& pTextSelectedColor,
		       const Color& pTextSelectedBackgColor)
{
	smFactoryTextColor = pTextColor;
	smFactoryTextBackgColor = pTextBackgColor;
	smFactoryTextSelectedColor = pTextSelectedColor;
	smFactoryTextSelectedBackgColor = pTextSelectedBackgColor;
}


Painter::ImageID TreeNode::smExpandIconID = Painter::INVALID_IMAGEID;
Painter::ImageID TreeNode::smCollapseIconID = Painter::INVALID_IMAGEID;
Painter::ImageID TreeNode::smExpandedIconID = Painter::INVALID_IMAGEID;
Painter::ImageID TreeNode::smCollapsedIconID = Painter::INVALID_IMAGEID;
Painter* TreeNode::smPrevPainter = 0;

uint8 TreeNode::smIconExpand[] =
{
	106, 106, 106, 106, 106, 106, 106, 106, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3, 255,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3, 255,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3, 255, 255, 255, 255, 255,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3, 255,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3, 255,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106, 106, 106, 106, 106, 106, 106, 106, 106,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

uint8 TreeNode::smIconCollapse[] =
{
	106, 106, 106, 106, 106, 106, 106, 106, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3, 255, 255, 255, 255, 255,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106,   3,   3,   3,   3,   3,   3,   3, 106,   0,   0,   0,   0,   0,   0,   0,
	106, 106, 106, 106, 106, 106, 106, 106, 106,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

uint8 TreeNode::smIconCollapsed[] =
{
	  0,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  1,   2,   2,   2,   2,   2,   2,   1,   0,   0,   0,   0,   0,   0,   0,   0,
	  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

uint8 TreeNode::smIconExpanded[] =
{
	  0,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  1,   2,   2,   2,   2,   2,   2,   1,   0,   0,   0,   0,   0,   0,   0,   0,
	  1,   2,   2,   2,   2,   2,   2,   1,   1,   1,   1,   1,   1,   0,   0,   0,
	  1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   0,   0,   0,
	  1,   2,   2,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
	  1,   2,   2,   2,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,
	  1,   2,   2,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   0,
	  1,   2,   2,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   0,
	  1,   2,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   0,   0,
	  1,   2,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   0,   0,
	  1,   1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   1,   0,   0,   0,
	  1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

Color TreeNode::smFactoryTextColor              = BLACK;
Color TreeNode::smFactoryTextBackgColor         = WHITE;
Color TreeNode::smFactoryTextSelectedColor      = BLACK;
Color TreeNode::smFactoryTextSelectedBackgColor = LIGHT_BLUE;


} // End namespace.
