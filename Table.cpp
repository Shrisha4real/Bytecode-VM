#include "Table.h"
#include "Value.h"
#include "Object.h"

uint32_t Table::hash_string(const std::shared_ptr<ObjString> key) const {
    uint32_t hash = 2166136261u;
    for (char c : key->get_string()) {
        hash ^= static_cast<uint8_t>(c);
        hash *= 16777619u;
    }
    return hash;
}

std::optional<size_t> Table::find_slot(const std::shared_ptr<ObjString>& key) const {
    if (table.empty()) return std::nullopt;
    size_t index = hash_string(key) % table.size();
    size_t start = index;

    while (true) {
        const Entry& entry = table[index];

        if (!entry.occupied) {
            // Empty slot found
            return index;
        }

        if (!entry.deleted && entry.key && *entry.key == *key) {
            // Matching key
            return index;
        }

        index = (index + 1) % table.size();
        if (index == start) break; // Full cycle
    }

    return std::nullopt; // No valid slot
}
void Table::resize() {
    std::vector<Entry> old_table = std::move(table);
    table = std::vector<Entry>(old_table.size() * 2);
    size = 0;

    for ( Entry& entry : old_table) {
        if (entry.occupied && !entry.deleted) {
            insert(entry.key, (entry.value));
        }
    }
}
Table::Table(size_t capacity)
    : table(capacity), size(0) {
}
bool Table::insert(const std::shared_ptr<ObjString>& key, Value& value) {
    if (static_cast<double>(size) / table.size() >= max_load_factor) {
        resize();
    }

    auto slot_opt = find_slot(key);
    if (!slot_opt.has_value()) return false;

    size_t index = slot_opt.value();
    Entry& entry = table[index];

    if (!entry.occupied || entry.deleted) {
        size++;
    }

    entry.key = key;
    entry.value = std::move(value);
    entry.occupied = true;
    entry.deleted = false;
    return true;
}

Value* Table::find(const std::shared_ptr<ObjString>& key)  {
    auto slot_opt = find_slot(key);
    if (!slot_opt.has_value()) return nullptr;

    Entry& entry = table[slot_opt.value()];
    if (entry.occupied && !entry.deleted) {
        return &entry.value;
    } 

    return nullptr;
}
bool Table::remove(const std::shared_ptr<ObjString>& key) {
    auto slot_opt = find_slot(key);
    if (!slot_opt.has_value()) return false;

    Entry& entry = table[slot_opt.value()];
    if (entry.occupied && !entry.deleted) {
        entry.deleted = true;
        size--;
        return true;
    }

    return false;
}
std::shared_ptr<ObjString> Table::find_string(const std::string& s, uint32_t hash)const {
    for (const auto& entry : table) {
        if (entry.occupied && !entry.deleted && entry.key) {
            if (entry.key->get_hash() == hash && entry.key->get_string() == s) {
                return entry.key;
            }
        }
    }
    return nullptr;
}
