#pragma once
#include "cocos2d.h"
USING_NS_CC;
class ResultScene :public cocos2d::Scene
{
public:
	bool init();
	CREATE_FUNC(ResultScene);
	void setText(std::string text);
private:
	Label* label;
};