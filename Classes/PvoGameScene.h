#pragma once
#include "ControllableGameScene.h"
#include "cocos2d.h"
#include "network\SocketIO.h"
#include "Json.h"

USING_NS_CC;
using namespace cocos2d::network;

class PvoGameScene :public ControllableGameScene, public SocketIO::SIODelegate
{
public:
	virtual bool init(std::string address);
	static PvoGameScene* create(std::string address);
	~PvoGameScene();
private:
	Label* roomLabel;
	ClippingNode* timerStencil;
	DrawNode* timerStencilDrawNode;
	LayerColor* timer;

	SIOClient* client;
	bool disconnected = false;
	bool shouldChangeTurn = false;
	bool started = false;
	Chessman nextChessman;//服务器提供的下一个新球
	int room;
	EndGameReason endGameReason;
	float time = timeLimit;

	void startTimer();
	void stopTimer();
	void setTurnFlag();
	void buildChessboard();
	void activateEjector(int col);
	void beginMoving(int col, Chessman chessman);
	void endMoving();
	void changeTurn();
	Chessman getNextChessman();
	void leftWins();
	void rightWins();

	void onConnect(SIOClient* client, const std::string &data);
	void onError(SIOClient* client, const std::string &data);
	void onClose(SIOClient* client) {};
	void onStart(SIOClient* client, const std::string &data);
	void onTellNewChessman(SIOClient* client, const std::string &data);
	void onBeginMoving(SIOClient* client, const std::string &data);
	void onChangeTurn(SIOClient* client, const std::string &data);
	void onEndGame(SIOClient* client, const std::string &data);
	void onDisconnected(SIOClient* client, const std::string &data);
};