#include "SlidingMenu.h"

SlidingMenuGrid::SlidingMenuGrid()
{
	
}

SlidingMenuGrid::~SlidingMenuGrid()
{
	
}

void SlidingMenuGrid::onExit()
{
//    Director::getInstance()->getTouchDispatcher()->removeDelegate(this);
}

Scene* SlidingMenuGrid::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = SlidingMenuGrid::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool SlidingMenuGrid::init()
{
	if ( !CCLayer::init() )
	{
		return false;
	}
    
    Vector<Ref *> menuItems;
    
    //set up the menu item sprites (the buttons in the grid)
    
    auto *muhSprite1 = Sprite::create("Stats_Default.png");
    auto *muhSprite1_Click = Sprite::create("Stats_Default_Click.png");
    auto menuSprite1 = MenuItemSprite::create(muhSprite1, muhSprite1_Click);
    // or if you want to call a function on click
    // auto menuSprite1 = MenuItemSprite::create(muhSprite1, muhSprite1_Click, CC_CALLBACK_1(YourNameSpace::YourFunction, arg));

    auto *muhSprite2 = Sprite::create("Single_Default.png");
    auto *muhSprite2_Click = Sprite::create("Single_Default_Click.png");
    auto menuSprite2 = MenuItemSprite::create(muhSprite2, muhSprite2_Click);

    auto *muhSprite3 = Sprite::create("Multi_Default.png");
    auto *muhSprite3_Click = Sprite::create("Multi_Default_Click.png");
    auto menuSprite3 = MenuItemSprite::create(muhSprite3, muhSprite3_Click);
    
    menuItems.pushBack(menuSprite1);
    menuItems.pushBack(menuSprite2);
    menuItems.pushBack(menuSprite3);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    
   	//create a sliding menu grid object
    SlidingMenuGrid* myGrid = SlidingMenuGrid::menuWithArray(menuItems, 1, 2, Point(origin.x + visibleSize.width, origin.y + visibleSize.height), Point(10, 20), false);

    //add it to the layer
    this->addChild(myGrid, 0);

    
    
    
	return true;
}

SlidingMenuGrid* SlidingMenuGrid::menuWithArray(Vector<Ref *> items, int cols, int rows, Point pos , Point pad)
{
	return menuWithArray(items,cols,rows,pos,pad,false);
}


SlidingMenuGrid* SlidingMenuGrid::menuWithArray(Vector<Ref *> items, int cols, int rows, Point pos , Point pad, bool vertical)
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

bool SlidingMenuGrid::initWithArray(Vector<Ref *> items, int cols, int rows, Point pos , Point pad, bool vertical)
{
	if( !CCLayer::init() )
	{
		return false;
	}
    
	selectedItem = NULL;
	//cocos2d-x 3 requires us to set up our own setTouchEnabled Function
	setTouchEnabled(true);
	this->pMenu= Menu::create();
    

    MenuItemSprite *getItem;
    
    // add each menu item sprite as a child to the Menu object
    for (Ref* object : items){
        getItem = (MenuItemSprite*)object;
        pMenu->addChild(getItem, 1, getItem->getTag());
    }
    
    this->addChild(pMenu,0);

    
	padding = pad;
	iCurrentPage = 0;
	bMoving = false;
	iMenuRows=rows;
	menuOrigin = pos;
	fMoveDeadZone = 10;
	bVerticalPaging = vertical;
	fAnimSpeed = 1;
	state=Menu::State::WAITING;
	(bVerticalPaging) ? this->buildGridVertical(cols ,rows) : this->buildGrid(cols, rows);
	this->setPosition(menuOrigin);

	return true;
}


