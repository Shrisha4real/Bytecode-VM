#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include "compiler.h"
#include <iostream>
#include<fstream>
#include <sstream>
#include<string>
using namespace std;

void read_prompt();
void run_file(const string& filename);

int  main(int argc, const char* argv[]) {
	if (argc == 1) {
		read_prompt();
	}
	else if(argc ==2 ){
		run_file(argv[1]);
	}

	/*Chunk* chunk = new Chunk;

	uint8_t constant1 = chunk->add_constant(1.2);
	chunk->write_chunk(OpCode::OP_CONSTANT , 123);
	chunk->write_chunk(constant1, 123);

	uint8_t constant2 = chunk->add_constant(1);
	chunk->write_chunk(OpCode::OP_CONSTANT, 124);
	chunk->write_chunk(constant2, 124);
	
	chunk->write_chunk(OpCode::OP_SUBTRACT, 123);


	chunk->write_chunk(OpCode::OP_NEGATE, 123);
	chunk->write_chunk(OpCode::OP_RETURN, 123);
	Debug::disassemble_chuck(chunk, "test chunk");
	VM* vm = new VM(chunk);
	vm->run();*/

	return 0;
}

void read_prompt() {
	std::string line;
	while (true) {
		std::cout << "> ";
		if ((!std::getline(std::cin, line))) break;

	}
}
void run_file(const string &filename) {
	ifstream file(filename, ios::binary);
	if (!file) {
		cerr << "The file doesn't exist\n";
		exit(404);
	}

	ostringstream buffer;
	buffer << file.rdbuf();
	const std::string code = buffer.str();
	Chunk* chunk = new Chunk;
	VM* vm = new VM(chunk);
	InterpretResult result =  vm->intepret(code);
	if (result == InterpretResult::INTERPRET_COMPILE_ERROR) {
		std::cout << "INTERPRET_COMPILE_ERROR\n";
		exit(65);
	}
	else if (result == InterpretResult::INTERPRET_RUNTIME_ERROR) {
		std::cout << "INTERPRET_RUNTIME_ERROR\n";
		exit(70);
	}
	else {
		std::cout << "successfull";
		exit(200);
	}
	file.close();
	std::cout << code;

}