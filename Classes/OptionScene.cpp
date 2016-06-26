#include "OptionScene.h"
#include "MainScene.h"
#include "Text.h"
USING_NS_CC;

bool OptionScene::init()
{
	if (!Scene::init())
		return false;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	//--返回按钮--//
	auto exitScene = [](Ref* pSender) {
		Director::getInstance()->replaceScene(TransitionFlipX::create(0.5f, MainScene::create(), TransitionScene::Orientation::LEFT_OVER));
	};
	auto backItem = MenuItemImage::create("UI/BackNormal.png", "UI/BackSelected.png", exitScene);
	backItem->setPosition(Vec2(backItem->getContentSize().width / 2 + 10, visibleSize.height - backItem->getContentSize().height / 2 - 10));
	auto backMenu = Menu::create(backItem, NULL);
	backMenu->setPosition(Vec2::ZERO);
	this->addChild(backMenu);
	//--根菜单--//
	auto rootLayout = createLayout(Text::get("optionTitle"), this->getContentSize());
	this->addChild(rootLayout);

	auto list = ui::ListView::create();
	list->setDirection(ui::ScrollView::Direction::VERTICAL);
	list->setGravity(ui::ListView::Gravity::CENTER_HORIZONTAL);
	list->setBounceEnabled(true);
	list->setItemsMargin(10);
	list->setContentSize(Size(700, 480));
	list->setAnchorPoint(Vec2(0.5f, 0.5f));
	list->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 20));
	rootLayout->addChild(list);
	//--语言按钮--//
	auto langButton = createButton(Text::get("selectLang"), Size(600, 60), [=](Ref* pSender) {
		auto button = (ui::Button*)pSender;
		auto langLayout = createLayout(Text::get("selectLang"), Size(600, 600));
		auto langList = ui::ListView::create();
		langList->setDirection(ui::ScrollView::Direction::VERTICAL);
		langList->setItemsMargin(10);
		langList->setGravity(ui::ListView::Gravity::CENTER_HORIZONTAL);
		langList->setBounceEnabled(true);
		langList->setContentSize(Size(600, 480));
		langList->setAnchorPoint(Vec2(0.5f, 0.5f));
		langList->setPosition(Vec2(langLayout->getContentSize().width / 2, langLayout->getContentSize().height / 2 - 20));
		langLayout->addChild(langList);
		auto langs = Text::getAllLangs();

		auto autoLangButton = createButton(Text::get("autoLang"), Size(500, 60), [this](Ref* pSender) {
			auto button = static_cast<ui::Button*> (pSender);
			highlightingButton->loadTextureNormal("UI/ButtonNormal.png");
			highlightingButton->setContentSize(Size(500, 60));
			highlightingButton = button;
			button->loadTextureNormal("UI/ButtonActive.png");
			button->setContentSize(Size(500, 60));
			Text::setAutoLang(true);
			Text::loadAutoLang();
		});
		if (Text::isAutoLang()) {
			autoLangButton->loadTextureNormal("UI/ButtonActive.png");
			autoLangButton->setContentSize(Size(500, 60));
			highlightingButton = autoLangButton;
		}
		langList->addChild(autoLangButton);
		for (auto i : langs) {
			auto langButton = createButton(i.first, Size(500, 60), [i, this](Ref* pSender) {
				auto button = static_cast<ui::Button*> (pSender);
				highlightingButton->loadTextureNormal("UI/ButtonNormal.png");
				highlightingButton->setContentSize(Size(500, 60));
				highlightingButton = button;
				button->loadTextureNormal("UI/ButtonActive.png");
				button->setContentSize(Size(500, 60));
				Text::setAutoLang(false);
				Text::loadLang(i.second);
			});
			if (!Text::isAutoLang() && i.second == Text::getCurrentLang()) {
				langButton->loadTextureNormal("UI/ButtonActive.png");
				langButton->setContentSize(Size(500, 60));
				highlightingButton = langButton;
			}
			langList->addChild(langButton);
		}
		goInto(rootLayout, button, langLayout);
		backItem->setCallback([=](Ref* pSender) {
			goOut(rootLayout, button, langLayout);
			backItem->setCallback(exitScene);
		});

	});
	list->addChild(langButton);
	return true;
}

