#include "Json.h"

Json::Json()
{
	doc.SetObject();
}

Json::Json(std::string str)
{
	doc.Parse(str.c_str());
}

bool Json::getBool(const char* name)
{
	if (doc.HasMember(name) && doc[name].IsBool()) {
		return doc[name].GetBool();
	}
	return false;
}

int Json::getInt(const char* name)
{
	if (doc.HasMember(name) && doc[name].IsInt()) {
		return doc[name].GetInt();
	}
	return 0;
}

int64_t Json::getInt64(const char* name)
{
	if (doc.HasMember(name) && doc[name].IsInt64()) {
		return doc[name].GetInt64();
	}
	return 0;
}

unsigned int Json::getUInt(const char* name)
{
	if (doc.HasMember(name) && doc[name].IsUint()) {
		return doc[name].GetUint();
	}
	return 0;
}

uint64_t Json::getUInt64(const char* name)
{
	if (doc.HasMember(name) && doc[name].IsUint64()) {
		return doc[name].GetUint64();
	}
	return 0;
}

double Json::getDouble(const char* name)
{
	if (doc.HasMember(name) && doc[name].IsDouble()) {
		return doc[name].GetDouble();
	}
	return 0.0;
}

const char* Json::getString(const char* name)
{
	if (doc.HasMember(name) && doc[name].IsString()) {
		return doc[name].GetString();
	}
	return nullptr;
}

std::string Json::toString()
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	return buffer.GetString();
}