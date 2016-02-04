#include "MainScene.h"
#include "PvpGameScene.h"
#include "PvaGameScene.h"
#include "XmlData.h"
USING_NS_CC;
bool MainScene::init()
{
	if (!Scene::init())
		return false;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto pvpLabel = Label::createWithTTF(XmlData::text["pvp"], "fonts/Deng.ttf", 32);
	pvpLabel->setTextColor(Color4B(0, 0, 0, 255));
	auto pvpItem = MenuItemLabel::create(pvpLabel, [](Ref* pSender)
	{Director::getInstance()->replaceScene(PvpGameScene::create()); });
	pvpItem->setPosition(visibleSize.width / 2, visibleSize.height / 2 + 50);

	auto pvaLabel = Label::createWithTTF(XmlData::text["pva"], "fonts/Deng.ttf", 32);
	pvaLabel->setTextColor(Color4B(0, 0, 0, 255));
	auto pvaItem = MenuItemLabel::create(pvaLabel, [](Ref* pSender)
	{Director::getInstance()->replaceScene(PvaGameScene::create());  });
	pvaItem->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 50);
	auto menu = Menu::create(pvpItem, pvaItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);
	return true;
}