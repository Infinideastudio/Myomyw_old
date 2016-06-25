#include "Text.h"
#include <sstream>

bool Text::autoLang = true;
std::string Text::currentLang;
std::unordered_map<std::string, std::string> Text::texts;

const std::string Text::normalFont = "fonts/deng.ttf";

#define READ_MAP(_FILE, _SEPARATOR, _MAP) \
	std::istringstream text(FileUtils::getInstance()->getStringFromFile(_FILE)); \
	std::string line; \
	int signIndex; \
	while (std::getline(text, line, '\n')) { \
		if (line.back() == '\r') { \
			line.pop_back(); \
		} \
		signIndex = line.find(_SEPARATOR); \
		_MAP[line.substr(0, signIndex)] = line.substr(signIndex + 1); \
	} \

void Text::loadLang(std::string lang)
{
	currentLang = lang;
	READ_MAP("lang/" + lang + ".lang", "=", texts);
}

void Text::init()
{
	if (autoLang) {
		loadAutoLang();
	}
}

void Text::loadAutoLang()
{
	switch (Application::getInstance()->getCurrentLanguage()) {
	case LanguageType::CHINESE:
		Text::loadLang("zh_CN");
		break;
	case LanguageType::ENGLISH:
	default:
		Text::loadLang("en_US");
		break;
	}
}

std::map<std::string, std::string> Text::getAllLangs()
{
	std::map<std::string, std::string> langs;
	READ_MAP("lang/langs.txt", "|", langs);
	return langs;
}

std::string Text::get(std::string text)
{
	return texts[text];
}

Label* Text::createLabel(std::string text, float size, Color4B color)
{
	auto label = Label::createWithTTF(text, normalFont, size);
	label->setTextColor(color);
	return label;
}
