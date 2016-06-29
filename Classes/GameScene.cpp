#include "GameScene.h"
#include "Lang.h"
#include "MyCreator.h"
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
	board->ignoreAnchorPointForPosition(false);
	board->setContentSize(Size(drawLength, drawLength));
	board->setPosition(visibleSize / 2);
	this->addChild(board);
	//--网格绘制节点--//
	gridNode = Node::create();
	board->addChild(gridNode, 0);
	border = DrawNode::create();
	board->addChild(border);
	//--遮罩及棋子节点--//
	stencilDrawNode = DrawNode::create();
	stencil = ClippingNode::create(stencilDrawNode);
	chessmanNode = Node::create();
	stencil->addChild(chessmanNode);
	board->addChild(stencil, 1);
	//--双方名称--//
	leftNameLabel = MyCreator::createLabel("", 26, Color4B(0, 0, 0, 255));
	this->addChild(leftNameLabel);
	rightNameLabel = MyCreator::createLabel("", 26, Color4B(0, 0, 0, 255));
	this->addChild(rightNameLabel);
	//--设置回调--//
	auto ejectorTouchListener = EventListenerTouchOneByOne::create();//<-这个只是添加到Layer
	ejectorTouchListener->onTouchBegan = CC_CALLBACK_2(GameScene::ejectorTouchBeganCallback, this);
	ejectorTouchListener->onTouchEnded = CC_CALLBACK_2(GameScene::ejectorTouchEndedCallback, this);
	ejectorTouchListener->setSwallowTouches(true);//目前加不加都没事，加了不让事件向下传递
	auto eventDispatcher = Director::getInstance()->getEventDispatcher();
	eventDispatcher->addEventListenerWithSceneGraphPriority(ejectorTouchListener, this);

	buildChessboard();
	setTurn(left);
	setTurnFlag();

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

void GameScene::setNextChessman(Chessman chessman)
{
	nextChessman = chessman;
	auto old = this->getChildByName("next");
	auto moveAction = MoveBy::create(nextChessmanChangeTime, Vec2(0, -20));
	if (old) {
		old->setName("");
		auto fadeOutAction = FadeOut::create(nextChessmanChangeTime);
		auto callAction = CallFunc::create([this, old]() {this->removeChild(old); });
		auto sequenceAction = Sequence::create(fadeOutAction, callAction, NULL);
		old->runAction(moveAction);
		old->runAction(sequenceAction);
	}

	auto nextChessmanSprite = createSpriteByChessman(nextChessman);
	nextChessmanSprite->setOpacity(0);
	nextChessmanSprite->setScale(0.8f);
	nextChessmanSprite->setPosition((Vec2)this->getContentSize() - Vec2(50, 70));
	nextChessmanSprite->setName("next");
	auto fadeInAction = FadeIn::create(nextChessmanChangeTime);
	nextChessmanSprite->runAction(moveAction->clone());
	nextChessmanSprite->runAction(fadeInAction);
	this->addChild(nextChessmanSprite);
}

