#pragma once
#include "cocos2d.h"
#include "GameScene.h"
class PvpGameScene :public GameScene
{
public:
	virtual bool init();
	CREATE_FUNC(PvpGameScene);
private:
	void changeTurn();
};