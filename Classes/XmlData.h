#pragma once
#include "cocos2d.h"
#include "tinyxml2/tinyxml2.h"

class XmlData
{
public:
	static void init();
	static std::unordered_map<std::string, std::string> text;
};