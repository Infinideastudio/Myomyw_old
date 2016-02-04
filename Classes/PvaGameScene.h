#pragma once
#include "cocos2d.h"
#include "ControllableGameScene.h"

class PvaGameScene :public ControllableGameScene
{
public:
	virtual bool init();
	CREATE_FUNC(PvaGameScene);
private:
	int AIMovementTimes;
	int AIMovingCol;
	void activateEjector(int col);
	void endMoving();
	void changeTurn();
};