#include "XmlData.h"
#include "ResFiles.h"

USING_NS_CC;
std::unordered_map<std::string, std::string> XmlData::text;
void XmlData::init()
{
	auto textDoc = new tinyxml2::XMLDocument();
	textDoc->Parse(FileUtils::getInstance()->getStringFromFile(textFile).c_str());
	auto root = textDoc->RootElement();
	for (auto e = root->FirstChildElement(); e; e = e->NextSiblingElement()) {
		text[e->FirstAttribute()->Value()] = e->FirstAttribute()->Next()->Value();
	}
	delete textDoc;
}
