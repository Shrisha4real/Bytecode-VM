#include "StringInterner.h"
#include"Table.h"
#include"Object.h"
#include"common.h"
#include"Value.h"


StringInterner::StringInterner() {
    strings = std::make_shared<Table>();
}

std::shared_ptr<ObjString> StringInterner::allocate_string(const std::string& chars) {
    auto obj = std::make_shared<ObjString>(chars.c_str(), static_cast<int>(chars.size()));
    Value nil{ ValueType::NIL, std::monostate{} };
    strings->insert(obj,nil );
    return obj;
}

std::shared_ptr<ObjString> StringInterner::copy_string(const char* chars, int length) {
    std::string str(chars, length);
    uint32_t hash = ObjString::hash_string(str);
    auto interned = strings->find_string(str, hash);
    if (interned) return interned;

    return allocate_string(str);
}

std::shared_ptr<ObjString> StringInterner::take_string(std::string&& str) {
    uint32_t hash = ObjString::hash_string(str);
    auto interned = strings->find_string(str, hash);
    if (interned) return interned;

    return allocate_string(std::move(str));
}

std::shared_ptr<Table> StringInterner::get_table() const {
    return strings;
}