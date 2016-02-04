#pragma once
#include "cocos2d.h"
#include "ControllableGameScene.h"
class PvpGameScene :public ControllableGameScene
{
public:
	virtual bool init();
	CREATE_FUNC(PvpGameScene);
};
