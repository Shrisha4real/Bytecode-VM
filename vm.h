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
#include<array>
#include"StringInterner.h"
#define FRAMES_MAX 64

class StringInterner;
class Chunk;
class ObjString;
class ObjFunction;

class CallFrame {
public:
	std::shared_ptr<ObjFunction> function;
	std::vector<uint8_t>::iterator ip;
	int slot_base;

	CallFrame();
	CallFrame(std::shared_ptr<ObjFunction> func,std::vector<uint8_t>::iterator ip_iter, int slot_base);

	//std::shared_ptr<Value> slots;
	//what is the constructor for this?
};
class VM
{
	
	
	int frame_count;//current height of the CallFrame stack; == frame.sixe()
	//comment out chunk and its ip
	// if we are commenting its ip then should there be an ip in the ObjFunction?
	/*std::shared_ptr<Chunk> chunk;
	std::vector<uint8_t>::iterator ip;*/
	inline uint8_t read_byte(CallFrame& frame) ;
	inline Value& read_constant(CallFrame& frame);
	inline uint16_t read_short(CallFrame& frame);
	std::array<CallFrame, FRAMES_MAX > frames;
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
	std::shared_ptr<ObjString> read_string(CallFrame& frame);
	bool call_value(Value& callee, int arg_count);
	bool call(std::shared_ptr<ObjFunction> function, int arg_count);

private:	

};

