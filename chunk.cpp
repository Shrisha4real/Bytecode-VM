/***************************************************************
 * File: chunk.cpp
 * Description: Stores the instructions stack,and the constant values
 *				The stacks are initially empty
 ***************************************************************/

#include"chunk.h"
#include "common.h"
#include"Value.h"

size_t Chunk::add_constant( Value&& value) {
    if (Value::is_obj(value)) {
        std::shared_ptr<Object> obj = std::get<std::shared_ptr<Object>>(value.data);
        if (!obj) {
            std::cerr << "[Info] Constant added: " << Value::as_string(value) << "\n";
            std::cerr << "[Error] Trying to add null object to chunk constants!\n";
        }
    }
	(this->values).push_back(std::move(value));
	return ((this->values).size() - 1);

}  

void Chunk::write_chunk(uint8_t byte , int line){
	(this->code).push_back(byte); 
	(this->lines).push_back(line);
}

Chunk::Chunk(const Chunk& other)
    : code(other.code),
    lines(other.lines)
{
    values.reserve(other.values.size());
    for (const Value& v : other.values) {
        values.push_back(v.clone());   
    }
}
Chunk& Chunk::operator=(const Chunk& other)
{
    if (this == &other) return *this;

    code = other.code;
    lines = other.lines;

    values.clear();
    values.reserve(other.values.size());
    for (const Value& v : other.values) {
        values.push_back(v.clone());
    }

    return *this;
}
