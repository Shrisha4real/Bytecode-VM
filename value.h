#pragma once
#include<iostream>
#include <variant>
#include "Object.h"

enum class ValueType{BOOL, NUMBER , NIL , OBJ};


// FIXME: use templates instead

/*
 methods with name <value_type> is for creating the objects. Eg. Bool(bool b)
 methods with name as_<value_type> is for getting the data the Object of type Value holds
 methods with name is_<value_type> is to verify if the value object is of <value_type> or not
*/
class Value {
	
public:
	ValueType type;
	std::variant < std::monostate, bool, double , Object> data;
	Value(ValueType t, std::variant<std::monostate, bool, double, Object> d);


	static Value Bool(bool b);
	static Value Nil();
	static Value Number(double d);
	static Value Obj(Object obj);
	bool as_bool() const;
	double as_number() const;
	bool as_nil() const;
	Object as_obj() const;

	static bool valuesEqual(Value a, Value b);

	static bool is_bool(const Value& v) ;
	static bool is_number(const Value& v) ;
	static bool is_nil(const Value& v);
	static bool is_obj(const Value& v);

};