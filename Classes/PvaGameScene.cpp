#include "PvaGameScene.h"

bool PvaGameScene::init()
{
	if (!ControllableGameScene::init())
		return false;
	return true;
}

void PvaGameScene::activateEjector(int col)
{
	if (turn == left) {
		ControllableGameScene::activateEjector(col);
	}
}

void PvaGameScene::endMoving()
{
	if (turn == right) {
		GameScene::endMoving();
		if (AIMovementTimes > 0) {
			AIMovementTimes--;
			auto delayAction = DelayTime::create(movingCooling);
			auto callingAction = CallFunc::create(CC_CALLBACK_0(PvaGameScene::beginMoving, this, AIMovingCol, getRandomChessman()));
			auto delayAndCallingAction = Sequence::create(delayAction, callingAction, NULL);
			this->runAction(delayAndCallingAction);
		}
		else {
			changeTurn();
		}
	}
	else {
		ControllableGameScene::endMoving();
	}
}

void PvaGameScene::changeTurn()
{
	ControllableGameScene::changeTurn();
	if (turn == right) {
		AIMovementTimes = rand() % maxMovementTimes;//这里的移动次数其实已经比实际的移动次数少1了(因为下面就移了一格)
		AIMovingCol = rand() % rCol;
		beginMoving(AIMovingCol, getRandomChessman());
	}
}
