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
	//����Ĵ���ò���ܼ򻯶԰ɣ�����򻯡�����
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
	//��Ϊ��(�޴�)��AI
	float maxWeighting = -100;//����Ȩ��
	int bestCol = 0;//Ȩ������һ��
	for (int i = 0; i < rCol; i++) {
		float weighting = 0;//��һ�е�Ȩ��
		for (int j = 0; j < lCol; j++) {
			float change = 0;
			switch (chessmen[j][i]) {
			case Chessman::key:
				//��������һ�еĺ�������ر��ر��ע
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
			//�����Խ��Ȩ��Խ��(����һ�����)
			weighting += change*(j / (float)lCol);
		}
		if (weighting > maxWeighting) {
			maxWeighting = weighting;
			bestCol = i;
		}
	}
	AIMovingCol = bestCol;
	//�ƶ���������Ȩ�ض���
	int times = round(5 * maxWeighting) + rand() % 3;
	//ȷ��ʵ���ƶ�����
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
