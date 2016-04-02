#include "Text.h"
#include <sstream>
#include "cocos2d.h"

std::unordered_map<std::string, std::string> Text::texts;

void Text::loadLang(std::string lang)
{
	std::istringstream text(cocos2d::FileUtils::getInstance()->getStringFromFile("lang/" + lang + ".lang"));
	std::string line;
	int signIndex;
	while (std::getline(text, line, '\n')) {
		if (line.back() == '\r') {
			line.pop_back();
		}
		signIndex = line.find('=');
		texts[line.substr(0, signIndex)] = line.substr(signIndex + 1);
	}
}

std::string Text::get(std::string text)
{
	return texts[text];
}
