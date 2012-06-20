
#ifndef __SLIDING_MENU_GRID_H
#define __SLIDING_MENU_GRID_H
/*
*Addon class for cocos2d-x framework (http://www.cocos2d-x.org) 
*Based on class Brandon Reynolds  http://brandonreynolds.com/blog/2011/01/09/cocos2d-sliding-menu-grid/,
*and port sources Mr.  K pop(http://www.cocos2d-x.org/boards/6/topics/9803) 
* Make by Starfair studio (starfair@inbox.ru)
* v1.0 29.03.2012 Add Label (CCLabelTTF *)for any pages (see metod  setPageLabel())
* v1.1 30.03.2012 Tag number saved in each CCItemMenu * in CCMenuItemsArray not rewrite inside SlidingMenuGrid (see metod initWithArray()) 
*				  and change logic to set items on pages (first added set on first page)
* v1.2 19.03.2012 Fixed a bug where a locked item indicated by the touch as the chosen.
				  Fixed a bug page counter by Mr.  K pop (http://www.cocos2d-x.org/boards/18/topics/9931?r=10275#message-10275	)
				  Added the ability to be used as a descriptor page of any object pointer CCNode *
* v1.3 20.06.2012 Paulo Coutinho - Changed method gotoPage to change page without animation, useful when menu is created.
*/

#include "cocos2d.h"

USING_NS_CC;

class SlidingMenuGrid:public CCLayer
{
protected:
	void buildGrid(int cols, int rows);
	void buildGridVertical(int cols, int rows);
	void moveToCurrentPage(bool animated = true);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchCancelled(CCTouch *pTouch, CCEvent *pEvent);
	CCPoint GetPositionOfCurrentPage();
	CCPoint GetPositionOfCurrentPageWithOffset(float offset);
	CCMenuItemSprite* GetItemWithinTouch(CCTouch* touch);

	tCCMenuState state; // State of our menu grid. (Eg. waiting, tracking touch, cancelled, etc)
	CCMenuItem *selectedItem; // Menu item that was selected/active.

	CCPoint padding; // Padding in between menu items. 
	CCPoint menuOrigin; // Origin position of the entire menu grid.
	CCPoint touchOrigin; // Where the touch action began.
	CCPoint touchStop; // Where the touch action stopped.

	int iPageCount; // Number of pages in this grid.
	int iCurrentPage; // Current page of menu items being viewed.

	bool bMoving; // Is the grid currently moving?
	bool bSwipeOnlyOnMenu; // Causes swiping functionality to only work when siping on top of the menu items instead of entire screen.
	bool bVerticalPaging; // Disabled by default. Allows for pages to be scrolled vertically instead of horizontal.

	float fMoveDelta; // Distance from origin of touch and current frame.
	float fMoveDeadZone; // Amount they need to slide the grid in order to move to a new page.
	float fAnimSpeed; // 0.0-1.0 value determining how slow/fast to animate the paging.

	int iMenuRows;
	CCMenu * pMenu;
public:
	SlidingMenuGrid();
	~SlidingMenuGrid();
	virtual bool init();
	virtual void onExit();
    static SlidingMenuGrid* menuWithArray(CCArray *items, int cols, int rows, CCPoint pos , CCPoint pad);
    static SlidingMenuGrid* menuWithArray(CCArray *items, int cols, int rows, CCPoint pos , CCPoint pad, bool vertical);
    bool initWithArray(CCArray *items, int cols, int rows, CCPoint pos , CCPoint pad, bool vertical);
	float GetSwipeDeadZone();
	void SetSwipeDeadZone(float fValue);
	bool IsVerticallyPaged();
	void SetVerticalPaging(bool bValue);
	int getPageCount(){return iPageCount;}
	
	void gotoPage(int iPage=0, bool animated = true);
	void setPageLabel(int,CCNode *);//Add label on the page
};
#endif
