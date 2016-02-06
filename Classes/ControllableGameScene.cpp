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
	if (!moving) {
		for (int i = 0; i < (turn == left ? lCol : rCol); i++) {
			Vec2 point = touch->getLocation() - ejectorNode->getChildByTag(turn == left ? i : lCol + i)->getPosition();
			if (point.x + point.y<halfDiagonal && point.x + point.y>-halfDiagonal && point.x - point.y>-halfDiagonal && point.y - point.x > -halfDiagonal) {
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
}

void ControllableGameScene::activateEjector(int col)
{
	if (!moving) {
		moveByTouching = true;
		beginMoving(col, getRandomChessman());
	}
}

void ControllableGameScene::beginMoving(int col, Chessman chessman)
{
	if (moveByTouching) {
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
		if (totalMovementTimes >= maxMovementTimes) {
			holdingTouching = false;
		}
		auto delayAction = DelayTime::create(movingCooling);
		auto callingAction = CallFunc::create(CC_CALLBACK_0(ControllableGameScene::beginMoving, this, movingCol, getRandomChessman()));
		auto delayAndCallingAction = Sequence::create(delayAction, callingAction, NULL);
		this->runAction(delayAndCallingAction);
	}
}

void ControllableGameScene::changeTurn()
{
	GameScene::changeTurn();
	totalMovementTimes = 0;
	moveByTouching = false;
	holdingTouching = false;
}