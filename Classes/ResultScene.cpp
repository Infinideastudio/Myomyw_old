#include "ResultScene.h"
#include "MainScene.h"
#include "Lang.h"
#include "MyCreator.h"

bool ResultScene::init(std::string text, Color4B color)
{
	if (!Scene::init())
		return false;
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto label = MyCreator::createLabel(text, 48,color);
	label->setPosition(0.5*visibleSize);
	label->setMaxLineWidth(visibleSize.width);
	this->addChild(label);
	auto backLabel = MyCreator::createLabel(Lang::get("back"), 32,Color4B(0, 0, 0, 255));
	auto backItem = MenuItemLabel::create(backLabel, [](Ref* pSender)
	{Director::getInstance()->replaceScene(MainScene::create()); });
	backItem->setPosition(0.5*Director::getInstance()->getVisibleSize() - Vec2(0, 200));
	auto menu = Menu::create(backItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);
	return true;
}

ResultScene* ResultScene::create(std::string text, Color4B color)
{
	ResultScene* pRet = new(std::nothrow) ResultScene();
	if (pRet && pRet->init(text, color))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}