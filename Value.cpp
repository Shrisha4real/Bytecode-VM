#include "Value.h"

Value::Value(ValueType t, std::variant<std::monostate, bool, double, Object> d)
    : type(t), data(std::move(d)) {
}



Value Value::Nil() {
    return { ValueType::NIL, std::monostate{} };
}

Value Value::Number(double n) {
    return { ValueType::NUMBER, n };
}

Value Value::Bool(bool b) {
    return { ValueType::BOOL , b };
}

Value Value::Obj(Object obj) {
    return{ ValueType::OBJ , obj };
}

double Value::as_number() const {
    return std::get<double>(this->data);
}

bool Value::as_nil() const {
    return std::holds_alternative<std::monostate>(data);
}

bool Value::as_bool() const {
    return std::get<bool>(this->data);
}

Object Value::as_obj() const {
    return std::get<Object>(this->data);
}

bool Value::is_bool(const Value& v) {
    return (v.type == ValueType::BOOL);
}
bool Value::is_number(const Value& v) {
    return (v.type == ValueType::NUMBER);
}
bool Value::is_nil(const Value& v) {
    return (v.type == ValueType::NIL);
}
bool Value::is_obj(const Value& v) {
    return (v.type == ValueType::OBJ);
}

bool Value::valuesEqual(Value a, Value b) {
    if (a.type != b.type) return false;
    switch (a.type) {
    case ValueType::BOOL:   return a.as_bool() == b.as_bool();
    case ValueType::NIL:    return true;
    case ValueType::NUMBER: return a.as_number() == b.as_number();
    default: return false; 
    }
    std::cerr << "shouldnt be reaching end of valEqual\n";
    return false;
}