//设置回合标志
void GameScene::setTurnFlag()
{
	int leftOpacity = (turn == left ? 255 : 150);
	int rightOpacity = (turn == right ? 255 : 150);
	for (int i = 0; i < lCol; i++) {
		leftEjectors[i]->setOpacity(leftOpacity);
	}
	for (int i = 0; i < rCol; i++) {
		rightEjectors[i]->setOpacity(rightOpacity);
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
	auto visibleSize = Director::getInstance()->getVisibleSize();
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
	//--绘制网格--//
	gridNode->removeAllChildren();
	for (int l = 0; l < drawLCol; l++)
	{
		for (int r = 0; r < drawRCol; r++)
		{
			Sprite* grid;
			if (l == 0 && r == 0) {
				continue;
			}
			else if (r == 0) {
				grid = Sprite::create("Grid/GreenEjector.png");
				leftEjectors[l - 1] = grid;
			}
			else if (l == 0) {
				grid = Sprite::create("Grid/BlueEjector.png");
				rightEjectors[r - 1] = grid;
			}
			else if ((l + r) % 2 == 0) {
				grid = Sprite::create("Grid/Grid1.png");
			}
			else {
				grid = Sprite::create("Grid/Grid2.png");
			}
			grid->setRotation(45);
			grid->setScale(diagonal / sqrt(2 * pow(MAX(grid->getContentSize().width, grid->getContentSize().height), 2)));
			grid->setPosition((r - l)*halfDiagonal + topVertex.x, drawLength - (l + r + 1)*halfDiagonal);
			gridNode->addChild(grid);
		}
		//--边框线--//
		border->clear();
		for (int i = 0; i <= drawLCol; i++)
		{
			border->drawLine(Vec2(topVertex.x - halfDiagonal*i, topVertex.y - halfDiagonal*i),
				i > 1 && i < drawLCol ?
				Vec2(topVertex.x - halfDiagonal*(i - 1), topVertex.y - halfDiagonal*(i + 1)) :
				Vec2(topVertex.x - halfDiagonal*(i - drawRCol), topVertex.y - halfDiagonal*(i + drawRCol)),
				Color4F(0.5, 0.5, 0.5, 1.0));
		}
		for (int i = 0; i <= drawRCol; i++)
		{
			border->drawLine(Vec2(topVertex.x + halfDiagonal*i, topVertex.y - halfDiagonal*i),
				i > 1 && i < drawRCol ?
				Vec2(topVertex.x + halfDiagonal*(i - 1), topVertex.y - halfDiagonal*(i + 1)) :
				Vec2(topVertex.x + halfDiagonal*(i - drawLCol), topVertex.y - halfDiagonal*(i + drawLCol)),
				Color4F(0.5, 0.5, 0.5, 1.0));
		}
	}
	setTurnFlag();
	updateChessboard();
}

//更新棋盘，在棋子变化后调用(棋盘大小改变也会调用)
void GameScene::updateChessboard()
{
	chessmanNode->removeAllChildren();
	for (int l = 0; l < lCol; l++) {
		for (int r = 0; r < rCol; r++) {
			auto chessman = createSpriteByChessman(chessmen[l][r]);
			chessman->setPosition(((r - l)*halfDiagonal + topVertex.x), drawLength - (l + r + 3)*halfDiagonal);
			chessman->setTag(l*rCol + r);
			chessmanNode->addChild(chessman);
		}
	}
}

Sprite* GameScene::createSpriteByChessman(Chessman type)
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
	case Chessman::flip:
		filename = "Chessman/Flip.png";
	}
	auto chessman = Sprite::create(filename);
	chessman->setScaleX(halfDiagonal / chessman->getContentSize().width);
	chessman->setScaleY(halfDiagonal / chessman->getContentSize().height);
	return chessman;
}

bool GameScene::ejectorTouchBeganCallback(Touch * touch, Event * event)
{
	if (controllable && state != ActionState::moving) {
		//遍历当前边的发射器
		for (int i = 0; i < (turn == left ? lCol : rCol); i++) {
			//判断鼠标是否处于发射器内(45°倾斜的正方形)
			auto ejector = turn == left ? leftEjectors[i] : rightEjectors[i];
			Vec2 point = board->convertToNodeSpace(touch->getLocation()) - ejector->getPosition();
			if (point.x + point.y <  halfDiagonal &&
				point.x + point.y > -halfDiagonal &&
				point.x - point.y > -halfDiagonal &&
				point.y - point.x > -halfDiagonal) {
				touching = true;
				beginMoving(i);
			}
		}
	}
	return true;//true表示在这里接收moved和ended事件
}

void GameScene::ejectorTouchEndedCallback(Touch * touch, Event * event)
{
	touching = false;
	//如果正在冷却,那么立刻停止
	if (controllable && state == ActionState::cooling) {
		unschedule("cool");
		changeTurnAndSetTurnFlag();
	}
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
	board->setScale((float)(lCol + rCol) / (this->lCol + this->rCol));
	auto scaleAction = ScaleTo::create(boardScaleTime, 1);
	board->runAction(scaleAction);
	this->lCol = lCol;
	this->rCol = rCol;
	buildChessboard();
	return true;
}

