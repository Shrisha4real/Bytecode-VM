#pragma once
#include<string>
#include<iostream>
typedef enum {
	OBJ_STRING,
} ObjType;

class Object
{
	ObjType type{};
public:
	Object(ObjType type) :type(type) {};

	virtual void print() const = 0;
	virtual bool compare(const Object* other) const =0 ;
	ObjType obj_type() const;
	virtual ~Object() = default;
};
class ObjString : public Object{
	std::string s{};
public:
	ObjString(const char* start, int length) :s(start, length), Object(ObjType::OBJ_STRING) {};
	ObjString& operator +=(const ObjString& b);    
	virtual void print() const override;
	virtual bool compare(const Object* other) const override;
	const std::string& get_string();
	~ObjString();
};
