#pragma once
#include "cocos2d.h"
USING_NS_CC;

class MyCreator
{
public:
	static const std::string normalFont;
	static Label* createLabel(std::string text, float size, Color4B color);
};