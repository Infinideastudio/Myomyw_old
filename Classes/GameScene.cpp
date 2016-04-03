#include "GameScene.h"
#include "Text.h"
#include "ResultScene.h"
#include "MainScene.h"
USING_NS_CC;

bool GameScene::init()
{
	if (!Scene::init())
		return false;
	//--初始化棋子数组--//
	for (int i = 0; i < maxLCol; i++) {
		for (int j = 0; j < maxRCol; j++) {
			chessmen[i][j] = Chessman::common;
		}
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	drawLength = MIN(visibleSize.width, visibleSize.height) - 10;
	//--返回按钮--//
	auto backItem = MenuItemImage::create("UI/BackNormal.png", "UI/BackSelected.png", [](Ref* pSender)
	{Director::getInstance()->replaceScene(MainScene::create()); });
	backItem->setPosition(Vec2(backItem->getContentSize() / 2 + Size(10, 10)));
	auto menu = Menu::create(backItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);
	//--棋盘层--//
	board = Layer::create();
	board->setPosition((visibleSize.width - drawLength) / 2, (visibleSize.height - drawLength) / 2);
	this->addChild(board);
	//--网格绘制节点--//
	gridDrawNode = DrawNode::create();
	board->addChild(gridDrawNode, 1);
	//--遮罩及棋子节点--//
	stencilDrawNode = DrawNode::create();
	stencil = ClippingNode::create(stencilDrawNode);
	chessmanNode = Node::create();
	stencil->addChild(chessmanNode);
	board->addChild(stencil, 2);
	//--发射器节点--//
	ejectorNode = Node::create();
	board->addChild(ejectorNode, 0);
	//--双方名称--//
	leftNameLabel = Label::createWithTTF("", "fonts/Deng.ttf", 26);
	leftNameLabel->setTextColor(Color4B(0, 0, 0, 255));
	this->addChild(leftNameLabel);
	rightNameLabel = Label::createWithTTF("", "fonts/Deng.ttf", 26);
	rightNameLabel->setTextColor(Color4B(0, 0, 0, 255));
	this->addChild(rightNameLabel);
	buildChessboard();
	return true;
}

void GameScene::setNames(std::string left, std::string right)
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	leftNameLabel->setString(left);
	leftNameLabel->setPosition(Vec2(leftNameLabel->getContentSize().width, visibleSize.height - leftNameLabel->getContentSize().height));
	rightNameLabel->setString(right);
	rightNameLabel->setPosition(Vec2(visibleSize.width - rightNameLabel->getContentSize().width, visibleSize.height - rightNameLabel->getContentSize().height));
}

bool GameScene::setBoardSize(int lCol, int rCol)
{
	if (lCol > maxLCol || lCol<minLCol || rCol>maxRCol || rCol < minRCol)
		return false;
	//如果棋盘变大，把多出来的那部分清空
	if (this->lCol < lCol) {
		for (int i = this->lCol; i < lCol; i++) {
			for (int j = 0; j < rCol; j++) {
				chessmen[i][j] = Chessman::common;
			}
		}
	}
	if (this->rCol < rCol) {
		for (int i = 0; i < lCol; i++) {
			for (int j = this->rCol; j < rCol; j++) {
				chessmen[i][j] = Chessman::common;
			}
		}
	}

	this->lCol = lCol;
	this->rCol = rCol;
	buildChessboard();
	return true;
}

//设置回合标志
void GameScene::setTurnFlag()
{
	int leftOpacity = (turn == left ? 255 : 150);
	int rightOpacity = (turn == right ? 255 : 150);
	for (int i = 0; i < lCol; i++) {
		ejectorNode->getChildByTag(i)->setOpacity(leftOpacity);
	}
	for (int i = lCol; i < lCol + rCol; i++) {
		ejectorNode->getChildByTag(i)->setOpacity(rightOpacity);
	}
	leftNameLabel->setTextColor(turn == left ? Color4B(0, 200, 0, 255) : Color4B(0, 0, 0, 255));
	rightNameLabel->setTextColor(turn == right ? Color4B(0, 0, 200, 255) : Color4B(0, 0, 0, 255));

	auto scaleAction = ScaleTo::create(0.2f, 1.2f);
	(turn == left ? leftNameLabel : rightNameLabel)->runAction(scaleAction);
	auto resetAction = ScaleTo::create(0.2f, 1.0f);
	(turn == left ? rightNameLabel : leftNameLabel)->runAction(resetAction);
}

