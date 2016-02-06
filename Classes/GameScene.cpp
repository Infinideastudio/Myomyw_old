#include "GameScene.h"
#include "XmlData.h"
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
			chessmen[i][j] = common;
		}
	}
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//--返回按钮--//
	auto backItem = MenuItemImage::create("BackNormal.png", "BackSelected.png", [](Ref* pSender)
	{Director::getInstance()->replaceScene(MainScene::create()); });
	backItem->setPosition(Vec2(backItem->getContentSize().width / 2 + 10, visibleSize.height - backItem->getContentSize().height / 2 - 10));
	auto menu = Menu::create(backItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);

	//--网格绘制节点--//
	gridDrawNode = DrawNode::create();
	this->addChild(gridDrawNode, 1);
	//--遮罩及棋子节点--//
	stencilDrawNode = DrawNode::create();
	stencil = ClippingNode::create(stencilDrawNode);
	chessmanNode = Node::create();
	stencil->addChild(chessmanNode);
	this->addChild(stencil, 2);

	//--发射器节点--//
	ejectorNode = Node::create();
	this->addChild(ejectorNode, 0);

	buildChessboard();
	return true;
}

bool GameScene::setBoardSize(int lCol, int rCol)
{
	if (lCol > maxLCol || lCol<minLCol || rCol>maxRCol || rCol < minRCol)
		return false;
	if (this->lCol < lCol) {
		for (int i = this->lCol; i < lCol; i++) {
			for (int j = 0; j < rCol; j++) {
				chessmen[i][j] = common;
			}
		}
	}
	if (this->rCol < rCol) {
		for (int i = 0; i < lCol; i++) {
			for (int j = this->rCol; j < rCol; j++) {
				chessmen[i][j] = common;
			}
		}
	}
	this->lCol = lCol;
	this->rCol = rCol;
	buildChessboard();
	return true;
}

void GameScene::buildChessboard()
{
	//画图时的网格数量(加上发射器)
	int drawLCol = lCol + 1, drawRCol = rCol + 1;
	auto visibleSize = this->getContentSize();
	drawSize = MIN(visibleSize.width, visibleSize.height);
	halfDiagonal = drawSize / (float)(drawLCol + drawRCol);
	diagonal = 2 * halfDiagonal;
	topVertex = Vec2(drawLCol*halfDiagonal, drawSize);
	rightVertex = Vec2(drawSize, topVertex.x);
	leftVertex = Vec2(0, drawSize - rightVertex.y);
	bottomVertex = Vec2(leftVertex.y, 0);

	//---建立模板遮罩---//
	stencilDrawNode->clear();
	//不包括发射器部分
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
		auto ejector = Sprite::create("GreenEjector.png");
		ejector->setPosition(Vec2(topVertex.x - (i + 1)*halfDiagonal, topVertex.y - (2 + i)*halfDiagonal));
		ejector->setScaleX(diagonal / ejector->getContentSize().width);
		ejector->setScaleY(diagonal / ejector->getContentSize().height);
		ejector->setTag(i);
		ejectorNode->addChild(ejector);
	}
	for (int i = 0; i < drawRCol - 1; i++)
	{
		auto ejector = Sprite::create("BlueEjector.png");
		ejector->setPosition(Vec2(topVertex.x + (i + 1)*halfDiagonal, topVertex.y - (2 + i)*halfDiagonal));
		ejector->setScaleX(diagonal / ejector->getContentSize().width);
		ejector->setScaleY(diagonal / ejector->getContentSize().height);
		ejector->setTag(lCol + i);
		ejectorNode->addChild(ejector);
	}
	updateChessboard();
}

void GameScene::updateChessboard()
{
	chessmanNode->removeAllChildren();
	for (int i = 0; i < lCol; i++) {
		for (int j = 0; j < rCol; j++) {
			auto chessman = createSpriteByChessman(chessmen[i][j]);
			chessman->setPosition(((j - i)*halfDiagonal + topVertex.x), drawSize - (i + j + 3)*halfDiagonal);
			chessman->setTag(i*rCol + j);
			chessmanNode->addChild(chessman);
		}
	}
}

Sprite * GameScene::createSpriteByChessman(Chessman type)
{
	std::string filename;
	switch (type) {
	case common:
		filename = "CommonChessman.png";
		break;
	case key:
		filename = "KeyChessman.png";
		break;
	case addCol:
		filename = "AddcolChessman.png";
		break;
	case delCol:
		filename = "DelcolChessman.png";
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
		newChessman->setPosition(((turn == left ? -movingCol - 1 : movingCol + 1) * halfDiagonal + topVertex.x), drawSize - (movingCol + 2)*halfDiagonal);
		chessmanNode->addChild(newChessman);
		auto movingAction = MoveBy::create(movingTime, Vec2(turn == left ? halfDiagonal : -halfDiagonal, -halfDiagonal));
		for (int j = 0; j < (turn == left ? rCol : lCol); j++) {
			chessmanNode->getChildByTag(turn == left ? movingCol*rCol + j : j*rCol + movingCol)->runAction(movingAction->clone());
		}
		//把移动动作添加到新棋子的时候顺便加个调用动作
		auto callingAction = CallFunc::create(CC_CALLBACK_0(GameScene::endMoving, this));
		auto movingAndCallingAction = Sequence::create(movingAction, callingAction, NULL);
		newChessman->runAction(movingAndCallingAction);
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
		case key: {
			rightWins();
			break;
		}
		case addCol:
			setBoardSize(lCol, rCol + 1);
			break;
		case delCol:
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
		case key: {
			leftWins();
			break;
		}
		case addCol:
			setBoardSize(lCol + 1, rCol);
			break;
		case delCol:
			setBoardSize(lCol - 1, rCol);
			break;
		}
	}
	updateChessboard();
}

void GameScene::changeTurn()
{
	turn = (turn == left ? right : left);
}

void GameScene::leftWins()
{
	auto rs = ResultScene::create(XmlData::text["green wins"], Color4B(0, 255, 0, 255));
	Director::getInstance()->replaceScene(rs);
}

void GameScene::rightWins()
{
	auto rs = ResultScene::create(XmlData::text["blue wins"], Color4B(0, 0, 255, 255));
	Director::getInstance()->replaceScene(rs);
}

Chessman GameScene::getRandomChessman()
{
	switch (rand() % 10)
	{
	case 0:
		return key;
	case 1:
		return addCol;
	case 2:
		return delCol;
	default:
		return common;
	}
}
