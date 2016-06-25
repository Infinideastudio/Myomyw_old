#include "PvpGameScene.h"
#include "Text.h"

bool PvpGameScene::init()
{
	if (!GameScene::init())
		return false;
	setNames(Text::get("green"), Text::get("blue"));
	setNextChessman(getRandomchessman());
	controllable = true;
	return true;
}

void PvpGameScene::beginMoving(int col)
{
	GameScene::beginMoving(col);
	setNextChessman(getRandomchessman());
}
