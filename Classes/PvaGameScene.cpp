#include "PvaGameScene.h"
#include "Lang.h"
#include "Player.h"
#include "ResultScene.h"

bool PvaGameScene::init()
{
	if (!GameScene::init())
		return false;
	setNames(Player::getName(), Lang::get("machine"));
	setNextChessman(getRandomchessman());
	controllable = true;
	return true;
}

void PvaGameScene::beginMoving(int col)
{
	GameScene::beginMoving(col);
	setNextChessman(getRandomchessman());
}

void PvaGameScene::endMoving()
{
	Side originalTurn = turn;//ȥ�����endMoving�ű��right�����
	GameScene::endMoving();
	if (originalTurn == right) {
		if (AIMovementTimes > 0) {
			AIMovementTimes--;
			scheduleOnce([this](float) {beginMoving(movingCol); }, movingCooling, "cool");
		}
		else {
			changeTurn();
		}
	}
}

void PvaGameScene::changeTurn()
{
	GameScene::changeTurn();
	if (turn == right) {
		controllable = false;
		//�л��غϺ���ȴһ������AI��(��������̫ͻȻ)
		scheduleOnce(CC_CALLBACK_0(PvaGameScene::AIMove, this), movingCooling, "changeCool");
	}
	else {
		controllable = true;
	}
}

void PvaGameScene::leftWins()
{
	auto rs = ResultScene::create(Lang::get("playerWins"), Color4B(0, 255, 0, 255));
	Director::getInstance()->replaceScene(rs);
}

void PvaGameScene::rightWins()
{
	auto rs = ResultScene::create(Lang::get("aiWins"), Color4B(0, 0, 0, 255));
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
	movingCol = bestCol;
	//�ƶ���������Ȩ�ض���
	int times = round(5 * maxWeighting) + rand() % 3;
	//ȷ��ʵ���ƶ�����
	if (times < 1) {
		AIMovementTimes = 1;
	}
	else if (times > maxMovementTimes) {
		AIMovementTimes = maxMovementTimes;
	}
	else {
		AIMovementTimes = times;
	}
	beginMoving(movingCol);
	AIMovementTimes--;
}
