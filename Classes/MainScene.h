#pragma once
#include "cocos2d.h"
#include "ui\UIEditBox\UIEditBox.h"

class MainScene :public cocos2d::Scene
{
public:
	virtual bool init();
	CREATE_FUNC(MainScene);
private:
	cocos2d::ui::EditBox* addressBox;

	void pvoItemCallback(Ref* pSender);
};