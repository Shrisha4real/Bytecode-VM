/***************************************************************
 * File: vm.cpp
 * Description: Implements the Virtual Machine execution logic
 *              for the Bytecode VM project.
 ***************************************************************/

#include "vm.h"


VM::VM():chunk(new Chunk()){
	ip = (this->chunk->code).begin();
};


uint8_t VM::read_byte() {
	return *((this->ip)++);
}

InterpretResult VM::run() {
	std::cout << "\nexecuting run()\n";
	while (true) {
		
		//std::cout << "run() stackprint\t";
		for (auto it = stack.begin(); it != stack.end(); it++) {
			std::cout << "[ " << *it << " ]\t";
		}
		std::cout << std::endl;
		Debug::disassemble_instruction(this->chunk, static_cast<int>(ip - ((this->chunk)->code).begin()));
	
		uint8_t instruction;
		switch (instruction = read_byte()) {

		case OpCode::OP_CONSTANT: {
			Value constant = read_constant();
			stack.push_back(constant);
			std::cout <<"run() -> case:OP_CONSTANT "<< constant << std::endl;
			break;
		}		

		case OpCode::OP_NEGATE:{
			Value top = stack.back();
			stack.pop_back();
			stack.push_back(-top);
			break;
		}
		case OpCode::OP_ADD: binary_op('+'); break;
		case OpCode::OP_SUBTRACT: binary_op('-'); break;
		case OpCode::OP_MULTIPLY: binary_op('*'); break;
		case OpCode::OP_DIVIDE: binary_op('/'); break;


		case OpCode::OP_RETURN:
			Value top = stack.back();			
			stack.pop_back(); 

			std::cout <<"top= " <<top << std::endl;
			return InterpretResult::INTERPRET_OK;
		}
	}
	std::cout << "return run()\n\n";
}
Value VM::read_constant() {
	return (chunk->values).at(VM::read_byte());
}
void VM::binary_op(char op) {
	Value b = stack.back();
	Value a = stack.at(stack.size() - 2);
	switch (op) {		
	case '+': {
		stack.pop_back();
		stack.pop_back();
		stack.push_back(a + b);
		break;
	}
	case '-': {
		stack.pop_back();
		stack.pop_back();
		stack.push_back(a - b);
		break;
	}
	case '*': {
		stack.pop_back();
		stack.pop_back();
		stack.push_back(a * b);
		break;
	}
	case '/': {
		if (b != 0) {
			stack.pop_back();
			stack.pop_back();
			stack.push_back(a / b);
		}
		else {
			std::cerr << "Division by zero!\n";
		}
		break;
	}
	default:
		std::cerr << "Invalid operator!\n";
		break;
	}
}

/*
 * Function: interpret
 * Purpose : interpretes the DSL
 * Returns : the status of interpretation
 */
InterpretResult VM::intepret(const std::string& source) {

	Compiler compiler(source , this->chunk);


	
	if (!compiler.compile()) {
		delete chunk;
		return InterpretResult::INTERPRET_COMPILE_ERROR;

	}
	//this->chunk = chunk;
	this->ip = (chunk->code).begin();
	InterpretResult result = this->run();

	delete chunk;
	return result;

}
