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
	Node* gridNode;//����ڵ�
	Node* chessmanNode;//���ӽڵ�(Ҳ���ǲ��еװ�)
	Label* leftNameLabel;
	Label* rightNameLabel;

	int lCol = defaultLCol, rCol = defaultRCol;//���̴�С
	Side turn = left;//��ǰ�غ�
	Chessman chessmen[maxLCol][maxRCol];//����
	ActionState state;
	int movingCol;//�ƶ��е���
	bool controllable = false;
	Chessman lastChessman;//�ݴ�����µ�����

	void setNames(std::string left, std::string right);
	Chessman getNextChessman() { return nextChessman; };
	void setNextChessman(Chessman chessman);
	virtual void setTurnFlag();
	virtual void buildChessboard();
	virtual void updateChessboard();
	virtual Sprite* createSpriteByChessman(Chessman type);
	Chessman getRandomchessman();
	virtual void beginMoving(int col);
	virtual void endMoving();
	virtual void setTurn(Side turn);
	virtual void changeTurn();
	void changeTurnAndSetTurnFlag();
	virtual void leftWins();
	virtual void rightWins();
private:
	ClippingNode* stencil;//���нڵ�
	DrawNode* stencilDrawNode;//����ģ��ͼ��

	bool touching = false;
	Chessman movingNewChessman;//�Ƴ�����������
	Chessman nextChessman;//��һ������
	int totalMovements;

	bool ejectorTouchBeganCallback(Touch* touch, Event* event);
	void ejectorTouchEndedCallback(Touch* touch, Event* event);
	bool setBoardSize(int lCol, int rCol);
	void flip();
};
