#include "MainScene.h"
#include "PvpGameScene.h"
#include "PvaGameScene.h"
#include "PvoGameScene.h"
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
	pvpItem->setPosition(visibleSize.width / 2, visibleSize.height / 2 + 100);

	auto pvaLabel = Label::createWithTTF(XmlData::text["pva"], "fonts/Deng.ttf", 32);
	pvaLabel->setTextColor(Color4B(0, 0, 0, 255));
	auto pvaItem = MenuItemLabel::create(pvaLabel, [](Ref* pSender)
	{Director::getInstance()->replaceScene(PvaGameScene::create());  });
	pvaItem->setPosition(visibleSize.width / 2, visibleSize.height / 2);

	auto pvoLabel = Label::createWithTTF(XmlData::text["pvo"], "fonts/Deng.ttf", 32);
	pvoLabel->setTextColor(Color4B(0, 0, 0, 255));
	auto pvoItem = MenuItemLabel::create(pvoLabel, CC_CALLBACK_1(MainScene::pvoItemCallback, this));
	pvoItem->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 100);

	auto menu = Menu::create(pvpItem, pvaItem, pvoItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);

	addressBox = ui::EditBox::create(Size(400, 40), ui::Scale9Sprite::create(Rect(1, 1, 1, 1), "EditBox.png"));
	addressBox->setFontColor(Color4B(0, 0, 0, 255));
	addressBox->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 150));
	addressBox->setPlaceHolder(XmlData::text["enter address"].c_str());
	this->addChild(addressBox);

	auto versionLabel = Label::createWithTTF(XmlData::text["version"], "fonts/Deng.ttf", 25);
	versionLabel->setTextColor(Color4B(0, 0, 0, 255));
	versionLabel->setPosition(visibleSize.width - versionLabel->getContentSize().width / 2, versionLabel->getContentSize().height / 2);
	this->addChild(versionLabel);

	return true;
}

void MainScene::pvoItemCallback(Ref * pSender)
{
	Director::getInstance()->replaceScene(PvoGameScene::create(addressBox->getText()));
}