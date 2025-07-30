#pragma once
#include<string>
#include<iostream>
#include "chunk.h"


typedef enum {
	OBJ_STRING,OBJ_FUNCTION
} ObjType;

class Object
{
	ObjType type{};
public:
	Object(ObjType type) :type(type) {};
	Object(const Object& other);
	inline virtual void print() const = 0;
	virtual bool compare(const Object* other) const =0 ;
	virtual std::shared_ptr<Object> clone() const = 0;
	ObjType obj_type() const;
	virtual ~Object() = default;
};


class ObjString : public Object{
	std::string s{};
	//uint32_t hash{};
	
public:
	ObjString(const char* start, int length);
	ObjString(const std::string& s);
	ObjString(const ObjString& other);
	const std::string& get_string() const;
	ObjString& operator +=(const ObjString& b); 
	ObjString operator+(const ObjString& other) const;
	bool operator==(const ObjString& other);
	bool operator!=(const ObjString& other);
	
	inline virtual void print() const override;
	virtual bool compare(const Object* other) const override;
	virtual std::shared_ptr<Object> clone() const override;
	const std::string& get_string();
	const uint32_t get_hash();
	static uint32_t hash_string(const std::string& s);
	~ObjString() = default;

};

class ObjFunction :public Object {
		
public:
	Chunk chunk;
	int arity;
	std::shared_ptr<ObjString> name;

	ObjFunction(const std::string& name = "<function>");
	ObjFunction(const ObjFunction& other);
	inline virtual void print() const override;
	virtual bool compare(const Object* other) const override;
	virtual std::shared_ptr<Object> clone() const override;
	~ObjFunction() = default;
};