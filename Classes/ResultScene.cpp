#include "ResultScene.h"
#include "MainScene.h"
#include "XmlData.h"
#include "ResFiles.h"
bool ResultScene::init(std::string text, Color4B color)
{
	if (!Scene::init())
		return false;
	auto label = Label::createWithTTF(text, normalFont, 48);
	label->setTextColor(color);
	label->setPosition(0.5*Director::getInstance()->getVisibleSize());
	this->addChild(label);
	auto backLabel = Label::createWithTTF(XmlData::text["back"], normalFont, 32);
	backLabel->setTextColor(Color4B(0, 0, 0, 255));
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