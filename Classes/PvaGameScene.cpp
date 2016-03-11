#include "PvaGameScene.h"
#include "Text.h"
#include "ResultScene.h"

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
	//下面的代码貌似能简化对吧，但别简化。。。
	if (turn == right) {
		ControllableGameScene::endMoving();
		if (AIMovementTimes > 0) {
			AIMovementTimes--;
			scheduleOnce(CC_CALLBACK_0(PvaGameScene::beginMoving, this, AIMovingCol, getNextChessman()), movingCooling, "cooling");
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
		scheduleOnce(CC_CALLBACK_0(PvaGameScene::AIMove, this), movingCooling, "changing");
	}
}

void PvaGameScene::leftWins()
{
	auto rs = ResultScene::create(Text::get("playerWins"), Color4B(0, 255, 0, 255));
	Director::getInstance()->replaceScene(rs);
}

void PvaGameScene::rightWins()
{
	auto rs = ResultScene::create(Text::get("aiWins"), Color4B(0, 0, 0, 255));
	Director::getInstance()->replaceScene(rs);
}

void PvaGameScene::AIMove()
{
	//极为简单(愚蠢)的AI
	float maxWeighting = -100;//最大的权重
	int bestCol = 0;//权重最大的一列
	for (int i = 0; i < rCol; i++) {
		float weighting = 0;//这一行的权重
		for (int j = 0; j < lCol; j++) {
			float change = 0;
			switch (chessmen[j][i]) {
			case Chessman::key:
				//对最下面一列的红球给予特别特别关注
				if (j == lCol - 1)
					change = -6;
				else
					change = -3;
				break;
			case Chessman::addCol:
				change = -1;
				break;
			case Chessman::delCol:
				change = 1;
				break;
			}
			//离出口越近权重越大(加上一点随机)
			weighting += change*(j / (float)lCol);
		}
		if (weighting > maxWeighting) {
			maxWeighting = weighting;
			bestCol = i;
		}
	}
	AIMovingCol = bestCol;
	//移动次数根据权重而变
	int times = round(5 * maxWeighting) + rand() % 3;
	//确定实际移动次数
	if (times < 1) {
		AIMovementTimes = 1;
	}
	else if (times>maxMovementTimes) {
		AIMovementTimes = maxMovementTimes;
	}
	else {
		AIMovementTimes = times;
	}
	beginMoving(AIMovingCol, getNextChessman());
	AIMovementTimes--;
}
