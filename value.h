#pragma once
#include<iostream>
#include <variant>


class Object;
class ObjString;
class VM;
enum class ValueType{BOOL, NUMBER , NIL , OBJ};


// FIXME: use templates instead

/*
 methods with name <value_type> is for creating the objects. Eg. Bool(bool b)
 methods with name as_<value_type> is for getting the data the Object of type Value holds
 methods with name is_<value_type> is to verify if the value object is of <value_type> or not
*/
class Value {
	
public:
	std::shared_ptr<VM>vm;
	ValueType type;
	std::variant < std::monostate, bool, double, std::shared_ptr<Object>> data;
	Value();
	Value(ValueType t, std::variant<std::monostate, bool, double, std::shared_ptr<Object>> d);
	Value(Value&& other) noexcept
		: type(other.type), data(std::move(other.data)) {
	}

	// Move assignment
	Value& operator=(Value&& other) noexcept {
		if (this != &other) {
			type = other.type;
			data = std::move(other.data);
		}
		return *this;
	}
	// Delete copy constructor to be explicit
	Value(const Value&) = delete;
	Value& operator=(const Value&) = delete;


	static Value Bool(bool b);
	static Value Nil();
	static Value Number(double d);
	static Value Obj(std::shared_ptr<Object>obj);
	bool as_bool() const;
	double as_number() const;
	bool as_nil() const;
	std::shared_ptr<Object> as_obj() const;
	static std::shared_ptr<ObjString> as_string(const Value& value);
	std::shared_ptr<Object> transfer_obj();
	//ObjString* as_string() const;
	static void print_value(const Value& value);


	static bool valuesEqual(Value& a, Value& b);

	static bool is_bool(const Value& v) ;
	static bool is_number(const Value& v) ;
	static bool is_nil(const Value& v);
	static bool is_obj(const Value& v);
	static bool is_string(const Value& v);
};