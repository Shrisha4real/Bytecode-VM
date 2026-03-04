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
#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include<string>
#include"StringInterner.h"


#define FRAMES_MAX 64

namespace py = pybind11;
// Keep this at file scope (or as a static member of VM).
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
	py::module_ pandas;
	py::module_ sklearn;
	py::scoped_interpreter guard;
	int frame_count;//current height of the CallFrame stack; == frame.sixe()
	//comment out chunk and its ip
	// if we are commenting its ip then should there be an ip in the ObjFunction?
	/*std::shared_ptr<Chunk> chunk;
	std::vector<uint8_t>::iterator ip;*/
	inline uint8_t read_byte(CallFrame* frame) ;
	inline Value& read_constant(CallFrame* frame);
	inline uint16_t read_short(CallFrame* frame);
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
	std::shared_ptr<ObjString> read_string(CallFrame* frame);
	bool call_value(Value& callee, int arg_count);
	bool call(std::shared_ptr<ObjFunction> function, int arg_count);
	void define_native(const std::string& name, NativeFn function);
	Value clock_native(int arg_count, int args);
	Value load_native(int arg_count, int stack_index);
	Value clean_native(int arg_count, int stack_index);
	std::pair<Value, Value> split_method(int arg_count, int stack_index);
	Value predict_method(int arg_count , int stack_index);
	Value train_method(int arg_count, int stack_index);
	Value accuracy_method(int arg_count , int stack_index);
	py::object import_model_from_registry(const std::string& model_name);
};

 
