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
	int lCol = defaultLCol, rCol = defaultRCol;//���̴�С
	Side turn = left;//��ǰ�غ�
	Chessman chessmen[maxLCol][maxRCol];//����
	bool moving = false;//�Ƿ��ƶ���
	int movingCol;//�ƶ��е���
	Chessman movingNewChessman;//�Ƴ�����������
	float drawSize;
	float halfDiagonal;
	float diagonal;
	Vec2 topVertex;
	Vec2 leftVertex;
	Vec2 rightVertex;
	Vec2 bottomVertex;
	ClippingNode* stencil;//���нڵ�
	DrawNode* stencilDrawNode;//����ģ��ͼ��
	Node* chessmanNode;//�����ӵĽڵ�(Ҳ���ǲ��еװ�)
	DrawNode* gridDrawNode;//������ƽڵ�
	Node* ejectorNode;//�ŷ������Ľڵ�

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