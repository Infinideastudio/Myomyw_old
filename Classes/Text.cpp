#include "Text.h"
#include <sstream>

std::unordered_map<std::string, std::string> Text::texts;

void Text::loadLang(std::string lang)
{
	std::istringstream text(FileUtils::getInstance()->getStringFromFile("lang/" + lang + ".lang"));
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

Label* Text::createLabel(std::string text, float size, Color4B color)
{
	auto label = Label::createWithTTF(text, "fonts/deng.ttf", size);
	label->setTextColor(color);
	return label;
}
