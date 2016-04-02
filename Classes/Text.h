#pragma once
#include <unordered_map>

class Text
{
public:
	static void loadLang(std::string lang);
	static std::string get(std::string text);
private:
	static std::unordered_map<std::string, std::string> texts;
};