//建立棋盘，在棋盘大小改变后调用
void GameScene::buildChessboard()
{
	int drawLCol = lCol + 1, drawRCol = rCol + 1;//画图时的网格数量(加上发射器)
	auto visibleSize = this->getContentSize();
	halfDiagonal = drawLength / (float)(drawLCol + drawRCol);
	diagonal = 2 * halfDiagonal;
	topVertex = Vec2(drawLCol*halfDiagonal, drawLength);
	rightVertex = Vec2(drawLength, topVertex.x);
	leftVertex = Vec2(0, drawLength - rightVertex.y);
	bottomVertex = Vec2(leftVertex.y, 0);
	//---建立模板遮罩---//
	stencilDrawNode->clear();
	//遮罩不包括发射器部分
	//扩大一像素是为了不遮住线
	Vec2 stencilPoly[] = { topVertex + Vec2(0, 1 - diagonal), leftVertex + Vec2(-1 + halfDiagonal, -halfDiagonal), bottomVertex + Vec2(0, -1) , rightVertex + Vec2(1 - halfDiagonal, -halfDiagonal) };
	stencilDrawNode->drawSolidPoly(stencilPoly, 4, Color4F(1.0, 1.0, 1.0, 1.0));
	//---绘制网格---//
	gridDrawNode->clear();
	for (int i = 0; i <= drawLCol; i++)
	{
		gridDrawNode->drawLine(Vec2(topVertex.x - halfDiagonal*i, topVertex.y - halfDiagonal*i),
			Vec2(topVertex.x - halfDiagonal*(i - drawRCol), topVertex.y - halfDiagonal*(i + drawRCol)),
			Color4F(0.0, 0.0, 0.0, 1.0));
	}
	for (int i = 0; i <= drawRCol; i++)
	{
		gridDrawNode->drawLine(Vec2(topVertex.x + halfDiagonal*i, topVertex.y - halfDiagonal*i),
			Vec2(topVertex.x + halfDiagonal*(i - drawLCol), topVertex.y - halfDiagonal*(i + drawLCol)),
			Color4F(0.0, 0.0, 0.0, 1.0));
	}
	//--装备发射器--//
	ejectorNode->removeAllChildren();
	for (int i = 0; i < drawLCol - 1; i++)
	{
		auto ejector = Sprite::create("Ejector/Green.png");
		ejector->setPosition(Vec2(topVertex.x - (i + 1)*halfDiagonal, topVertex.y - (2 + i)*halfDiagonal));
		ejector->setScaleX(diagonal / ejector->getContentSize().width);
		ejector->setScaleY(diagonal / ejector->getContentSize().height);
		ejector->setTag(i);
		ejectorNode->addChild(ejector);
	}
	for (int i = 0; i < drawRCol - 1; i++)
	{
		auto ejector = Sprite::create("Ejector/Blue.png");
		ejector->setPosition(Vec2(topVertex.x + (i + 1)*halfDiagonal, topVertex.y - (2 + i)*halfDiagonal));
		ejector->setScaleX(diagonal / ejector->getContentSize().width);
		ejector->setScaleY(diagonal / ejector->getContentSize().height);
		ejector->setTag(lCol + i);
		ejectorNode->addChild(ejector);
	}
	setTurnFlag();
	updateChessboard();
}

