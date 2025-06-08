#pragma once
#include "common.h"
#include<vector>
typedef double Value;


class Chunk {
public:
	std::vector<uint8_t>code;
	std::vector<Value>values;
	std::vector<int>lines;

	Chunk() :code({}), values({}), lines({}) {};

	size_t add_constant(Value value);
	void write_chunk(uint8_t byte , int line);
};

