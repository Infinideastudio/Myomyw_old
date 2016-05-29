#include "OptionScene.h"
#include "MainScene.h"
#include "Text.h"

bool OptionScene::init()
{
	if (!Scene::init())
		return false;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//--返回按钮--//
	auto backItem = MenuItemImage::create("UI/BackNormal.png", "UI/BackSelected.png", [](Ref* pSender) {
		Director::getInstance()->replaceScene(TransitionFlipX::create(0.5f, MainScene::create(), TransitionScene::Orientation::LEFT_OVER));
	});
	backItem->setPosition(Vec2(backItem->getContentSize() / 2 + Size(10, 10)));
	//--标题--//
	auto titleLabel = Text::createLabel(Text::get("optionTitle"), 40, Color4B::BLACK);
	titleLabel->setPosition(visibleSize.width / 2, visibleSize.height - titleLabel->getContentSize().height / 2 - 20);
	this->addChild(titleLabel);

	auto menu = Menu::create(backItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu);
	return true;
}
