#pragma once
#include "cocos2d.h"
#include "GameScene.h"
#include "GameDefinitions.h"

class ControllableGameScene :public GameScene
{
public:
	virtual bool init();
	CREATE_FUNC(ControllableGameScene);
protected:
	bool holdingTouching = false;//发射器是否保持按下
	int totalMovementTimes = 0;//总共的移动次数
	bool moveByTouching = false;//按下发射器引发的移动
	bool cooling = false;

	bool ejectorTouchBeganCallback(Touch* touch, Event* event);
	void ejectorTouchEndedCallback(Touch* touch, Event* event);
	virtual void activateEjector(int col);
	virtual void beginMoving(int col, Chessman chessman);
	virtual void endMoving();
	virtual void changeTurn();
	virtual Chessman getNextChessman();
};