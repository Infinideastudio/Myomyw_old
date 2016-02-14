#include "PvoGameScene.h"
#include "XmlData.h"
#include "ResultScene.h"
#include "MainScene.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

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
	client->on("disconnect", [](SIOClient* client, const std::string &data) {client->release(); });

	return true;
}

PvoGameScene * PvoGameScene::create(std::string address)
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
	client->disconnect();
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
		if (holdingTouching) {
			rapidjson::Document d;
			d.SetObject();
			rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
			d.AddMember("col", col, allocator);
			d.AddMember("chessman", (int)nextNewChessman, allocator);
			rapidjson::StringBuffer buffer;
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			d.Accept(writer);
			auto a = buffer.GetString();
			client->emit("beginMoving", buffer.GetString());
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
	if (turn == left) {
		client->emit("changeTurn", "");
	}
	ControllableGameScene::changeTurn();
}

void PvoGameScene::leftWins()
{
	auto rs = ResultScene::create(XmlData::text["player wins"], Color4B(0, 255, 0, 255));
	Director::getInstance()->replaceScene(rs);
}

void PvoGameScene::rightWins()
{
	auto rs = ResultScene::create(XmlData::text["online player wins"], Color4B(0, 0, 0, 255));
	Director::getInstance()->replaceScene(rs);
}

void PvoGameScene::onError(SIOClient * client, const std::string & data)
{
	MessageBox(("onError!" + data).c_str(), "Myomyw");
	Director::getInstance()->replaceScene(MainScene::create());
}

void PvoGameScene::onStart(SIOClient * client, const std::string & data)
{
	started = true;
	turn = std::stoi(data) == 0 ? left : right;
}

void PvoGameScene::onTellNewChessman(SIOClient * client, const std::string & data)
{
	nextNewChessman = (Chessman)stoi(data);
}

void PvoGameScene::onBeginMoving(SIOClient * client, const std::string & data)
{
	if (turn == right) {
		rapidjson::Document d;
		d.Parse(data.c_str());
		beginMoving(d["col"].GetInt(), (Chessman)d["chessman"].GetInt());
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
