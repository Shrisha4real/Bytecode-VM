#include "compiler.h"
#include <functional> 

/*
 * Function: contructor
 * Purpose : creates a Scanner and a parser onjects and sets all the parsing rules got every keyword
 * Returns : InterpretResult - the result of program execution.
 */
Compiler::Compiler(const std::string& source, Chunk* chunk) : source(source), compiling_chunk(chunk), rules(static_cast<size_t>(token_type::TOKEN_EOF) + 1) {
	scanner = new Scanner(this->source);
	parser = new Parser(this->scanner);
	rules[token_type::TOKEN_LEFT_PAREN] = {std::bind(&Compiler::grouping, this), nullptr, Precedence::PREC_CALL };
	rules[token_type::TOKEN_RIGHT_PAREN] = { nullptr,  nullptr, Precedence::PREC_NONE};
	rules[token_type::TOKEN_LEFT_BRACE] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_RIGHT_BRACE] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_COMMA] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_DOT] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_MINUS] = { std::bind(&Compiler::unary, this),   std::bind(&Compiler::binary, this),  Precedence::PREC_TERM };
	rules[token_type::TOKEN_PLUS] = { nullptr,  std::bind(&Compiler::binary, this),  Precedence::PREC_TERM };
	rules[token_type::TOKEN_SEMICOLON] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_SLASH] = { nullptr,  std::bind(&Compiler::binary, this),  Precedence::PREC_FACTOR };
	rules[token_type::TOKEN_STAR] = { nullptr,  std::bind(&Compiler::binary, this),  Precedence::PREC_FACTOR };
	rules[token_type::TOKEN_BANG] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_BANG_EQUAL] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_EQUAL] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_EQUAL_EQUAL] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_GREATER] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_GREATER_EQUAL] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_LESS] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_LESS_EQUAL] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_IDENTIFIER] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_STRING] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_NUMBER] = { std::bind(&Compiler::number, this),   nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_AND] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_CLASS] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_ELSE] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_FALSE] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_FOR] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_FUN] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_IF] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_NIL] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_OR] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_PRINT] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_RETURN] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_SUPER] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_THIS] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_TRUE] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_VAR] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_WHILE] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_ERROR] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_EOF] = { nullptr,  nullptr, Precedence::PREC_NONE };
};

/*
 * Function: compile
 * Purpose : compiles the given expression
 * Returns : true if the expression is valid else(or compilation is sucessful else return false.
 */
bool Compiler::compile() {

	this->parser->advance();//sets the first token as the current token to be parsed
	this->expression();
	this->parser->consume(token_type::TOKEN_EOF, "Expect end of an expression");
	this->end_compiler();
	return !this->parser->had_error;

}

/*
 * Function: emit_byte
 * Purpose : adds the instructions to the VM stack
 */
void Compiler::emit_byte(uint8_t byte) {
	this->compiling_chunk->write_chunk(byte, this->parser->previous.line);
}

/*
 * Function: emit_bytes
 * Purpose : adds 2 instructions to the VM stack at once
 */
void Compiler::emit_bytes(uint8_t byte1, uint8_t byte2) {
	this->emit_byte(byte1);
	this->emit_byte(byte2);
}

/*
 * Function: end_compiler
 * Purpose : executed when the expression is ivaluated
 */
void Compiler::end_compiler() {
	this->emit_return();
}


/*
 * Function: emit_return
 * Purpose : adds the return instructions into the VM code stack
 */
void Compiler::emit_return() {
	this->emit_byte(OpCode::OP_RETURN);
}

/*
 * Function: number
 * Purpose : evaluates and pushes a constant number into the VM
 */
void Compiler::number() {
	std::stringstream ss(this->parser->previous.start);
	double value;
	ss >> value;
	this->emit_constant(value);
}

/*
 * Function: number
 * Purpose : runs a function that pushes a constant number into the VM
 */
void Compiler::emit_constant(Value value) {
	this->emit_bytes(OpCode::OP_CONSTANT, Compiler::make_constant(value));

}

/*
 * Function: make_constant
 * Purpose : pushes a constant number into the VM
 */
uint8_t Compiler::make_constant(Value value) {
	int constant = this->compiling_chunk->add_constant(value);
	if (constant > UINT8_MAX) {
		this->parser->error("Too many constants in one chunk.");
		return 0;
	}
	return static_cast<uint8_t>(constant);
}



/*
 * Function: parse_precedence
 * Purpose : starts at the current token and parses any expression at the given precedence level or higher.
 */
void Compiler::parse_precedence(Precedence precedence) {
	this->parser->advance();
	parse_fn prefix_rule = this->get_rule(this->parser->previous.type)->prefix;
	if (prefix_rule == nullptr) {
		this->parser->error("expect an expression");
		return;
	}
	prefix_rule();
	while (precedence <= this->get_rule(this->parser->current.type)->precedence) {
		this->parser->advance();
		parse_fn infix_rule = this->get_rule(this->parser->previous.type)->infix;
		infix_rule();
	}
}

/*
 * Function: expression
 * Purpose : The function reads the code from the command prompt
 * Returns : InterpretResult - the result of program execution.
 */
void Compiler::expression() {
	this->parse_precedence(Precedence::PREC_ASSIGNMENT);
}

void Compiler::unary() {
	token_type operator_type = this->parser->previous.type;
	this->parse_precedence(Precedence::PREC_UNARY);
	switch (operator_type) {
	case(token_type::TOKEN_MINUS): {
		this->emit_byte(OpCode::OP_NEGATE);
		break;
	}
	default: return;
	}

}
void Compiler::binary() {
	token_type operator_type = parser->previous.type;
	ParseRule* rule = this->get_rule(operator_type);
	this->parse_precedence(static_cast<Precedence>(rule->precedence + 1));
	switch (operator_type) {
	case(token_type::TOKEN_PLUS): {
		this->emit_byte(OpCode::OP_ADD);
		break;
	}
	case(token_type::TOKEN_MINUS): {
		this->emit_byte(OpCode::OP_SUBTRACT);
		break;
	}
	case(token_type::TOKEN_STAR): {
		this->emit_byte(OpCode::OP_MULTIPLY);
		break;
	}
	case(token_type::TOKEN_SLASH): {
		this->emit_byte(OpCode::OP_DIVIDE);
		break;
	}
	default:return;
	}
 }

/*
 * Function: grouping
 * Purpose : executes paranthesized expressions
 */
void Compiler::grouping() {
	Compiler::expression();
	this->parser->consume(token_type::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

ParseRule* Compiler::get_rule(token_type type) {
	return &this->rules[type];
}
