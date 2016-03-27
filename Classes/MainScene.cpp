#include "MainScene.h"
#include "PvpGameScene.h"
#include "PvaGameScene.h"
#include "PvoGameScene.h"
#include "Text.h"
USING_NS_CC;

bool MainScene::init()
{
	if (!Scene::init())
		return false;
	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto background = Sprite::create("UI/MainScene.png");
	float scale = MAX(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
	background->setScale(scale);
	background->setPosition(visibleSize.width / 2, visibleSize.height - background->getContentSize().height * scale * 0.5);
	this->addChild(background);

	auto ui = Layer::create();
	this->addChild(ui);

	auto pvpLabel = Label::createWithTTF(Text::get("pvp"), "fonts/Deng.ttf", 32);
	pvpLabel->setTextColor(Color4B(30, 100, 30, 255));
	pvpLabel->setColor(Color3B(0, 250, 60));
	auto pvpItem = MenuItemLabel::create(pvpLabel, [](Ref* pSender)
	{Director::getInstance()->replaceScene(PvpGameScene::create()); });
	pvpItem->setPosition(visibleSize.width / 2, visibleSize.height / 2 + 100);

	auto pvaLabel = Label::createWithTTF(Text::get("pva"), "fonts/Deng.ttf", 32);
	pvaLabel->setTextColor(Color4B(30, 100, 30, 255));
	auto pvaItem = MenuItemLabel::create(pvaLabel, [](Ref* pSender)
	{Director::getInstance()->replaceScene(PvaGameScene::create());  });
	pvaItem->setPosition(visibleSize.width / 2, visibleSize.height / 2);

	auto pvoLabel = Label::createWithTTF(Text::get("pvo"), "fonts/Deng.ttf", 32);
	pvoLabel->setTextColor(Color4B(30, 100, 30, 255));
	auto pvoItem = MenuItemLabel::create(pvoLabel, CC_CALLBACK_1(MainScene::pvoItemCallback, this));
	pvoItem->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 100);

	auto menu = Menu::create(pvpItem, pvaItem, pvoItem, NULL);
	menu->setPosition(Vec2::ZERO);
	ui->addChild(menu);

	addressBox = ui::EditBox::create(Size(400, 40), ui::Scale9Sprite::create(Rect(1.5, 1.5, 1, 1), "UI/EditBox.png"));
	addressBox->setFontColor(Color4B(0, 0, 0, 255));
	addressBox->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 150));
	addressBox->setPlaceHolder(Text::get("enterAddress").c_str());
	addressBox->setPlaceholderFontColor(Color4B(100, 100, 100, 255));
	ui->addChild(addressBox);

	ui->setPosition(0, -ui->getContentSize().height);
	auto moveAction = MoveTo::create(1, Vec2::ZERO);
	auto easeAction = EaseBackOut::create(moveAction);
	ui->runAction(easeAction);

	auto versionLabel = Label::createWithTTF(Text::get("version"), "fonts/Deng.ttf", 25);
	versionLabel->setTextColor(Color4B(0, 0, 0, 255));
	versionLabel->setPosition(visibleSize.width - versionLabel->getContentSize().width / 2, versionLabel->getContentSize().height / 2);
	this->addChild(versionLabel);
	return true;
}

void MainScene::pvoItemCallback(Ref * pSender)
{
	Director::getInstance()->replaceScene(PvoGameScene::create(addressBox->getText()));
}