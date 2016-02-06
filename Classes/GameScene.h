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
	int lCol = defaultLCol, rCol = defaultRCol;//棋盘大小
	Side turn = left;//当前回合
	Chessman chessmen[maxLCol][maxRCol];//棋盘
	bool moving = false;//是否移动中
	int movingCol;//移动中的列
	Chessman movingNewChessman;//移出来的新棋子
	float drawSize;
	float halfDiagonal;
	float diagonal;
	Vec2 topVertex;
	Vec2 leftVertex;
	Vec2 rightVertex;
	Vec2 bottomVertex;
	ClippingNode* stencil;//裁切节点
	DrawNode* stencilDrawNode;//裁切模板图形
	Node* chessmanNode;//放棋子的节点(也就是裁切底板)
	DrawNode* gridDrawNode;//网格绘制节点
	Node* ejectorNode;//放发射器的节点

	bool setBoardSize(int lCol, int rCol);
	virtual void buildChessboard();
	virtual void updateChessboard();
	Sprite* createSpriteByChessman(Chessman type);
	virtual void beginMoving(int col, Chessman chessman);
	virtual void endMoving();
	virtual void changeTurn();
	static Chessman getRandomChessman();
private:

};