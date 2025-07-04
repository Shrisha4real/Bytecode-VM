#pragma once
#include"chunk.h"
#include "common.h"
#include <variant>
#include"debug.h"
#include"compiler.h"
#include<iostream>
#include<stack>
#include <format>
#include<algorithm>
#include<memory>
#include<utility>
#include "Value.h"
class VM
{
	Chunk* chunk;
	std::vector<uint8_t>::iterator ip;
	inline uint8_t read_byte() ;
	inline Value read_constant();
	std::vector<Value>stack;
	
public:
	VM();	
	InterpretResult run();
	inline void binary_op(char op);
	InterpretResult intepret(const std::string& source);
	void runtimeError(const std::string& message);
	Value& peek(int distance);
	void concatinate();
	bool is_falsey(Value& value);
private:
	

};

