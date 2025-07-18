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
	uint32_t hash{};
	
public:
	ObjString(const char* start, int length);
	ObjString& operator +=(const ObjString& b); 
	ObjString(const ObjString& other);
	bool operator==(const ObjString& other);
	bool operator!=(const ObjString& other);
	const std::string& get_string() const;
	virtual void print() const override;
	virtual bool compare(const Object* other) const override;
	const std::string& get_string();
	const uint32_t get_hash();
	static uint32_t hash_string(const std::string& s);
	~ObjString();

};
