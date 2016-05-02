#pragma once
#include "cocos2d.h"
#include "GameScene.h"

class PvaGameScene :public GameScene
{
public:
	virtual bool init();
	CREATE_FUNC(PvaGameScene);
private:
	int AIMovementTimes;
	int AIMovingCol;
	void endMoving();
	void changeTurn();
	void leftWins();
	void rightWins();
	void AIMove();
};