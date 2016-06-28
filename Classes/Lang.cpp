#include "Lang.h"
#include <sstream>

bool Lang::autoLang = true;
std::string Lang::currentLang;
std::unordered_map<std::string, std::string> Lang::texts;

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

void Lang::loadLang(std::string lang)
{
	currentLang = lang;
	READ_MAP("lang/" + lang + ".lang", "=", texts);
}

void Lang::init()
{
	if (autoLang) {
		loadAutoLang();
	}
}

void Lang::loadAutoLang()
{
	switch (Application::getInstance()->getCurrentLanguage()) {
	case LanguageType::CHINESE:
		loadLang("zh_CN");
		break;
	case LanguageType::ENGLISH:
	default:
		loadLang("en_US");
		break;
	}
}

std::map<std::string, std::string> Lang::getAllLangs()
{
	std::map<std::string, std::string> langs;
	READ_MAP("lang/langs.txt", "|", langs);
	return langs;
}

std::string Lang::get(std::string text)
{
	return texts[text];
}