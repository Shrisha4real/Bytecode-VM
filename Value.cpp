#include "Value.h"

Value::Value(ValueType t, std::variant<std::monostate, bool, double> d)
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
double Value::as_number() const {
    return std::get<double>(this->data);
}

bool Value::is_nil() const {
    return std::holds_alternative<std::monostate>(data);
}

bool Value::as_bool() const {
    return std::get<bool>(this->data);
}