#pragma once
#include<string>
#include<iostream>
#include <iomanip>
class Chunk;


class Debug {
public:
	static void disassemble_chuck(Chunk* chunk, std::string name);
	static int disassemble_instruction(Chunk* chunkk, int offset);
	static int simple_instruction(std::string name, int offset);
	static int constant_instruction(Chunk* chunk, std::string name, int offset);
	static int byte_instruction(Chunk* chunk, std::string name, uint8_t offset);
	static int jump_instruction(Chunk* chunk, std::string name,int sign, int offset);
};



