#include "MainScene.h"
#include "PvpGameScene.h"
#include "PvaGameScene.h"
#include "PvoGameScene.h"
#include "OptionScene.h"
#include "Text.h"
#include "Player.h"
USING_NS_CC;

bool MainScene::init()
{
	if (!Scene::init())
		return false;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//--背景图片--//
	auto background = Sprite::create("UI/MainScene.png");
	float scale = MAX(visibleSize.width / background->getContentSize().width, visibleSize.height / background->getContentSize().height);
	background->setScale(scale);
	background->setPosition(visibleSize.width / 2, visibleSize.height - background->getContentSize().height * scale * 0.5);
	this->addChild(background);

	auto optionItem = MenuItemImage::create("UI/OptionNormal.png", "UI/OptionSelected.png", [](Ref* pSender) {
		Director::getInstance()->replaceScene(TransitionFlipX::create(0.5f, OptionScene::create()));
	});
	optionItem->setPosition((Vec2)optionItem->getContentSize() / 2 + Vec2(10, 10));
	//--固定菜单--//
	auto fixedMenu = Menu::create(optionItem, NULL);
	fixedMenu->setPosition(Vec2::ZERO);
	this->addChild(fixedMenu);
	//--滑动界面层--//
	scrollableLayer = Layer::create();
	if (Player::isLogged()) {
		scrollableLayer->setPosition(-visibleSize.width, 0);
	}
	this->addChild(scrollableLayer);
	//--登录界面层--//
	auto loginLayer = Layer::create();
	scrollableLayer->addChild(loginLayer);
	//--标题--//
	auto titleLabel = Text::createLabel(Text::get("loginTitle"), 40, Color4B::BLACK);
	titleLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2 + 100);
	loginLayer->addChild(titleLabel);

	nameBox = ui::EditBox::create(Size(400, 40), ui::Scale9Sprite::create(Rect(1.5, 1.5, 1, 1), "UI/EditBox.png"));
	nameBox->setFontColor(Color4B::BLACK);
	nameBox->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 50));
	nameBox->setPlaceHolder(Text::get("enterName").c_str());
	nameBox->setPlaceholderFontColor(Color4B(100, 100, 100, 255));
	loginLayer->addChild(nameBox);

	addressBox = ui::EditBox::create(Size(400, 40), ui::Scale9Sprite::create(Rect(1.5, 1.5, 1, 1), "UI/EditBox.png"));
	addressBox->setFontColor(Color4B::BLACK);
	addressBox->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 50));
	addressBox->setPlaceHolder(Text::get("enterAddress").c_str());
	addressBox->setPlaceholderFontColor(Color4B(100, 100, 100, 255));
	loginLayer->addChild(addressBox);

	auto loginLabel = Text::createLabel(Text::get("login"), 32, Color4B(30, 100, 30, 255));
	auto loginItem = MenuItemLabel::create(loginLabel, [this](Ref* pSender) {
		Player::login(nameBox->getText(), addressBox->getText(), CC_CALLBACK_0(MainScene::moveToMainLayer, this), [](std::string error) {
			MessageBox(error.c_str(), "Myomyw");
		});
	});
	loginItem->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 100);

	auto guestLabel = Text::createLabel(Text::get("loginWithGuest"), 32, Color4B(30, 100, 30, 255));
	auto guestItem = MenuItemLabel::create(guestLabel, [this](Ref* pSender) {
		Player::loginWithGuest();
		moveToMainLayer();
	});
	guestItem->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 200);

	loginMenu = Menu::create(loginItem, guestItem, NULL);
	loginMenu->setPosition(Vec2::ZERO);
	loginLayer->addChild(loginMenu);
	//--主界面层--//
	auto mainLayer = Layer::create();
	mainLayer->setPosition(Vec2(visibleSize.width, 0));
	scrollableLayer->addChild(mainLayer);

	auto pvpLabel = Text::createLabel(Text::get("pvp"), 32, Color4B(30, 100, 30, 255));
	auto pvpItem = MenuItemLabel::create(pvpLabel, [](Ref* pSender)
	{Director::getInstance()->replaceScene(PvpGameScene::create()); });
	pvpItem->setPosition(visibleSize.width / 2, visibleSize.height / 2 + 100);

	auto pvaLabel = Text::createLabel(Text::get("pva"), 32, Color4B(30, 100, 30, 255));
	auto pvaItem = MenuItemLabel::create(pvaLabel, [](Ref* pSender)
	{Director::getInstance()->replaceScene(PvaGameScene::create());  });
	pvaItem->setPosition(visibleSize.width / 2, visibleSize.height / 2);

	auto pvoLabel = Text::createLabel(Text::get("pvo"), 32, Color4B(30, 100, 30, 255));
	pvoItem = MenuItemLabel::create(pvoLabel, [](Ref* pSender) {
		Director::getInstance()->replaceScene(PvoGameScene::create());
	});
	pvoItem->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 100);
	if (Player::isLogged()) {
		pvoItem->setEnabled(false);
	}

	auto logoutLabel = Text::createLabel(Text::get("logout"), 25, Color4B(255, 0, 0, 255));
	auto logoutItem = MenuItemLabel::create(logoutLabel, [this](Ref* pSender) {
		Player::logout();
		moveToLoginLayer();
	});
	logoutItem->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 200);

	mainMenu = Menu::create(pvpItem, pvaItem, pvoItem, logoutItem, NULL);
	mainMenu->setPosition(Vec2::ZERO);
	mainMenu->setEnabled(false);
	mainLayer->addChild(mainMenu);

	playerLabel = Text::createLabel("", 25, Color4B(0, 0, 0, 255));
	mainLayer->addChild(playerLabel);
	if (Player::isLogged()) {
		updatePlayerLabel();
	}

	auto versionLabel = Text::createLabel("Alpha 0.2", 25, Color4B(0, 0, 0, 255));
	versionLabel->setPosition(visibleSize.width - versionLabel->getContentSize().width / 2, versionLabel->getContentSize().height / 2);
	this->addChild(versionLabel);
	return true;
}

void MainScene::moveToMainLayer()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto moveAction = MoveTo::create(1, Vec2(-visibleSize.width, 0));
	auto easeAction = EaseExponentialInOut::create(moveAction);
	scrollableLayer->runAction(easeAction);
	pvoItem->setEnabled(!Player::isGuest());
	updatePlayerLabel();
}

void MainScene::moveToLoginLayer()
{
	loginMenu->setEnabled(true);
	mainMenu->setEnabled(false);
	auto moveAction = MoveTo::create(1, Vec2::ZERO);
	auto easeAction = EaseExponentialInOut::create(moveAction);
	scrollableLayer->runAction(easeAction);
}

void MainScene::updatePlayerLabel()
{
	Size visibleSize = Director::getInstance()->getVisibleSize();
	loginMenu->setEnabled(false);
	mainMenu->setEnabled(true);
	auto str = Player::getName();
	if (!Player::isGuest()) {
		str.append("  ");
		str.append(Text::get("serverName"));
		str.append(Player::getServerName());
	}
	playerLabel->setString(str);
	playerLabel->setPosition(Vec2(playerLabel->getContentSize().width / 2 + 20,
		visibleSize.height - playerLabel->getContentSize().height / 2 - 20));
}
