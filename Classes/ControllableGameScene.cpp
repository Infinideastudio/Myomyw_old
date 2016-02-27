#include "ControllableGameScene.h"

bool ControllableGameScene::init()
{
	if (!GameScene::init())
		return false;
	auto ejectorTouchListener = EventListenerTouchOneByOne::create();//<-���ֻ����ӵ�Layer
	ejectorTouchListener->onTouchBegan = CC_CALLBACK_2(ControllableGameScene::ejectorTouchBeganCallback, this);
	ejectorTouchListener->onTouchEnded = CC_CALLBACK_2(ControllableGameScene::ejectorTouchEndedCallback, this);
	ejectorTouchListener->setSwallowTouches(true);//Ŀǰ�Ӳ��Ӷ�û�£����˲����¼����´���
	auto eventDispatcher = Director::getInstance()->getEventDispatcher();
	eventDispatcher->addEventListenerWithSceneGraphPriority(ejectorTouchListener, this);
	return true;
}

bool ControllableGameScene::ejectorTouchBeganCallback(Touch* touch, Event* event)
{
	for (int i = 0; i < (turn == left ? lCol : rCol); i++) {
		Vec2 point = touch->getLocation() - ejectorNode->getChildByTag(turn == left ? i : lCol + i)->getPosition();
		if (point.x + point.y<halfDiagonal && point.x + point.y>-halfDiagonal && point.x - point.y>-halfDiagonal && point.y - point.x > -halfDiagonal) {
			holdingTouching = true;
			activateEjector(i);
		}
	}
	return true;//true��ʾ���������moved��ended�¼�
}

void ControllableGameScene::ejectorTouchEndedCallback(Touch* touch, Event* event)
{
	holdingTouching = false;
	if (moveByTouching && cooling) {
		cooling = false;
		changeTurn();
	}
}

void ControllableGameScene::activateEjector(int col)
{
	if (!moving && !cooling) {
		moveByTouching = true;
		beginMoving(col, getNextChessman());
	}
}

void ControllableGameScene::beginMoving(int col, Chessman chessman)
{
	if (moveByTouching) {
		cooling = false;
		if (holdingTouching) {
			GameScene::beginMoving(col, chessman);
			totalMovementTimes++;
		}
		else {
			changeTurn();
		}
	}
	else {
		GameScene::beginMoving(col, chessman);
	}
}

void ControllableGameScene::endMoving()
{
	GameScene::endMoving();
	if (moveByTouching) {
		if (holdingTouching) {
			if (totalMovementTimes >= maxMovementTimes) {
				changeTurn();
			}
			else {
				scheduleOnce(CC_CALLBACK_0(ControllableGameScene::beginMoving, this, movingCol, getNextChessman()), movingCooling, "cooling");
				cooling = true;
			}
		}
		else {
			changeTurn();
		}

	}
}

void ControllableGameScene::changeTurn()
{
	GameScene::changeTurn();
	totalMovementTimes = 0;
	moveByTouching = false;
	holdingTouching = false;
}

Chessman ControllableGameScene::getNextChessman()
{
	switch (rand() % 10)
	{
	case 0:
		return Chessman::key;
	case 1:
		return Chessman::addCol;
	case 2:
		return Chessman::delCol;
	default:
		return Chessman::common;
	}
}