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
	bool firstMove = true;

	SIOClient* client;
	bool disconnected = false;
	bool shouldEndTurn = false;
	int room;
	EndGameReason endGameReason;
	float time = timeLimit;

	void startTimer();
	void stopTimer();
	void setTurnFlag();
	void buildChessboard();
	void beginMoving(int col);
	void endMoving();
	void changeTurn();
	void leftWins();
	void rightWins();

	void onConnect(SIOClient* client, const std::string &data);
	void onError(SIOClient* client, const std::string &data);
	void onClose(SIOClient* client) {};
	void onSendName(SIOClient* client, const std::string &data);
	void onStart(SIOClient* client, const std::string &data);
	void onNextChessman(SIOClient* client, const std::string &data);
	void onMove(SIOClient* client, const std::string &data);
	void onChangeTurn(SIOClient* client, const std::string &data);
	void onEndGame(SIOClient* client, const std::string &data);
	void onDisconnected(SIOClient* client, const std::string &data);
};
