#pragma once
#include <variant>
enum class ValueType{BOOL, NUMBER , NIL};

class Value {
	ValueType type;
	std::variant < std::monostate, bool, double> data;
public:
	Value(ValueType t, std::variant<std::monostate, bool, double> d);
	static Value Bool(bool b);
	static Value Nil();
	static Value Number(double d);

	bool as_bool() const;
	double as_number() const;
	bool is_nil() const;

};