void SlidingMenuGrid::buildGrid(int cols, int rows)
{
	Size winSize = Director::getInstance()->getWinSize();
    
	int col = 0, row = 0;
	Vector<Node*> child = pMenu->getChildren();
	
	Point position=getPosition();
	iPageCount=0;
	
	//for each menuitemsprite, set the correct position
    for (Ref* item : child){
		MenuItemSprite* getItem = (MenuItemSprite*) item;
		getItem->setPosition(Point(position.x + menuOrigin.x + col * padding.x + (iPageCount * winSize.width), position.y +(winSize.height-menuOrigin.y)- row * padding.y));
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

	if(child.size() > rows*cols*iPageCount)//   <-- add code for FIX (Mr.  K pop)
	{
		iPageCount++;
	}
}



void SlidingMenuGrid::buildGridVertical(int cols, int rows)
{
	Size winSize = Director::getInstance()->getWinSize();
    
	int col = 0, row = 0;
	Vector<Node*> child = pMenu->getChildren();

	Point position=getPosition();
	iPageCount=0;
    for (Ref* item : child ) {
		MenuItemSprite* getItem = (MenuItemSprite*) item;
		getItem->setPosition(Point(position.x + menuOrigin.x+col * padding.x , position.y +(winSize.height-menuOrigin.y)- row * padding.y - (iPageCount * winSize.height)));
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
	if(child.size() > rows*cols*iPageCount)//   <-- add code for FIX (Mr.  K pop)
	{
		iPageCount++;
	}
}

void SlidingMenuGrid::setTouchEnabled(bool enabled) {
    
    auto myListener = EventListenerTouchOneByOne::create();
    myListener->onTouchBegan = CC_CALLBACK_2(SlidingMenuGrid::touchBegan, this);
    myListener->onTouchMoved = CC_CALLBACK_2(SlidingMenuGrid::touchMoved, this);
    myListener->onTouchEnded = CC_CALLBACK_2(SlidingMenuGrid::touchEnded, this);
    myListener->onTouchCancelled = CC_CALLBACK_2(SlidingMenuGrid::touchCancelled, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(myListener, this);
}


MenuItemSprite* SlidingMenuGrid::GetItemWithinTouch(Touch* touch)
{
    Point touchLocation = Director::getInstance()->convertToGL(touch->getLocationInView());
    
	// Parse our menu items and see if our touch exists within one.
	Vector<Node*> child = pMenu->getChildren();
	

    for (Ref* item : child){
		
		MenuItemSprite * getItem= static_cast<MenuItemSprite *>(item);
        
		Point local = getItem->convertToNodeSpace(touchLocation);
		Rect r = getItem->rect();
		r.origin = Point::ZERO;// If the touch was within this item. Return the item.
		if (r.containsPoint(local))
		{
			return getItem;
		}
	}
	// Didn't touch an item.
	return NULL;
}

bool SlidingMenuGrid::touchBegan(Touch* touch, Event* event)
{
    touchOrigin = Director::getInstance()->convertToGL(touch->getLocationInView());
    
    // If we weren't in "waiting" state bail out.
	if (state == Menu::State::WAITING)
	{
		// Activate the menu item if we are touching one.
		if(!bMoving)
		{
			selectedItem = GetItemWithinTouch(touch);
			if(selectedItem)
			{
				if(selectedItem->isEnabled())
					selectedItem->selected();
			}
		}
		state = Menu::State::TRACKING_TOUCH;
	}
	return true;
}

void SlidingMenuGrid::touchCancelled(Touch* touch, Event* event)
{
	if(selectedItem)
	{
		selectedItem->unselected();
		selectedItem = NULL;
		state = Menu::State::WAITING;
	}
}

void SlidingMenuGrid::touchMoved(Touch* touch, Event* event)
{
	if(GetItemWithinTouch(touch) == NULL && selectedItem)
	{
		//Touch Cancelled
		if(selectedItem->isEnabled())
		{
			selectedItem->unselected();
		}
		selectedItem = NULL;
		state = Menu::State::WAITING;
		return;
	}
    
	if(GetItemWithinTouch(touch) != NULL && selectedItem)
	{
		return;
	}
	// Calculate the current touch point during the move.
    touchStop = Director::getInstance()->convertToGL(touch->getLocationInView());// Distance between the origin of the touch and current touch point.
	fMoveDelta = (bVerticalPaging) ? (touchStop.y - touchOrigin.y) : (touchStop.x - touchOrigin.x);// Set our position.
	setPosition(GetPositionOfCurrentPageWithOffset(fMoveDelta));
	bMoving = true;
}

void SlidingMenuGrid::touchEnded(Touch* touch, Event* event)
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
			if(selectedItem->isEnabled())
			{
				selectedItem->unselected();
				selectedItem->activate();
			}
			//selectedItem=NULL;
		}
	}
	state = Menu::State::WAITING;
}

void SlidingMenuGrid::moveToCurrentPage(bool animated)
{
	Size winSize = Director::getInstance()->getWinSize();
    
    if (animated)
    {
    	//animations are currently broken - to be implemented later
//        EaseBounce* action =EaseBounce::initWithAction(CCMoveTo::actionWithDuration(fAnimSpeed*0.3f, GetPositionOfCurrentPage()));
//        EaseBounce* action ;
//        action->initWithAction(MoveTo::initWithDuration(fAnimSpeed*0.3f, GetPositionOfCurrentPage()))
//        runAction(action);
        setPosition(GetPositionOfCurrentPage());
    }
    else
    {
        setPosition(GetPositionOfCurrentPage());
    }
}

Point SlidingMenuGrid::GetPositionOfCurrentPage()
{
	Size winSize = Director::getInstance()->getWinSize();
	Point position=getPosition();
	if(bVerticalPaging)
		position=Point(0,iCurrentPage*winSize.height);
	else
		position=Point(-(iCurrentPage)*winSize.width,0);
	return position;
}

Point SlidingMenuGrid::GetPositionOfCurrentPageWithOffset(float offset)
{
	Size winSize = Director::getInstance()->getWinSize();
	Point position=GetPositionOfCurrentPage();
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

void SlidingMenuGrid::gotoPage(int iPage, bool animated)
{
	int iSetPage=iPage;
	if(iPage<=0)
		iSetPage=1;
	else if(iPage>iPageCount)
	{
		iSetPage=iPageCount;
	}
	iCurrentPage=iSetPage-1;
	moveToCurrentPage(animated);
}

void SlidingMenuGrid::setPageLabel( int iPage,Node * pLabel)
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
	Size winSize = Director::getInstance()->getWinSize();
	Size labelSize= pLabel->getContentSize();
	Point pt=getPosition();
	if(bVerticalPaging)
		pt=Point(menuOrigin.x+labelSize.width/2,winSize.height-menuOrigin.y-winSize.height*(iSetPage-1)-iMenuRows*padding.y-labelSize.height/2);
	else
		pt=Point(menuOrigin.x+(iSetPage-1)*winSize.width+labelSize.width/2,winSize.height-menuOrigin.y-iMenuRows*padding.y-labelSize.height/2);
	pLabel->setPosition(pt);
}