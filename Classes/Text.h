#pragma once
#include <unordered_map>
#include "cocos2d.h"
USING_NS_CC;

class Text
{
public:
	static void loadLang(std::string lang);
	static std::string get(std::string text);
	static Label* createLabel(std::string text, float size, Color4B color);
private:
	static std::unordered_map<std::string, std::string> texts;
};