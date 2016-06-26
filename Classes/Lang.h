#pragma once
#include <unordered_map>
#include "cocos2d.h"
USING_NS_CC;

class Lang
{
public:
	static void loadLang(std::string lang);
	static bool isAutoLang() { return autoLang; };
	static void setAutoLang(bool autoLang) { Lang::autoLang = autoLang; };
	static void init();
	static void loadAutoLang();
	static std::map<std::string, std::string> getAllLangs();
	static std::string get(std::string text);
	static std::string getCurrentLang() { return currentLang; };
private:
	static bool autoLang;
	static std::string currentLang;
	static std::unordered_map<std::string, std::string> texts;
};