#pragma once
#include<iostream>
#include <variant>
enum class ValueType{BOOL, NUMBER , NIL};


// FIXME: use templates instead
class Value {
	
public:
	ValueType type;
	std::variant < std::monostate, bool, double> data;
	Value(ValueType t, std::variant<std::monostate, bool, double> d);
	static Value Bool(bool b);
	static Value Nil();
	static Value Number(double d);

	bool as_bool() const;
	double as_number() const;
	bool as_nil() const;

	static bool valuesEqual(Value a, Value b);

	static bool is_bool(Value v) ;
	static bool is_number(Value v) ;
	static bool is_nil(Value v);


};