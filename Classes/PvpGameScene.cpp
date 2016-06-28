#include "PvpGameScene.h"
#include "Lang.h"

bool PvpGameScene::init()
{
	if (!GameScene::init())
		return false;
	setNames(Lang::get("green"), Lang::get("blue"));
	setNextChessman(getRandomchessman());
	controllable = true;
	return true;
}

void PvpGameScene::beginMoving(int col)
{
	GameScene::beginMoving(col);
	setNextChessman(getRandomchessman());
}
