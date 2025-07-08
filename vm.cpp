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
		
		std::cout << "run() stackprint\t";
		for (auto it = stack.begin(); it != stack.end(); it++) {
			std::cout << "[ ";
			std::visit([](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, std::monostate>) {
					std::cout << "nil";
				}
				else if constexpr (std::is_same_v< T,std::unique_ptr<Object>>) {
					std::cout << "object:\t";
					arg->print();
				}
				else {
					std::cout << arg;
				}
				}, it->data);

			std::cout << " ]\t";
		}
		std::cout << std::endl;
		Debug::disassemble_instruction(this->chunk, static_cast<int>(ip - ((this->chunk)->code).begin()));
	
		uint8_t instruction;
		switch (instruction = read_byte()) {

		case OpCode::OP_CONSTANT: {
			Value constant = read_constant();
			stack.push_back(std::move(constant));
			std::cout << "run() -> case:OP_CONSTANT ";

			std::visit([](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, std::monostate>) {
					std::cout << "nil";
				}
				else if constexpr (std::is_same_v< T, std::unique_ptr<Object>>) {

					arg->print();
				}
				else {
					std::cout << arg;
				}
				}, stack.back().data);
			std::cout << std::endl;

			break;
		}		
		case OpCode::OP_NEGATE:{
			auto b = std::move(stack.back()).as_number();
			stack.pop_back();
			stack.push_back(Value::Number(-b));
			break;
		}
		case OpCode::OP_ADD: {
			//binary_op('+'); 
			if (Value::is_number(this->peek(0)) && Value::is_number(this->peek(1))) {
				
				double b = std::move(stack.back()).as_number();
				double a = std::move(stack.at(stack.size() - 2)).as_number();
				stack.pop_back();
				stack.pop_back();
				stack.push_back(Value::Number(a + b));
			
			}
			else if (Value::is_string(this->peek(0)) && Value::is_string(this->peek(1))) {
				concatinate();
			}
			else {
				runtimeError("Operands must be numbers.");
				return InterpretResult::INTERPRET_RUNTIME_ERROR;

			}
			
			break;

		};
		case OpCode::OP_SUBTRACT: binary_op('-'); break;
		case OpCode::OP_MULTIPLY: binary_op('*'); break;		
		case OpCode::OP_DIVIDE: binary_op('/'); break;
		case OpCode::OP_GREATER:  binary_op('>'); break;
		case OpCode::OP_LESS:     binary_op('<'); break;
		case OpCode::OP_NIL: stack.push_back(Value::Nil()); break;
		case OpCode::OP_FALSE: stack.push_back(Value::Bool(false)); break;
		case OpCode::OP_TRUE: stack.push_back(Value::Bool(true)); break;
		
		case OpCode::OP_EQUAL: {
			Value b = std::move(stack.back());
			stack.pop_back();
			Value a = std::move(stack.back());
			stack.pop_back();
			stack.push_back(Value::Bool(Value::valuesEqual(a, b)));
			break;
		}

		case OpCode::OP_NOT:
		{
			Value back = std::move(stack.back());
			stack.pop_back();
			stack.push_back(Value::Bool(is_falsey(back)));
			break;
		}
		//FIXME: if the back of the stack is a bool like true then this doesnt work
		case OpCode::OP_RETURN: {
			Value top = std::move(stack.back());
			stack.pop_back();
			std::cout << "top = ";
			std::visit([](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, std::monostate>) {
					std::cout << "nil";
				}
				else if constexpr (std::is_same_v< T, std::unique_ptr<Object>>) {
					std::cout << "object:\t";
					arg->print();
				}
				else {
					std::cout << arg;
				}
				}, top.data);
			std::cout << std::endl;
			return InterpretResult::INTERPRET_OK;
		}

			
		}
	}
	std::cout << "return run()\n\n";
}
Value VM::read_constant() {
	return std::move((chunk->values).at(VM::read_byte()));
}
void VM::binary_op(char op) {
	if (!Value::is_number(this->peek(0)) || !Value::is_number(this->peek(1))) {
		runtimeError("Operands must be numbers.");
		return;
		//FIXME: should return INTERPRET_RUNTIME_ERROR
	}
	Value b_val = std::move(stack.back());
	Value a_val = std::move(stack.at(stack.size() - 2));
	double b = b_val.as_number();
	double a = a_val.as_number();
	stack.pop_back();
	stack.pop_back();
	switch (op) {		
	case '+': {
				stack.push_back(Value::Number(a + b));
		break;
	}
	case '-': {
		stack.push_back(Value::Number(a - b));
		break;
	}
	case '*': {
		stack.push_back(Value::Number(a * b));
		break;
	}
	case '/': {
		if (b != 0) {
			stack.push_back(Value::Number(a / b));
		}
		else {
			std::cerr << "Division by zero!\n";
		}
		break;
	}
	case'>':
		stack.push_back(Value::Bool(a > b));
		break;
	case'<':
		stack.push_back(Value::Bool(a > b));
		break;
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
void VM::runtimeError(const std::string& message) {
	std::cerr << message << "\n";

	size_t instruction = std::distance((this->chunk->code).begin(), this->ip) - 1;
	int line = this->chunk->lines[instruction];
	std::cerr << "[line " << line << "] in script\n";
	this->stack.clear();
}


Value& VM::peek(int distance) {
	if (distance > this->stack.size() - 1) {
		std::cerr << "Unreachable peek\n";
	}
	return this->stack.at(this->stack.size() - 1 - distance);
}

//CHECK IF CONDITION FOR ALL CASES
bool VM::is_falsey(Value& value) {
	return (Value::is_nil(value) ||Value::is_bool(value));
}

//FIXME: The ObjString pointer is pointing to the unique_ptr
void VM::concatinate() {
	std::unique_ptr<Object> b = std::move(stack.back()).transfer_obj();
	ObjString* b_ptr = dynamic_cast<ObjString*>(b.get());
	if (!b_ptr) {
		std::cout << "the object isn't of type string\n";
		return;
	}
	stack.pop_back();

	std::unique_ptr<Object> a = std::move(stack.back()).transfer_obj();
	ObjString* a_ptr = dynamic_cast<ObjString*>(a.get());
	if (!a_ptr) {
		std::cout << "the object isn't of type string\n";
		return;
	}
	stack.pop_back();

	// Modify a with b
	(*a_ptr) += (*b_ptr);

	// Push back the modified a
	stack.push_back(Value::Obj(std::move(a)));
}
