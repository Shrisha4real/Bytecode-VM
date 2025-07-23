/***************************************************************
 * File: vm.cpp
 * Description: Implements the Virtual Machine execution logic
 *              for the Bytecode VM project.
 ***************************************************************/

#include "vm.h"
#include"StringInterner.h"
#include "Value.h"
#include"Table.h"
#include"Object.h"
#include"ParseRule.h"

VM::VM() :chunk(new Chunk()) {
	ip = (this->chunk->code).begin();
	strings = std::make_shared<StringInterner>();
	globals = std::make_shared<StringInterner>();
};


uint8_t VM::read_byte() {
	return *((this->ip)++);
}

InterpretResult VM::run() {
	std::cout << "\nexecuting run()\n";
	while (true) {
		
		//std::cout << "run() stackprint\t";
		
		/*for (auto it = stack.begin(); it != stack.end(); it++) {
			std::cout << "[ ";
			std::visit([](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, std::monostate>) {
					std::cout << "nil";
				}
				else if constexpr (std::is_same_v< T,std::shared_ptr<Object>>) {
					std::cout << "object:\t";
					arg->print();
				}
				else {
					std::cout << arg;
				}
				}, it->data);

			std::cout << " ]\t";
		}*/
		//std::cout << std::endl;
 		Debug::disassemble_instruction(this->chunk, static_cast<int>(ip - ((this->chunk)->code).begin()));
	
		uint8_t instruction;
		switch (instruction = read_byte()) {

		case OpCode::OP_CONSTANT: {
			Value& constant = read_constant();
			stack.push_back(std::move(constant));
  			std::cout << "run() -> case:OP_CONSTANT ";

			std::visit([](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, std::monostate>) {
					std::cout << "nil";
				}
				else if constexpr (std::is_same_v< T, std::shared_ptr<Object>>) {

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

		case OpCode::OP_PRINT: {
			Value::print_value(pop());
			//std::cout << std::endl;
			break;
		}
		case OpCode::OP_POP: {
			Value popped = pop();
			break; 
		}
		case OpCode::OP_GET_LOCAL: {
			uint8_t slot = read_byte();
			stack.push_back(stack.at(slot).clone());
			break;
		}
		case OpCode::OP_SET_LOCAL: {
			stack.at(read_byte()).set(peek(0));

		}
		case OpCode::OP_DEFINE_GLOBAL: {

			std::shared_ptr<ObjString>  name = read_string();
			globals->get_table()->insert(name , peek(0));
			pop();
			break;
		}
		case OpCode::OP_GET_GLOBAL: {
			std::shared_ptr<ObjString>  name = read_string();
			Value* value = globals->get_table()->find(name);
			if (!value) {
				runtimeError("undefined variable -> " + name->get_string());
				return InterpretResult::INTERPRET_RUNTIME_ERROR;
			}
			stack.push_back(value->clone());
			break;
		}
		//FIXME: if the back of the stack is a bool like true then this doesnt work
		case OpCode::OP_SET_GLOBAL: {
			std::shared_ptr<ObjString>  name = read_string();
			if (globals->get_table()->insert(name, peek(0))) {
				globals->get_table()->remove(name);
				runtimeError("undefined variable -> " + name->get_string());
				return InterpretResult::INTERPRET_RUNTIME_ERROR;
			}
			break;
		}
		case::OpCode::OP_JUMP_IF_FALSE: {
			uint16_t offset = read_short();
			if (is_falsey(peek(0))) ip += offset;
			break;
		}
		case::OpCode::OP_JUMP: {
			uint16_t offset = read_short();
			ip += offset;
			break;
		}
		case OpCode::OP_RETURN: {
			/*if (stack.empty()) {
				return InterpretResult::INTERPRET_RUNTIME_ERROR;
			}
			Value top = std::move(stack.back());
			stack.pop_back();
			std::cout << "top = ";
			std::visit([](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, std::monostate>) {
					std::cout << "nil";
				}
				else if constexpr (std::is_same_v< T, std::shared_ptr<Object>>) {
					std::cout << "object:\t";
					arg->print();
				}
				else {
					std::cout << arg;
				}
				}, top.data);*/
			std::cout << std::endl;
			return InterpretResult::INTERPRET_OK;
		}

			
		}
	}
	std::cout << "return run()\n\n";
}

 Value& VM::read_constant() {
	return (chunk->values).at(read_byte());
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
			runtimeError("Division by zero!\n");
			return;
			/*std::cerr << "Division by zero!\n";*/
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

	Compiler compiler(source , this->chunk,this->strings);


	
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
	return std::visit([&](auto&& arg) -> bool {
		using T = std::decay_t<decltype(arg)>;

		// nil is falsey
		if constexpr (std::is_same_v<T, std::monostate>) {
			return true;
		}
		// Boolean false is falsey
		else if constexpr (std::is_same_v<T, bool>) {
			return arg == false;
		}
		// Handle objects like strings
		else if constexpr (std::is_same_v<T, std::shared_ptr<Object>>) {
			if (arg && arg->obj_type() == ObjType::OBJ_STRING) {
				auto strObj = std::static_pointer_cast<ObjString>(arg);
				return strObj->get_string().empty();
			}
			return false;
		}
		else if constexpr (std::is_same_v < T, double >) {
			return arg == static_cast<double>(0);
		}
		else {
			return false;
		}
		}, value.data);
}

//FIXME: The ObjString pointer is pointing to the shared_ptr
void VM::concatinate() {
	// Get the top two values on the stack
	std::shared_ptr<ObjString> b = Value::as_string(stack.back());
	if (!b) {
		std::cerr << "Right operand is not a string\n";
		return;
	}
	stack.pop_back();

	std::shared_ptr<ObjString> a = Value::as_string(stack.back());
	if (!a) {
		std::cerr << "Left operand is not a string\n";
		return;
	}
	stack.pop_back();

	// Create a new ObjString with the concatenated result
	std::string combined = a->get_string() + b->get_string();  // Combine strings
	std::shared_ptr<ObjString> result = std::make_shared<ObjString>(combined);

	// Push the result back onto the stack
	stack.push_back(Value::Obj(result));
}

Value VM::pop() {
	Value val = std::move(stack.back());
	stack.pop_back();
	return val;
}

std::shared_ptr<ObjString>  VM::read_string() {
	Value& top_value = read_constant(); 
	if (Value::is_string(top_value)) {
		return Value::as_string(top_value); 
	}
	std::cerr << "Value is not a string.\n";
	return nullptr;
 }

uint16_t VM::read_short() {
	ip += 2;
	return static_cast<uint16_t>(*(ip - 2) << 8 | *(ip - 1));

}