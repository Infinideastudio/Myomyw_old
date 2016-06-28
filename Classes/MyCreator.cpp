#include "MyCreator.h"

const std::string MyCreator::normalFont = "fonts/Deng.ttf";

Label* MyCreator::createLabel(std::string text, float size, Color4B color)
{
	auto label = Label::createWithTTF(text, normalFont, size);
	label->setTextColor(color);
	return label;
}
