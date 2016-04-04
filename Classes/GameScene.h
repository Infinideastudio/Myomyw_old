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

	void setNames(std::string left, std::string right);
	bool setBoardSize(int lCol, int rCol);
	virtual void setTurnFlag();
	virtual void buildChessboard();
	virtual void updateChessboard();
	Sprite* createSpriteByChessman(Chessman type);
	Sprite* createSpriteByGridPicture();
	virtual void beginMoving(int col, Chessman chessman);
	virtual void endMoving();
	virtual void changeTurn();
	virtual void leftWins();
	virtual void rightWins();
private:
	ClippingNode* stencil;//���нڵ�
	DrawNode* stencilDrawNode;//����ģ��ͼ��
};