#include "PvoGameScene.h"
#include "Lang.h"
#include "MyCreator.h"
#include "Player.h"
#include "ResultScene.h"
#include "MainScene.h"

bool PvoGameScene::init()
{
	//这两个要提早初始化，否则buildChessboard的时候会出错
	timerStencilDrawNode = DrawNode::create();
	timer = LayerColor::create();
	if (!GameScene::init())
		return false;
	setNames(Player::getName(), Lang::get("opponent"));
	Size visibleSize = Director::getInstance()->getVisibleSize();
	roomLabel = MyCreator::createLabel(Lang::get("connecting"), 25, Color4B(0, 0, 0, 255));
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
	client->on("nextChessman", CC_CALLBACK_2(PvoGameScene::onNextChessman, this));
	client->on("move", CC_CALLBACK_2(PvoGameScene::onMove, this));
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
	GameScene::setTurnFlag();
	timer->initWithColor(turn == left ? Color4B(0, 255, 0, 100) : Color4B(0, 100, 255, 100));
}

void PvoGameScene::buildChessboard()
{
	GameScene::buildChessboard();
	timerStencilDrawNode->clear();
	Vec2 stencilPoly[] = { topVertex,topVertex - Vec2(halfDiagonal,halfDiagonal),topVertex - Vec2(0,diagonal),topVertex - Vec2(-halfDiagonal,halfDiagonal) };
	timerStencilDrawNode->drawSolidPoly(stencilPoly, 4, Color4F(1.0, 1.0, 1.0, 1.0));
}

void PvoGameScene::beginMoving(int col)
{
	GameScene::beginMoving(col);
	stopTimer();
	if (turn == left && !disconnected) {
		if (firstMove) {
			Json j;
			j.add("col", col);
			client->emit("move", j.toString());
			firstMove = false;
		}
		else {
			client->emit("move", "");
		}

	}
}

void PvoGameScene::endMoving()
{
	GameScene::endMoving();
	if (shouldEndTurn) {
		changeTurnAndSetTurnFlag();
		shouldEndTurn = false;
	}
}

void PvoGameScene::changeTurn()
{
	if (turn == left && !disconnected) {
		client->emit("endTurn", "");
	}
	GameScene::changeTurn();
	if (turn == left) {
		controllable = true;
	}
	else {
		controllable = false;
	}
	time = timeLimit;
	startTimer();
}

void PvoGameScene::leftWins()
{
	auto rs = ResultScene::create(Lang::get("playerWins"), Color4B(0, 255, 0, 255));
	Director::getInstance()->replaceScene(rs);
}

void PvoGameScene::rightWins()
{
	auto rs = ResultScene::create(Lang::get("onlinePlayerWins"), Color4B(0, 0, 0, 255));
	Director::getInstance()->replaceScene(rs);
}

void PvoGameScene::onConnect(SIOClient * client, const std::string & data)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	roomLabel->setString(Lang::get("waiting"));
	roomLabel->setPosition(visibleSize.width - roomLabel->getContentSize().width, roomLabel->getContentSize().height);
}

void PvoGameScene::onError(SIOClient * client, const std::string & data)
{
	MessageBox(("Error! " + data).c_str(), "Myomyw");
	Director::getInstance()->replaceScene(MainScene::create());
}

void PvoGameScene::onStart(SIOClient * client, const std::string & data)
{
	Json j(data);
	if (j.getInt("side") == right) {
		setTurn(right);
		setTurnFlag();
		controllable = false;
	}
	else {
		controllable = true;
	}
	room = j.getInt("room");
	roomLabel->setString(Lang::get("room") + std::to_string(room));
	Size visibleSize = Director::getInstance()->getVisibleSize();
	roomLabel->setPosition(visibleSize.width - roomLabel->getContentSize().width, roomLabel->getContentSize().height);

	timer->setPosition(0, drawLength - diagonal + time / timeLimit * diagonal);
	startTimer();
}

void PvoGameScene::onNextChessman(SIOClient * client, const std::string & data)
{
	Json j(data);
	setNextChessman((Chessman)j.getInt("chessman"));
}

void PvoGameScene::onMove(SIOClient * client, const std::string & data)
{
	if (turn == right) {
		Json j(data);
		if (firstMessage) {
			movingCol = j.getInt("col");
			firstMessage = false;
		}
		if (state != ActionState::moving) {
			beginMoving(movingCol);
		}
	}
}

void PvoGameScene::onChangeTurn(SIOClient * client, const std::string & data)
{
	if (turn == right) {
		firstMessage = true;
		firstMove = true;
		//如果还在移动就设置shouldEndTurn为true，这样移动完成后就会切换回合
		if (state == ActionState::moving) {
			shouldEndTurn = true;
		}
		else {
			changeTurnAndSetTurnFlag();
		}
	}
}

void PvoGameScene::onEndGame(SIOClient * client, const std::string & data)
{
	Json j(data);
	endGameReason = (EndGameReason)j.getInt("reason");
	if (endGameReason == EndGameReason::opponentLeft) {
		Director::getInstance()->replaceScene(ResultScene::create(Lang::get("opponentLeft"), Color4B(0, 0, 0, 255)));
	}
	else if (endGameReason == EndGameReason::timeOut) {
		if (turn == left) {
			Director::getInstance()->replaceScene(ResultScene::create(Lang::get("playerTimeOut"), Color4B(0, 0, 0, 255)));
		}
		else {
			Director::getInstance()->replaceScene(ResultScene::create(Lang::get("opponentTimeOut"), Color4B(0, 0, 0, 255)));
		}
	}
}

void PvoGameScene::onDisconnected(SIOClient * client, const std::string & data)
{
	if (endGameReason == EndGameReason::unknown) {
		Director::getInstance()->replaceScene(ResultScene::create(Lang::get("unknownDisconnect"), Color4B(0, 0, 0, 255)));
	}
	disconnected = true;
}
