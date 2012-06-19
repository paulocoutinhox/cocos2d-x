#include "SlidingMenu.h"

SlidingMenuGrid::SlidingMenuGrid() 
{
	
}

SlidingMenuGrid::~SlidingMenuGrid() 
{
	
}

void SlidingMenuGrid::onExit()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

bool SlidingMenuGrid::init()
{
	if ( !CCLayer::init() )
	{
		return false;
	}
	return true;
}

SlidingMenuGrid* SlidingMenuGrid::menuWithArray(CCArray *items, int cols, int rows, CCPoint pos , CCPoint pad)
{ 
	return menuWithArray(items,cols,rows,pos,pad,false);
}


SlidingMenuGrid* SlidingMenuGrid::menuWithArray(CCArray *items, int cols, int rows, CCPoint pos , CCPoint pad, bool vertical)
{
	SlidingMenuGrid *slidingMenu = new SlidingMenuGrid();
	if (slidingMenu && slidingMenu->initWithArray(items ,cols ,rows ,pos ,pad ,vertical))
	{
		slidingMenu->autorelease();
		return slidingMenu;
	}
	CC_SAFE_DELETE(slidingMenu);
	return NULL;
}

bool SlidingMenuGrid::initWithArray(CCArray *items, int cols, int rows, CCPoint pos , CCPoint pad, bool vertical)
{
	if( !CCLayer::init() )
	{
		return false;
	}

	selectedItem = NULL;
	setIsTouchEnabled(true);
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,0,true);
	pMenu=new CCMenu;
	addChild(pMenu,0);

    CCObject *object;
    CCMenuItemSprite *getItem;

    CCARRAY_FOREACH(items, object)
    {
        getItem = (CCMenuItemSprite*)object;
        pMenu->addChild(getItem, 1, getItem->getTag());
    }

	padding = pad;
	iCurrentPage = 0;
	bMoving = false;
	iMenuRows=rows;
	menuOrigin = pos;
	fMoveDeadZone = 10;
	bVerticalPaging = vertical;
	fAnimSpeed = 1;
	state=kCCMenuStateWaiting;
	(bVerticalPaging) ? this->buildGridVertical(cols ,rows) : this->buildGrid(cols, rows);
	this->setPosition(menuOrigin);
	return true;
}


void SlidingMenuGrid::buildGrid(int cols, int rows)
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	int col = 0, row = 0;
	CCArray* child = pMenu->getChildren();
	CCObject* item;
	CCPoint position=getPosition();
	iPageCount=0;
	CCARRAY_FOREACH(child, item)
	{
		CCMenuItemSprite* getItem = (CCMenuItemSprite*) item;
		getItem->setPosition(ccp(position.x +  menuOrigin.x + col * padding.x + (iPageCount * winSize.width), position.y +(winSize.height-menuOrigin.y)- row * padding.y));
		++col;
		if (col == cols)
		{
			col = 0;
			++row;
			if( row == rows )
			{
				iPageCount++;
				col = 0;
				row = 0;
			}
		}
		
	}
	if(child->count() > rows*cols*iPageCount)//   <-- add code for FIX (Mr.  K pop)
	{
		iPageCount++;
	}	
}



void SlidingMenuGrid::buildGridVertical(int cols, int rows)
{ 
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();

	int col = 0, row = 0;
	CCArray* child = pMenu->getChildren();
	CCObject* item;
	CCPoint position=getPosition();
	iPageCount=0;
	CCARRAY_FOREACH(child, item)
	{
		CCMenuItemSprite* getItem = (CCMenuItemSprite*) item;
		getItem->setPosition(ccp(position.x + menuOrigin.x+col * padding.x , position.y +(winSize.height-menuOrigin.y)- row * padding.y - (iPageCount * winSize.height)));
		++col;
		if (col == cols)
		{
			col = 0;
			++row;
			if( row == rows )
			{
				iPageCount++;
				col = 0;
				row = 0;
			}
		}

	}
	if(child->count() > rows*cols*iPageCount)//   <-- add code for FIX (Mr.  K pop)
	{
		iPageCount++;
	}
}


CCMenuItemSprite* SlidingMenuGrid::GetItemWithinTouch(CCTouch* touch)
{
    CCPoint touchLocation = CCDirector::sharedDirector()->convertToGL(touch->locationInView());

	// Parse our menu items and see if our touch exists within one.
	CCArray* child = pMenu->getChildren();
	CCObject* item;
	CCARRAY_FOREACH(child, item)
	{
		
		CCMenuItemSprite * getItem= static_cast<CCMenuItemSprite *>(item);
				
		CCPoint local = getItem->convertToNodeSpace(touchLocation);
		CCRect r = getItem->rect();
		r.origin = CCPointZero;// If the touch was within this item. Return the item.
		if (CCRect::CCRectContainsPoint(r, local))
		{
			return getItem;			
		}
	}
	// Didn't touch an item. 
	return NULL;
}

bool SlidingMenuGrid::ccTouchBegan(CCTouch* touch, CCEvent* event)
{
    touchOrigin = CCDirector::sharedDirector()->convertToGL(touch->locationInView());

    // If we weren't in "waiting" state bail out.
	if (state == kCCMenuStateWaiting)
	{ 
		// Activate the menu item if we are touching one.
		if(!bMoving)
		{
			selectedItem = GetItemWithinTouch(touch);
			if(selectedItem)
			{
				if(selectedItem->getIsEnabled())
					selectedItem->selected();
			}
		}
		state = kCCMenuStateTrackingTouch;
	}
	return true;
}

