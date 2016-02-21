#pragma once
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

//一个方便读写一层json的类
class Json
{
public:
	Json();
	Json(std::string str);
	template <typename T>
	void add(const char* name, T value)
	{
		doc.AddMember(rapidjson::GenericValue<rapidjson::UTF8<char>>(name, strlen(name)), value, doc.GetAllocator());
	}
	void add(const char* name, const char* value)
	{
		doc.AddMember(rapidjson::GenericValue<rapidjson::UTF8<char>>
			(name, strlen(name)), rapidjson::GenericValue<rapidjson::UTF8<char>>(value, strlen(value)), doc.GetAllocator());
	}
	
	bool getBool(const char* name);
	int getInt(const char* name);
	int64_t getInt64(const char* name);
	unsigned int getUInt(const char* name);
	uint64_t getUInt64(const char* name);
	double getDouble(const char* name);
	const char* getString(const char* name);

	std::string toString();
private:
	rapidjson::Document doc;
};