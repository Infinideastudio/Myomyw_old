#include "PvpGameScene.h"
#include "Text.h"

bool PvpGameScene::init()
{
	if (!GameScene::init())
		return false;
	setNames(Text::get("green"), Text::get("blue"));
	controllable = true;
	return true;
}