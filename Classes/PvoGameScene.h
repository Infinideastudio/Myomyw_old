#pragma once
#include "GameScene.h"
#include "cocos2d.h"
#include "network\SocketIO.h"
#include "Json.h"

USING_NS_CC;
using namespace cocos2d::network;

class PvoGameScene :public GameScene, public SocketIO::SIODelegate
{
public:
	virtual bool init();
	CREATE_FUNC(PvoGameScene);
	~PvoGameScene();
private:
	Label* roomLabel;
	ClippingNode* timerStencil;
	DrawNode* timerStencilDrawNode;
	LayerColor* timer;
	bool firstMessage = true;

	SIOClient* client;
	bool disconnected = false;
	bool shouldEndTurn = false;
	bool started = false;
	std::vector<Chessman> movementBuffer;//传入移动时的缓冲
	Chessman nextChessman;//服务器提供的下一个新球
	int room;
	EndGameReason endGameReason;
	float time = timeLimit;

	void startTimer();
	void stopTimer();
	void setTurnFlag();
	void buildChessboard();
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