//更新棋盘，在棋子变化后调用(棋盘大小改变也会调用)
void GameScene::updateChessboard()
{
	chessmanNode->removeAllChildren();
	for (int i = 0; i < lCol; i++) {
		for (int j = 0; j < rCol; j++) {
			auto chessman = createSpriteByChessman(chessmen[i][j]);
			chessman->setPosition(((j - i)*halfDiagonal + topVertex.x), drawLength - (i + j + 3)*halfDiagonal);
			chessman->setTag(i*rCol + j);
			chessmanNode->addChild(chessman);
		}
	}
}

Sprite * GameScene::createSpriteByChessman(Chessman type)
{
	std::string filename;
	switch (type) {
	case Chessman::common:
		filename = "Chessman/Common.png";
		break;
	case Chessman::key:
		filename = "Chessman/Key.png";
		break;
	case Chessman::addCol:
		filename = "Chessman/AddCol.png";
		break;
	case Chessman::delCol:
		filename = "Chessman/DelCol.png";
		break;
	}
	auto chessman = Sprite::create(filename);
	chessman->setScaleX(halfDiagonal / chessman->getContentSize().width);
	chessman->setScaleY(halfDiagonal / chessman->getContentSize().height);
	return chessman;
}

void GameScene::beginMoving(int col, Chessman chessman)
{
	if (!moving) {
		moving = true;
		movingCol = col;
		movingNewChessman = chessman;
		auto newChessman = createSpriteByChessman(movingNewChessman);
		newChessman->setPosition(((turn == left ? -movingCol - 1 : movingCol + 1) * halfDiagonal + topVertex.x), drawLength - (movingCol + 2)*halfDiagonal);
		chessmanNode->addChild(newChessman);
		auto movingAction = MoveBy::create(movingTime, Vec2(turn == left ? halfDiagonal : -halfDiagonal, -halfDiagonal));
		for (int j = 0; j < (turn == left ? rCol : lCol); j++) {
			chessmanNode->getChildByTag(turn == left ? movingCol*rCol + j : j*rCol + movingCol)->runAction(movingAction->clone());
		}
		newChessman->runAction(movingAction->clone());
		scheduleOnce(CC_CALLBACK_0(GameScene::endMoving, this), movingTime, "move");
	}
}

void GameScene::endMoving()
{
	moving = false;
	Chessman lastChessman;//暂存最底下的棋子
	if (turn == left) {
		lastChessman = chessmen[movingCol][rCol - 1];
		for (int i = rCol - 1; i > 0; i--) {
			chessmen[movingCol][i] = chessmen[movingCol][i - 1];
		}
		chessmen[movingCol][0] = movingNewChessman;
		switch (lastChessman) {
		case Chessman::key:
			rightWins();
			break;
		case Chessman::addCol:
			setBoardSize(lCol, rCol + 1);
			break;
		case Chessman::delCol:
			setBoardSize(lCol, rCol - 1);
			break;
		}
	}
	else {
		lastChessman = chessmen[lCol - 1][movingCol];
		for (int i = lCol - 1; i > 0; i--) {
			chessmen[i][movingCol] = chessmen[i - 1][movingCol];
		}
		chessmen[0][movingCol] = movingNewChessman;
		switch (lastChessman) {
		case Chessman::key:
			leftWins();
			break;
		case Chessman::addCol:
			setBoardSize(lCol + 1, rCol);
			break;
		case Chessman::delCol:
			setBoardSize(lCol - 1, rCol);
			break;
		}
	}
	updateChessboard();
}

void GameScene::changeTurn()
{
	turn = (turn == left ? right : left);
	setTurnFlag();
}

void GameScene::leftWins()
{
	auto rs = ResultScene::create(Text::get("greenWins"), Color4B(0, 255, 0, 255));
	Director::getInstance()->replaceScene(rs);
}

void GameScene::rightWins()
{
	auto rs = ResultScene::create(Text::get("blueWins"), Color4B(0, 0, 255, 255));
	Director::getInstance()->replaceScene(rs);
}
