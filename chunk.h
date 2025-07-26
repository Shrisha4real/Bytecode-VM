#pragma once
#include<vector>
#include "Value.h"
class Value;

class Chunk {
private:

public:

	std::vector<uint8_t>code;// sequence of the bytecode instructions as a bytes
	std::vector<Value>values;// the list of constants that are referenced by the above instructions
	std::vector<int>lines;//line number of the code in the file
		
	Chunk() = default;
	Chunk(const Chunk& other);
	Chunk& operator=(const Chunk& other);

	Chunk(Chunk&&) noexcept = default;
	Chunk& operator=(Chunk&&) noexcept = default;

	size_t add_constant(Value&& value);
	void write_chunk(uint8_t byte , int line);

};

