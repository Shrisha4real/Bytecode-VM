#pragma once
#include<string>
#include<iostream>
#include <iomanip>
class Chunk;

#include "common.h"

class Debug {
public:
	static void disassemble_chuck(const Chunk* chunk, std::string name) ;
	static int disassemble_instruction(const Chunk* chunk, int offset);
	static int simple_instruction(std::string name, int offset);
	static int constant_instruction(const Chunk* chunk, std::string name,  int offset);
	
};



