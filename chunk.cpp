#include"chunk.h"

size_t Chunk::add_constant( Value value) {
	(this->values).push_back(value);
	return ((this->values).size() - 1);

}

void Chunk::write_chunk(uint8_t byte , int line){
	(this->code).push_back(byte);
	(this->lines).push_back(line);
}