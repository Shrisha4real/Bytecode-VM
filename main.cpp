/***************************************************************
 * File: main.cpp
 * Description: reads the DSL either from the command line or the file
 ***************************************************************/

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

/*
* 
*/
void run_file(const string& filename);



/*
*  main is the start of the function
* it should take in none or a single argument
* if no arguement then, read from the cmd
* if 1 argument, the argument is the location of the file conaining a dsl
*/
int  main(int argc, const char* argv[]) {
	// take the argument from the cmd if no arguement , by running read_prompt() 
	if (argc == 1) {
		read_prompt();
	}
	// if  argument then, get the location of the dsl file and pass it as a parameter to the run_file() function
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


/*
 * Function: read_prompt
 * Purpose : function reads the DSL from the file (filename)
 */
void read_prompt() {

	std::string line;
	while (true) {
		std::cout << "> ";
		if ((!std::getline(std::cin, line))) break;

	}
}


/*
 * Function: run_file
 * Purpose : The function reads the code from the command prompt
 * Returns : InterpretResult - the result of program execution.
 */
void run_file(const string &filename) {
	/*
	* file is a ifstream type(fromthe ffstream library) that reads the file in binary ,mode
	* the file is read exactly as it is ,as specified in the file
	*/
	ifstream file(filename, ios::binary);
	if (!file) {
		cerr << "The file doesn't exist\n";
		exit(404);
	}
	/* buffer is of ostringstream type (of the sstream library) 
	reads the entire content of the file into an ostringstream buffer,
	which is then converted to a std::string code.
	*/
	ostringstream buffer;
	buffer << file.rdbuf();
	const std::string code = buffer.str();

	VM* vm = new VM();
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