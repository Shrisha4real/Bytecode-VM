#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "Value.h"
#include "Object.h"

class Table {
private:
    struct Entry {
        std::shared_ptr<ObjString> key;
        std::shared_ptr<Value> value;
        bool occupied;
        bool deleted;

        Entry() : occupied(false), deleted(false) {}
    };

    std::vector<Entry> table;
    size_t size;
    const double max_load_factor = 0.75;
 
     uint32_t hash_string(const std::shared_ptr<ObjString>& key) const;
     std::optional<size_t> find_slot(const std::shared_ptr<ObjString>& key) const;
     void resize();

public:
    Table(size_t capacity = 3);
    bool insert(const std::shared_ptr<ObjString>& key, const std::shared_ptr<Value>& value);
    std::shared_ptr<Value> find(const std::shared_ptr<ObjString>& key) const;
    bool remove(const std::shared_ptr<ObjString>& key);
    std::shared_ptr<ObjString> find_string(const std::string& s, uint32_t hash)const;

};
