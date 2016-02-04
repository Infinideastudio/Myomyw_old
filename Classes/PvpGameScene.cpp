#include "PvpGameScene.h"

bool PvpGameScene::init()
{
	if (!ControllableGameScene::init())
		return false;
	return true;
}