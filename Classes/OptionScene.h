#pragma once
#include "cocos2d.h"
#include "ui/CocosGUI.h"
USING_NS_CC;
class OptionScene :public Scene
{
public:
	virtual bool init();
	CREATE_FUNC(OptionScene);
private:
	ui::Button* highlightingButton;

	ui::Layout* createLayout(std::string title, Size size);
	ui::Button* createButton(std::string text, Size size, std::function<void(Ref*)> callback);
	void goInto(ui::Widget* layout, ui::Widget* widget, ui::Widget* newWidget);
	void goOut(ui::Widget* layout, ui::Widget* widget, ui::Widget* newWidget);
};