void GameScene::flip()
{
	for (int i = 0; i < lCol; i++)
	{
		for (int j = i + 1; j < rCol; j++)
		{
			std::swap(chessmen[i][j], chessmen[j][i]);
		}
	}
	std::swap(lCol, rCol);

	auto scaleAction1 = ScaleTo::create(boardFilpTime / 2, 0, 1);
	auto callAction1 = CallFunc::create(CC_CALLBACK_0(GameScene::buildChessboard, this));
	auto callAction2 = CallFunc::create(CC_CALLBACK_0(GameScene::setTurnFlag, this));
	auto scaleAction2 = ScaleTo::create(boardFilpTime / 2, 1);
	auto sequenceAction = Sequence::create(scaleAction1, callAction1, callAction2, scaleAction2, NULL);
	board->runAction(sequenceAction);
}

Chessman GameScene::getRandomchessman()
{
	switch (rand() % 10)
	{
	case 0:
		return Chessman::key;
	case 1:
		return Chessman::addCol;
	case 2:
		return Chessman::delCol;
	case 3:
		return Chessman::flip;
	default:
		return Chessman::common;
	}
}

void GameScene::beginMoving(int col)
{
	if (state != ActionState::moving) {
		state = ActionState::moving;
		movingCol = col;
		movingNewChessman = nextChessman;
		auto newChessman = createSpriteByChessman(movingNewChessman);
		newChessman->setPosition(((turn == left ? -movingCol - 1 : movingCol + 1) * halfDiagonal + topVertex.x), drawLength - (movingCol + 2)*halfDiagonal);
		chessmanNode->addChild(newChessman);
		auto movingAction = MoveBy::create(movingTime, Vec2(turn == left ? halfDiagonal : -halfDiagonal, -halfDiagonal));
		for (int i = 0; i < (turn == left ? rCol : lCol); i++) {
			chessmanNode->getChildByTag(turn == left ? movingCol*rCol + i : i*rCol + movingCol)->runAction(movingAction->clone());
		}
		newChessman->runAction(movingAction->clone());
		scheduleOnce(CC_CALLBACK_0(GameScene::endMoving, this), movingTime, "move");
		totalMovements++;
	}
}

void GameScene::endMoving()
{
	Chessman lastChessman;//暂存最底下的棋子
	if (turn == left) {
		lastChessman = chessmen[movingCol][rCol - 1];
		for (int i = rCol - 1; i > 0; i--) {
			chessmen[movingCol][i] = chessmen[movingCol][i - 1];
		}
		chessmen[movingCol][0] = movingNewChessman;
		switch (lastChessman) {
		case Chessman::common:
			updateChessboard();
			break;
		case Chessman::key:
			rightWins();
			break;
		case Chessman::addCol:
			setBoardSize(lCol, rCol + 1);
			break;
		case Chessman::delCol:
			setBoardSize(lCol, rCol - 1);
			break;
		case Chessman::flip:
			flip();
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
		case Chessman::common:
			updateChessboard();
			break;
		case Chessman::key:
			leftWins();
			break;
		case Chessman::addCol:
			setBoardSize(lCol + 1, rCol);
			break;
		case Chessman::delCol:
			setBoardSize(lCol - 1, rCol);
			break;
		case Chessman::flip:
			flip();
			break;
		}
	}

	if (controllable) {
		if (lastChessman == Chessman::flip) {
			changeTurn();
		}
		else {
			if (touching && totalMovements < maxMovementTimes) {
				scheduleOnce(CC_CALLBACK_0(GameScene::beginMoving, this, movingCol), movingCooling, "cool");
				state = ActionState::cooling;
			}
			else {
				changeTurnAndSetTurnFlag();
			}
		}
	}
	else {
		state = ActionState::nothing;
	}

}

void GameScene::setTurn(Side turn)
{
	this->turn = turn;
	totalMovements = 0;
	touching = false;
	state = ActionState::nothing;
}

void GameScene::changeTurn()
{
	setTurn(turn == left ? right : left);
}

void GameScene::changeTurnAndSetTurnFlag()
{
	changeTurn();
	setTurnFlag();
}

void GameScene::leftWins()
{
	auto rs = ResultScene::create(Lang::get("greenWins"), Color4B(0, 255, 0, 255));
	Director::getInstance()->replaceScene(rs);
}

void GameScene::rightWins()
{
	auto rs = ResultScene::create(Lang::get("blueWins"), Color4B(0, 0, 255, 255));
	Director::getInstance()->replaceScene(rs);
}
