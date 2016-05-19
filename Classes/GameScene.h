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
	GameState state = GameState::controlling;

	void setNames(std::string left, std::string right);
	virtual void setTurnFlag();
	virtual void buildChessboard();
	virtual void updateChessboard();
	Sprite* createSpriteByChessman(Chessman type);
	//�ⲿ����(�ⲿ���ǲ�����Ҳ������µ��ƶ�)
	void setMovingCol(int col);
	void move(Chessman chessman);
	void endTurn();
	//----
	virtual Chessman getNextChessman();
	virtual void beginMoving(int col, Chessman chessman);
	virtual void endMoving();
	virtual void setTurn(Side turn);
	virtual void changeTurn();
	virtual void leftWins();
	virtual void rightWins();
private:
	ClippingNode* stencil;//���нڵ�
	DrawNode* stencilDrawNode;//����ģ��ͼ��

	bool touching;
	bool moveByControl;//���·������������ƶ�
	int movingCol;//�ƶ��е���
	Chessman movingNewChessman;//�Ƴ�����������
	int totalMovements;


	bool externalColLock = false;//�ⲿ����,�������ⲿ�޷��ı��ƶ�����
	bool ejectorTouchBeganCallback(Touch* touch, Event* event);
	void ejectorTouchEndedCallback(Touch* touch, Event* event);
	bool setBoardSize(int lCol, int rCol);
	void flip();
};
