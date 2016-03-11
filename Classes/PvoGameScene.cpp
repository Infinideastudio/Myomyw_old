#include "PvoGameScene.h"
#include "Text.h"
#include "ResultScene.h"
#include "MainScene.h"

bool PvoGameScene::init(std::string address)
{
	if (!ControllableGameScene::init())
		return false;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	roomLabel = Label::createWithTTF(Text::get("connecting"), "fonts/Deng.ttf", 25);
	roomLabel->setTextColor(Color4B(0, 0, 0, 255));
	roomLabel->setPosition(visibleSize.width - roomLabel->getContentSize().width, visibleSize.height - roomLabel->getContentSize().height);
	this->addChild(roomLabel);

	client = SocketIO::connect(address, *this);
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

PvoGameScene* PvoGameScene::create(std::string address)
{
	PvoGameScene* pRet = new(std::nothrow) PvoGameScene();
	if (pRet && pRet->init(address))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

PvoGameScene::~PvoGameScene()
{
	if (!disconnected) {
		client->disconnect();
	}
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
}

Chessman PvoGameScene::getNextChessman()
{
	return nextChessman;
}

void PvoGameScene::leftWins()
{
	if (endGameReason == EndGameReason::youWin) {
		auto rs = ResultScene::create(Text::get("playerWins"), Color4B(0, 255, 0, 255));
		Director::getInstance()->replaceScene(rs);
	}
}

void PvoGameScene::rightWins()
{
	if (endGameReason == EndGameReason::youLose) {
		auto rs = ResultScene::create(Text::get("onlinePlayerWins"), Color4B(0, 0, 0, 255));
		Director::getInstance()->replaceScene(rs);
	}
}

void PvoGameScene::onConnect(SIOClient * client, const std::string & data)
{
	roomLabel->setString(Text::get("waiting"));
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
}

void PvoGameScene::onDisconnected(SIOClient * client, const std::string & data)
{
	if (endGameReason == EndGameReason::unknown) {
		Director::getInstance()->replaceScene(ResultScene::create(Text::get("unknownDisconnect"), Color4B(0, 0, 0, 255)));
	}
	disconnected = true;
}