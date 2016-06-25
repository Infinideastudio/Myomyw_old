#pragma once
#include "cocos2d.h"
#include "GameDefinitions.h"
USING_NS_CC;
class GameScene :public cocos2d::Scene
{
public:
	virtual bool init();
	CREATE_FUNC(GameScene);
protected:
	float drawLength;
	float halfDiagonal;
	float diagonal;
	Vec2 topVertex;
	Vec2 leftVertex;
	Vec2 rightVertex;
	Vec2 bottomVertex;
	Layer* board;
	Sprite* leftEjectors[maxLCol], *rightEjectors[maxRCol];
	DrawNode* border;
	Node* gridNode;//网格节点
	Node* chessmanNode;//棋子节点(也就是裁切底板)
	Label* leftNameLabel;
	Label* rightNameLabel;

	int lCol = defaultLCol, rCol = defaultRCol;//棋盘大小
	Side turn = left;//当前回合
	Chessman chessmen[maxLCol][maxRCol];//棋盘
	ActionState state;
	int movingCol;//移动中的列
	bool controllable = false;

	void setNames(std::string left, std::string right);
	virtual void setTurnFlag();
	virtual void buildChessboard();
	virtual void updateChessboard();
	Sprite* createSpriteByChessman(Chessman type);
	virtual Chessman getNextChessman();
	virtual void beginMoving(int col, Chessman chessman);
	virtual void endMoving();
	virtual void setTurn(Side turn);
	virtual void changeTurn();
	virtual void leftWins();
	virtual void rightWins();
private:
	ClippingNode* stencil;//裁切节点
	DrawNode* stencilDrawNode;//裁切模板图形

	bool touching = false;
	Chessman movingNewChessman;//移出来的新棋子
    Chessman nextChessman;//下一个棋子
	int totalMovements;

	bool ejectorTouchBeganCallback(Touch* touch, Event* event);
	void ejectorTouchEndedCallback(Touch* touch, Event* event);
	bool setBoardSize(int lCol, int rCol);
	void flip();
};
