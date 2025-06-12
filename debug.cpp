#include "debug.h"
#include "common.h"

void Debug::disassemble_chuck(const Chunk* chunk, std::string name)  {
	std::cout << "==\t" << name << "\t=="<<std::endl;
	for (int offset = 0; offset < (chunk->code).size();) {
		offset = disassemble_instruction(chunk, offset);
	}
}


int Debug::disassemble_instruction(const Chunk* chunk,  int offset) {
	std::cout << std::setw(4) << std::setfill('0') <<  offset << '\t';
	if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
		std::cout << "|\t";
	}
	else {
		std::cout << chunk->lines.at(offset)<<'\t';
	}
	uint8_t instruction = (chunk->code).at(offset);

	switch (instruction) {
	case OpCode::OP_RETURN:
		return simple_instruction("OP_RETURN", offset);
	case OpCode::OP_CONSTANT:
		return constant_instruction(chunk, "OP_CONSTANT", offset);
	case OpCode::OP_ADD:
		return simple_instruction("OP_ADD", offset);
	case OpCode::OP_SUBTRACT:
		return simple_instruction("OP_SUBTRACT", offset);
	case OpCode::OP_MULTIPLY:
		return simple_instruction("OP_MULTIPLY", offset);
	case OpCode::OP_DIVIDE:
		return simple_instruction("OP_DIVIDE", offset);
	case OpCode::OP_NEGATE: 
		return	simple_instruction("OP_NEGATE", offset);

	default:
		std::cout << "Unknown op code" << instruction << std::endl;
		return offset+1;
	}
}

int Debug::simple_instruction(std::string name, int offset) {
	std::cout << name << std::endl;
	return offset + 1;
}

int Debug::constant_instruction(const Chunk* chunk , std::string name,  int offset) {
	uint8_t constant = (chunk->code).at(offset + 1);
	std::cout << name << '\t' << static_cast<int>(constant) << '\t' << (chunk->values[constant]) << std::endl;
	return offset + 2;	
}