#include "PvoGameScene.h"
#include "Text.h"
#include "Player.h"
#include "ResultScene.h"
#include "MainScene.h"

bool PvoGameScene::init()
{
	//这两个要提早初始化，否则buildChessboard的时候会出错
	timerStencilDrawNode = DrawNode::create();
	timer = LayerColor::create();
	if (!ControllableGameScene::init())
		return false;
	setNames(Player::getName(), Text::get("opponent"));
	Size visibleSize = Director::getInstance()->getVisibleSize();
	roomLabel = Text::createLabel(Text::get("connecting"), 25, Color4B(0, 0, 0, 255));
	roomLabel->setPosition(visibleSize.width - roomLabel->getContentSize().width, roomLabel->getContentSize().height);
	this->addChild(roomLabel);

	timerStencil = ClippingNode::create(timerStencilDrawNode);
	timer->changeWidthAndHeight(drawLength, drawLength);
	timer->ignoreAnchorPointForPosition(false);
	timer->setAnchorPoint(Vec2(0, 1));
	timerStencil->addChild(timer);
	board->addChild(timerStencil);

	client = SocketIO::connect(Player::getServer(), *this);
	if (!client)
		return false;
	client->on("connect", CC_CALLBACK_2(PvoGameScene::onConnect, this));
	client->on("start", CC_CALLBACK_2(PvoGameScene::onStart, this));
	client->on("tellNewChessman", CC_CALLBACK_2(PvoGameScene::onTellNewChessman, this));
	client->on("beginMoving", CC_CALLBACK_2(PvoGameScene::onBeginMoving, this));
	client->on("changeTurn", CC_CALLBACK_2(PvoGameScene::onChangeTurn, this));
	client->on("endGame", CC_CALLBACK_2(PvoGameScene::onEndGame, this));
	client->on("disconnect", CC_CALLBACK_2(PvoGameScene::onDisconnected, this));
	return true;
}

PvoGameScene::~PvoGameScene()
{
	if (!disconnected) {
		client->disconnect();
	}
}

void PvoGameScene::startTimer()
{
	stopTimer();
	timer->setPosition(0, drawLength);
	auto moveAction = MoveBy::create(timeLimit, Vec2(0, -diagonal));
	timer->runAction(moveAction);
}

void PvoGameScene::stopTimer()
{
	timer->stopAllActions();
}

void PvoGameScene::setTurnFlag()
{
	ControllableGameScene::setTurnFlag();
	timer->initWithColor(turn == left ? Color4B(0, 255, 0, 100) : Color4B(0, 100, 255, 100));
}

void PvoGameScene::buildChessboard()
{
	ControllableGameScene::buildChessboard();
	timerStencilDrawNode->clear();
	Vec2 stencilPoly[] = { topVertex,topVertex - Vec2(halfDiagonal,halfDiagonal),topVertex - Vec2(0,diagonal),topVertex - Vec2(-halfDiagonal,halfDiagonal) };
	timerStencilDrawNode->drawSolidPoly(stencilPoly, 4, Color4F(1.0, 1.0, 1.0, 1.0));
}

void PvoGameScene::activateEjector(int col)
{
	if (turn == left && started) {
		ControllableGameScene::activateEjector(col);
	}
}

void PvoGameScene::beginMoving(int col, Chessman chessman)
{
	ControllableGameScene::beginMoving(col, chessman);
	stopTimer();
	if (turn == left) {
		if (holdingTouching && !disconnected) {
			Json j;
			j.add("col", col);
			client->emit("beginMoving", j.toString());
		}
	}
}

void PvoGameScene::endMoving()
{
	ControllableGameScene::endMoving();
	if (shouldChangeTurn) {
		changeTurn();
		shouldChangeTurn = false;
	}
}

void PvoGameScene::changeTurn()
{
	if (turn == left && !disconnected) {
		client->emit("changeTurn", "");
	}
	ControllableGameScene::changeTurn();
	time = timeLimit;

	startTimer();
}

Chessman PvoGameScene::getNextChessman()
{
	return nextChessman;
}

void PvoGameScene::leftWins()
{
	auto rs = ResultScene::create(Text::get("playerWins"), Color4B(0, 255, 0, 255));
	Director::getInstance()->replaceScene(rs);
}

void PvoGameScene::rightWins()
{
	auto rs = ResultScene::create(Text::get("onlinePlayerWins"), Color4B(0, 0, 0, 255));
	Director::getInstance()->replaceScene(rs);
}

void PvoGameScene::onConnect(SIOClient * client, const std::string & data)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	roomLabel->setString(Text::get("waiting"));
	roomLabel->setPosition(visibleSize.width - roomLabel->getContentSize().width, roomLabel->getContentSize().height);

}

void PvoGameScene::onError(SIOClient * client, const std::string & data)
{
	MessageBox(("Error! " + data).c_str(), "Myomyw");
	Director::getInstance()->replaceScene(MainScene::create());
}

void PvoGameScene::onStart(SIOClient * client, const std::string & data)
{
	started = true;
	Json j(data);
	if (j.getInt("side") == right) {
		changeTurn();
	}
	room = j.getInt("room");
	roomLabel->setString(Text::get("room") + std::to_string(room));
	Size visibleSize = Director::getInstance()->getVisibleSize();
	roomLabel->setPosition(visibleSize.width - roomLabel->getContentSize().width, roomLabel->getContentSize().height);

	timer->setPosition(0, drawLength - diagonal + time / timeLimit * diagonal);
	startTimer();
}

void PvoGameScene::onTellNewChessman(SIOClient * client, const std::string & data)
{
	Json j(data);
	nextChessman = (Chessman)j.getInt("chessman");
}

void PvoGameScene::onBeginMoving(SIOClient * client, const std::string & data)
{
	if (turn == right) {
		Json j(data);
		beginMoving(j.getInt("col"), (Chessman)j.getInt("chessman"));
	}
}

void PvoGameScene::onChangeTurn(SIOClient * client, const std::string & data)
{
	if (turn == right) {
		//如果还在移动就设置shouldChangeTurn为true，这样移动完成后就会切换回合
		if (moving) {
			shouldChangeTurn = true;
		}
		else {
			changeTurn();
		}
	}
}

void PvoGameScene::onEndGame(SIOClient * client, const std::string & data)
{
	Json j(data);
	endGameReason = (EndGameReason)j.getInt("reason");
	if (endGameReason == EndGameReason::opponentLeft) {
		Director::getInstance()->replaceScene(ResultScene::create(Text::get("opponentLeft"), Color4B(0, 0, 0, 255)));
	}
	else if (endGameReason == EndGameReason::timeOut) {
		if (turn == left) {
			Director::getInstance()->replaceScene(ResultScene::create(Text::get("playerTimeOut"), Color4B(0, 0, 0, 255)));
		}
		else {
			Director::getInstance()->replaceScene(ResultScene::create(Text::get("opponentTimeOut"), Color4B(0, 0, 0, 255)));
		}
	}
}

void PvoGameScene::onDisconnected(SIOClient * client, const std::string & data)
{
	if (endGameReason == EndGameReason::unknown) {
		Director::getInstance()->replaceScene(ResultScene::create(Text::get("unknownDisconnect"), Color4B(0, 0, 0, 255)));
	}
	disconnected = true;
}