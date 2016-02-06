#pragma once
#include "cocos2d.h"
USING_NS_CC;
class ResultScene :public cocos2d::Scene
{
public:
	bool init(std::string text, Color4B color);
	static ResultScene* create(std::string text, Color4B color);
};