ui::Button* OptionScene::createButton(std::string text, Size size, std::function<void(Ref*)> callback)
{
	auto button = ui::Button::create("UI/ButtonNormal.png", "UI/ButtonSelected.png");
	button->setTitleText(text);
	button->setTitleFontName(Text::normalFont);
	button->setTitleColor(Color3B::BLACK);
	button->setTitleFontSize(30);
	button->addClickEventListener(callback);
	button->setScale9Enabled(true);
	button->setContentSize(size);
	return button;
}

ui::Layout * OptionScene::createLayout(std::string title, Size size)
{
	auto layout = ui::Layout::create();
	auto titleLabel = Text::createLabel(title, 40, Color4B::BLACK);
	titleLabel->setPosition(size.width / 2, size.height - titleLabel->getContentSize().height / 2 - 20);
	layout->addChild(titleLabel);
	layout->setContentSize(size);
	return layout;
}

const float goIntoOutTime = 0.5f;

void OptionScene::goInto(ui::Widget* layout, ui::Widget* widget, ui::Widget* newWidget)
{
	layout->setEnabled(false);
	newWidget->setEnabled(false);
	auto buttonCenter = layout->convertToNodeSpace(widget->getWorldPosition());
	auto layoutSize = layout->getContentSize();
	auto buttonSize = widget->getContentSize();
	layout->setAnchorPoint(Vec2(buttonCenter.x / layoutSize.width, buttonCenter.y / layoutSize.height));
	layout->setPosition(buttonCenter);
	auto moveAction = MoveTo::create(goIntoOutTime, this->getContentSize() / 2);
	layout->runAction(moveAction);
	int sacle;
	if (buttonSize.width > buttonSize.height) {
		sacle = this->getContentSize().height / buttonSize.height;
	}
	else {
		sacle = this->getContentSize().width / buttonSize.width;
	}
	auto scaleAction = ScaleTo::create(goIntoOutTime, sacle);
	layout->runAction(scaleAction);
	auto fadeOutAaction = FadeOut::create(goIntoOutTime);
	widget->runAction(fadeOutAaction);

	newWidget->setAnchorPoint(Vec2(0.5f, 0.5f));
	newWidget->setPosition(buttonCenter);
	this->addChild(newWidget);
	newWidget->setScale(1 / sacle);
	auto newScaleAction = ScaleTo::create(goIntoOutTime, 1.0f);
	newWidget->runAction(newScaleAction);
	newWidget->runAction(moveAction->clone());
	auto fadeInAaction = FadeIn::create(goIntoOutTime);
	newWidget->runAction(fadeInAaction);
	scheduleOnce([newWidget](float) {
		newWidget->setEnabled(true);
	}, goIntoOutTime, "goInto");
}

void OptionScene::goOut(ui::Widget* layout, ui::Widget* widget, ui::Widget* newWidget)
{
	layout->setEnabled(false);
	newWidget->setEnabled(false);
	auto buttonCenter = layout->convertToNodeSpace(widget->getWorldPosition());
	auto layoutSize = layout->getContentSize();
	auto buttonSize = widget->getContentSize();
	layout->setAnchorPoint(Vec2(buttonCenter.x / layoutSize.width, buttonCenter.y / layoutSize.height));
	layout->setPosition(buttonCenter);
	auto moveAction = MoveTo::create(goIntoOutTime, buttonCenter);
	layout->runAction(moveAction);
	auto scaleAction = ScaleTo::create(goIntoOutTime, 1.0f);
	layout->runAction(scaleAction);
	auto fadeInAaction = FadeIn::create(goIntoOutTime);
	widget->runAction(fadeInAaction);

	int sacle;
	if (buttonSize.width > buttonSize.height) {
		sacle = this->getContentSize().height / buttonSize.height;
	}
	else {
		sacle = this->getContentSize().width / buttonSize.width;
	}
	auto newScaleAction = ScaleTo::create(goIntoOutTime, 1 / sacle);
	newWidget->runAction(newScaleAction);
	newWidget->runAction(moveAction->clone());
	auto fadeOutAaction = FadeOut::create(goIntoOutTime);
	newWidget->runAction(fadeOutAaction);
	scheduleOnce([this, newWidget, layout](float) {
		this->removeChild(newWidget);
		layout->setEnabled(true);
	}, goIntoOutTime, "goOut");
}
