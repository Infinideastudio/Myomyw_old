#include "PvpGameScene.h"
#include "Text.h"

bool PvpGameScene::init()
{
	if (!ControllableGameScene::init())
		return false;
	setNames(Text::get("green"), Text::get("blue"));
	return true;
}