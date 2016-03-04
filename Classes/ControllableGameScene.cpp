#include "ControllableGameScene.h"

bool ControllableGameScene::init()
{
	if (!GameScene::init())
		return false;
	auto ejectorTouchListener = EventListenerTouchOneByOne::create();//<-这个只是添加到Layer
	ejectorTouchListener->onTouchBegan = CC_CALLBACK_2(ControllableGameScene::ejectorTouchBeganCallback, this);
	ejectorTouchListener->onTouchEnded = CC_CALLBACK_2(ControllableGameScene::ejectorTouchEndedCallback, this);
	ejectorTouchListener->setSwallowTouches(true);//目前加不加都没事，加了不让事件向下传递
	auto eventDispatcher = Director::getInstance()->getEventDispatcher();
	eventDispatcher->addEventListenerWithSceneGraphPriority(ejectorTouchListener, this);
	return true;
}

bool ControllableGameScene::ejectorTouchBeganCallback(Touch* touch, Event* event)
{
	for (int i = 0; i < (turn == left ? lCol : rCol); i++) {
		//判断鼠标是否处于发射器内(45°倾斜的正方形)
		Vec2 point = touch->getLocation() - ejectorNode->getChildByTag(turn == left ? i : lCol + i)->getPosition();
		if (point.x + point.y<halfDiagonal && point.x + point.y>-halfDiagonal && point.x - point.y>-halfDiagonal && point.y - point.x > -halfDiagonal) {
			if (!moving && !cooling) {
				holdingTouching = true;
				activateEjector(i);
			}
		}
	}
	return true;//true表示在这里接收moved和ended事件
}

void ControllableGameScene::ejectorTouchEndedCallback(Touch* touch, Event* event)
{
	holdingTouching = false;
	if (moveByTouching && cooling) {
		cooling = false;
		unschedule("cooling");
		changeTurn();
	}
}

void ControllableGameScene::activateEjector(int col)
{
	moveByTouching = true;
	beginMoving(col, getNextChessman());
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