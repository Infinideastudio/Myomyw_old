#include "PvoGameScene.h"
#include "XmlData.h"
#include "ResultScene.h"
#include "MainScene.h"

bool PvoGameScene::init(std::string address)
{
	if (!ControllableGameScene::init())
		return false;

	client = SocketIO::connect(address, *this);
	if (!client)
		return false;
	client->on("connect", [](SIOClient* client, const std::string &data) {});
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
	if (turn == left) {
		ControllableGameScene::beginMoving(col, nextNewChessman);
		if (holdingTouching && !disconnected) {
			Json j;
			j.add("col", col);
			client->emit("beginMoving", j.toString());
		}
	}
	else {
		ControllableGameScene::beginMoving(col, chessman);
	}
}

void PvoGameScene::endMoving()
{
	ControllableGameScene::endMoving();
	if (shouldStopMoving) {
		changeTurn();
		shouldStopMoving = false;
	}
}

void PvoGameScene::changeTurn()
{
	if (turn == left && !disconnected) {
		client->emit("changeTurn", "");
	}
	ControllableGameScene::changeTurn();
}

void PvoGameScene::leftWins()
{
	if (endGameReason == youWin) {
		auto rs = ResultScene::create(XmlData::text["player wins"], Color4B(0, 255, 0, 255));
		Director::getInstance()->replaceScene(rs);
	}
}

void PvoGameScene::rightWins()
{
	if (endGameReason == youLose) {
		auto rs = ResultScene::create(XmlData::text["online player wins"], Color4B(0, 0, 0, 255));
		Director::getInstance()->replaceScene(rs);
	}
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
}

void PvoGameScene::onTellNewChessman(SIOClient * client, const std::string & data)
{
	Json j(data);
	nextNewChessman = (Chessman)j.getInt("chessman");
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
		if (moving) {
			shouldStopMoving = true;
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
	if (endGameReason == opponentLeft) {
		Director::getInstance()->replaceScene(ResultScene::create(XmlData::text["opponent left"], Color4B(0, 0, 0, 255)));
	}
}

void PvoGameScene::onDisconnected(SIOClient * client, const std::string & data)
{
	if (endGameReason == unknown) {
		Director::getInstance()->replaceScene(ResultScene::create(XmlData::text["unknown disconnect"], Color4B(0, 0, 0, 255)));
	}
	disconnected = true;
}