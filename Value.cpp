#include "Value.h"

Value::Value(ValueType t, std::variant<std::monostate, bool, double, std::shared_ptr<Object>> d)
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

Value Value::Obj(std::shared_ptr<Object>obj) {
    return{ ValueType::OBJ , std::move(obj)};
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

std::shared_ptr<Object> Value::as_obj() const {
    return std::get<std::shared_ptr<Object>>(this->data);
}

std::shared_ptr<Object> Value::transfer_obj() {
    return std::exchange(std::get<std::shared_ptr<Object>>(data), nullptr);
}

//ObjString* Value::as_string() const {
//    const ObjString* other_v = dynamic_cast<ObjString*>(v.as_obj());
//
//}

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
bool Value::is_string(const Value& v) {
    return v.as_obj()->obj_type() == ObjType::OBJ_STRING;

  /* const ObjString* other_v = dynamic_cast<ObjString*>(v.as_obj());
   if (!other_v) return false;
   return true;*/
}

bool Value::valuesEqual(Value& a, Value& b) {
    if (a.type != b.type) return false;
    switch (a.type) {
    case ValueType::BOOL:   return a.as_bool() == b.as_bool();
    case ValueType::NIL:    return true;
    case ValueType::NUMBER: return a.as_number() == b.as_number();
    case ValueType::OBJ: {
        //FIXME  string interning should work
        return a.as_obj() == b.as_obj();
        
        //return ob1->compare(ob2);
         
    }
    default: return false; 
    }
    std::cerr << "shouldnt be reaching end of valEqual\n";
    return false;
}

void Value::print_value(const Value& value) {
    std::visit([](const auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            std::cout << "nil";
        }
        else if constexpr (std::is_same_v<T, std::shared_ptr<Object>>) {
            std::cout << "object:\t";
            arg->print();  
        }
        else {
            std::cout << arg;
        }
        }, value.data);
    std::cout << std::endl;
}
//static in declaration in Value class
//FIXME string interning not implemented
std::shared_ptr<ObjString> Value::as_string(const Value& v) {
    if (!std::holds_alternative<std::shared_ptr<Object>>(v.data)) return nullptr;

    std::shared_ptr<Object> obj = std::get<std::shared_ptr<Object>>(v.data);

    std::shared_ptr<ObjString> string = std::dynamic_pointer_cast<ObjString>(obj);
    return string; 
}
