#pragma once
#include "cocos2d.h"
#include "ui\UIEditBox\UIEditBox.h"
USING_NS_CC;

class MainScene :public cocos2d::Scene
{
public:
	virtual bool init();
	CREATE_FUNC(MainScene);
private:
	ui::EditBox *nameBox, *addressBox;
	Layer* scrollableLayer;
	Label* playerLabel;
	MenuItemLabel* pvoItem;

	void moveToMainLayer();
	void moveToLoginLayer();
	void updatePlayerLabel();
};