void SlidingMenuGrid::ccTouchCancelled(CCTouch* touch, CCEvent* event)
{
	if(selectedItem)
	{
		selectedItem->unselected();
		selectedItem = NULL;
		state = kCCMenuStateWaiting;
	}
}

void SlidingMenuGrid::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
	if(GetItemWithinTouch(touch) == NULL && selectedItem)
	{
		//Touch Cancelled
		if(selectedItem->getIsEnabled())
		{
			selectedItem->unselected();
		}
		selectedItem = NULL;
		state = kCCMenuStateWaiting; 
		return;
	}

	if(GetItemWithinTouch(touch) != NULL && selectedItem)
	{
		return;
	}
	// Calculate the current touch point during the move.
    touchStop = CCDirector::sharedDirector()->convertToGL(touch->locationInView());// Distance between the origin of the touch and current touch point.
	fMoveDelta = (bVerticalPaging) ? (touchStop.y - touchOrigin.y) : (touchStop.x - touchOrigin.x);// Set our position.    
	setPosition(GetPositionOfCurrentPageWithOffset(fMoveDelta));
	bMoving = true;
}

void SlidingMenuGrid::ccTouchEnded(CCTouch* touch, CCEvent* event)
{
	if( bMoving )
	{
		bMoving = false;
		{			
			// Do we have multiple pages?
			if( iPageCount > 1 && (fMoveDeadZone < abs(fMoveDelta)))
			{
				// Are we going forward or backward?
				bool bForward = (fMoveDelta > 0) ? true : false;
				
				// Do we have a page available?
				if(bVerticalPaging)
				{
					if(bForward && (iPageCount>iCurrentPage+1))
					{
						// Increment currently active page.
						iCurrentPage++;
					}
					else if(!bForward && (iCurrentPage > 0))
					{
						// Decrement currently active page.
						iCurrentPage--;
					}
				}
				else
				{
					if(!bForward && (iPageCount>iCurrentPage+1))
					{
						// Increment currently active page.
						iCurrentPage++;
					}
					else if(bForward && (iCurrentPage > 0))
					{
						// Decrement currently active page.
						iCurrentPage--;
					}
				}
				
				// Start sliding towards the current page.
				moveToCurrentPage();
			}
			else 
			{
				if(selectedItem)
				{
					selectedItem->unselected();
					selectedItem->activate();
				}
			}				
		}
	}
	else
	{
		//Only touch
		if(selectedItem)
		{
			if(selectedItem->getIsEnabled())
			{
				selectedItem->unselected();
				selectedItem->activate();
			}
			//selectedItem=NULL;			
		}
	}
	state = kCCMenuStateWaiting;
}

void SlidingMenuGrid::moveToCurrentPage()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	// Perform the action
	CCEaseBounce* action =CCEaseBounce::actionWithAction(CCMoveTo::actionWithDuration(fAnimSpeed*0.3f, GetPositionOfCurrentPage()));
	runAction(action);
}

CCPoint SlidingMenuGrid::GetPositionOfCurrentPage()
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCPoint position=getPosition();
	if(bVerticalPaging)
		position=CCPointMake(0,iCurrentPage*winSize.height);
	else
		position=CCPointMake(-(iCurrentPage)*winSize.width,0);
	return position;
}

CCPoint SlidingMenuGrid::GetPositionOfCurrentPageWithOffset(float offset)
{
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCPoint position=GetPositionOfCurrentPage();
	if(bVerticalPaging)
		position.y+=offset;
	else
		position.x+=offset;		
	return position;
}

// Returns the swiping dead zone. 
float SlidingMenuGrid::GetSwipeDeadZone()
{
	return fMoveDeadZone;
}

void SlidingMenuGrid::SetSwipeDeadZone(float fValue) 
{
	fMoveDeadZone = fValue;
}

// Returns wheather or not vertical paging is enabled.
bool SlidingMenuGrid::IsVerticallyPaged()
{
	return bVerticalPaging; 
}

// Sets the vertical paging flag.
void SlidingMenuGrid::SetVerticalPaging(bool bValue) 
{
	bVerticalPaging = bValue;
	// this->buildGridVertical();
}

void SlidingMenuGrid::gotoPage(int iPage)
{
	int iSetPage=iPage;
	if(iPage<=0)
		iSetPage=1;
	else if(iPage>iPageCount)
	{
		iSetPage=iPageCount;
	}
	iCurrentPage=iSetPage-1;
	moveToCurrentPage();
}

void SlidingMenuGrid::setPageLabel( int iPage,CCNode * pLabel)
{
	addChild(pLabel,10);
	if(!pLabel)
		return;
	int iSetPage=iPage;
	if(iPage<=0)
		iSetPage=1;
	else if(iPage>iPageCount)
	{
		iSetPage=iPageCount;
	}
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCSize labelSize= pLabel->getContentSize();
	CCPoint pt=getPosition();
	if(bVerticalPaging)
		pt=CCPointMake(menuOrigin.x+labelSize.width/2,winSize.height-menuOrigin.y-winSize.height*(iSetPage-1)-iMenuRows*padding.y-labelSize.height/2);
	else
		pt=CCPointMake(menuOrigin.x+(iSetPage-1)*winSize.width+labelSize.width/2,winSize.height-menuOrigin.y-iMenuRows*padding.y-labelSize.height/2);
	pLabel->setPosition(pt);
}
