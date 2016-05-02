#pragma once
class GameEvent
{
public:
	GameEvent();
	enum class Type
	{

	};
	Type getType() { return type; }

private:
	Type type;
	void* data;
};