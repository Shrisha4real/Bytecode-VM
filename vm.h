#pragma once
#include"chunk.h"
#include "common.h"

#include"debug.h"
#include"compiler.h"
#include<iostream>
#include<stack>

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} InterpretResult;
class VM
{
	Chunk* chunk;
	std::vector<uint8_t>::iterator ip;
	inline uint8_t read_byte() ;
	inline Value read_constant();
	std::vector<Value>stack;
	
public:
	VM(Chunk* chunk);

	
	InterpretResult run();
	inline void binary_op(char op);
	InterpretResult intepret(const std::string& source);
private:
	

};

