#pragma once

#include <vector>
#include <memory>
#include <optional>
#include"Value.h"
class Object;
class ObjString;
class Value;

class Table {
private:
    struct Entry {
        std::shared_ptr<ObjString> key;
        Value value;
        bool occupied;
        bool deleted;

        Entry() : key(nullptr), value(), occupied(false), deleted(false) {}
    };

    std::vector<Entry> table;
    size_t size;
    const double max_load_factor = 0.75;
 
     uint32_t hash_string(const std::shared_ptr<ObjString> key) const;
     std::optional<size_t> find_slot(const std::shared_ptr<ObjString>& key) const;
     void resize();

public:
    Table(size_t capacity = 3);
    bool insert(const std::shared_ptr<ObjString>& key, Value& value);
    Value* find(const std::shared_ptr<ObjString>& key) ;
    bool remove(const std::shared_ptr<ObjString>& key);
    std::shared_ptr<ObjString> find_string(const std::string& s, uint32_t hash)const;

};
