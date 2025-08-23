#pragma once
#include<string>
#include<iostream>
#include<functional>
#include "chunk.h"
//typedef Value(*NativeFn)(int, int);
using NativeFn = std::function<Value(int, int)>;

// call it like: native(argCount, stack_index);
// because you've captured `this` already
//value is pointing to the value on the stack, in my case the index of the value on the stack

typedef enum {
	OBJ_STRING,OBJ_FUNCTION, OBJ_NATIVE, OBJ_PYTHON
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

class ObjNative : public Object {
public:
	NativeFn function;

	ObjNative(NativeFn fn);
	ObjNative(const ObjNative& other);
	inline virtual void print() const override;
	virtual bool compare(const Object* other) const override;
	virtual std::shared_ptr<Object> clone() const override;

	
};

class ObjPython : public Object {
public:
	py::object py_object;
	ObjPython(py::object obj);
	ObjPython(const ObjPython& other);
	void print() const override;
	bool compare(const Object* other) const override;
	std::shared_ptr<Object> clone() const override;
	~ObjPython() = default;
};