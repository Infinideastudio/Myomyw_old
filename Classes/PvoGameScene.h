#pragma once
#include "ControllableGameScene.h"
#include "cocos2d.h"
#include "network\SocketIO.h"
#include "Json.h"

using namespace cocos2d::network;

class PvoGameScene :public ControllableGameScene, public SocketIO::SIODelegate
{
public:
	virtual bool init(std::string address);
	static PvoGameScene* create(std::string address);
	~PvoGameScene();
private:
	cocos2d::network::SIOClient* client;
	bool disconnected = false;
	bool shouldStopMoving = false;
	bool started = false;
	Chessman nextNewChessman;//服务器提供的下一个新球
	int room;
	EndGameReason endGameReason;
	void activateEjector(int col);
	void beginMoving(int col, Chessman chessman);
	void endMoving();
	void changeTurn();
	void leftWins();
	void rightWins();
	void onError(SIOClient* client, const std::string &data);
	void onClose(SIOClient* client) {};
	void onStart(SIOClient* client, const std::string &data);
	void onTellNewChessman(SIOClient* client, const std::string &data);
	void onBeginMoving(SIOClient* client, const std::string &data);
	void onChangeTurn(SIOClient* client, const std::string &data);
	void onEndGame(SIOClient* client, const std::string &data);
	void onDisconnected(SIOClient* client, const std::string &data);
};