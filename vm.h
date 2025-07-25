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
#include<unordered_map>
#include"StringInterner.h"

class StringInterner;
class Chunk;
class ObjString;


class VM
{
	Chunk* chunk;
	std::vector<uint8_t>::iterator ip;
	inline uint8_t read_byte() ;
	inline Value& read_constant();
	inline uint16_t read_short();
	std::vector<Value>stack;
	std::shared_ptr<StringInterner> strings;
	std::shared_ptr<StringInterner> globals;
public:
	VM();	
	InterpretResult run();
	inline void binary_op(char op);
	InterpretResult intepret(const std::string& source);
	void runtimeError(const std::string& message);
	Value& peek(int distance);
	void concatinate();
	bool is_falsey(Value& value);
	Value pop();
	std::shared_ptr<ObjString> read_string();
	

private:
	

};

