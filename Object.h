#pragma once
#include<string>
typedef enum {
	OBJ_STRING,
} ObjType;

class Object
{
	ObjType type{};
public:
	Object(ObjType type) :type(type) {};
	ObjType obj_type() const {
		return type;
	}

};
class ObjString : public Object{
	std::string s{};
public:
	ObjString(const char* start, int length) :s(start, length), Object(ObjType::OBJ_STRING) {};

};
