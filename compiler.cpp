#include "compiler.h"
#include <functional> 
#include "chunk.h"
#include "Parser.h"
#include "common.h"
#include "Token.h"
#include "Scanner.h"
#include "vm.h"
#include "ParseRule.h"
#include "debug.h"
#include "Value.h"
#include"Object.h"
#include "StringInterner.h"
using namespace std::placeholders;


/*
 * Function: contructor
 * Purpose : creates a Scanner and a parser onjects and sets all the parsing rules got every keyword
 * Returns : InterpretResult - the result of program execution.
 */
Compiler::Compiler( const std::string& source, Chunk* chunk, std::shared_ptr<StringInterner>string_table) : source(source), compiling_chunk(chunk), rules(static_cast<size_t>(token_type::TOKEN_EOF) + 1), string_table(string_table) {
	scanner = new Scanner(this->source);
	parser = new Parser(this->scanner);
	current = std::make_shared<LocalCompiler>();
	rules[token_type::TOKEN_LEFT_PAREN] = {std::bind(&Compiler::grouping, this, _1), nullptr, Precedence::PREC_CALL};
	rules[token_type::TOKEN_RIGHT_PAREN] = { nullptr,  nullptr, Precedence::PREC_NONE};
	rules[token_type::TOKEN_LEFT_BRACE] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_RIGHT_BRACE] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_COMMA] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_DOT] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_MINUS] = { std::bind(&Compiler::unary, this, _1),   std::bind(&Compiler::binary, this, _1),  Precedence::PREC_TERM };
	rules[token_type::TOKEN_PLUS] = { nullptr,  std::bind(&Compiler::binary, this, _1),   Precedence::PREC_TERM };
	rules[token_type::TOKEN_SEMICOLON] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_SLASH] = { nullptr,  std::bind(&Compiler::binary, this, _1),  Precedence::PREC_FACTOR };
	rules[token_type::TOKEN_STAR] = { nullptr,  std::bind(&Compiler::binary, this, _1),  Precedence::PREC_FACTOR };
	rules[token_type::TOKEN_BANG] = { std::bind(&Compiler::unary,this, _1),  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_BANG_EQUAL] = { nullptr,  std::bind(&Compiler::binary, this, _1), Precedence::PREC_EQUALITY };
	rules[token_type::TOKEN_EQUAL] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_EQUAL_EQUAL] = { nullptr,  std::bind(&Compiler::binary, this, _1), Precedence::PREC_EQUALITY };
	rules[token_type::TOKEN_GREATER] = { nullptr,  std::bind(&Compiler::binary, this, _1), Precedence::PREC_EQUALITY };
	rules[token_type::TOKEN_GREATER_EQUAL] = { nullptr,  std::bind(&Compiler::binary, this, _1), Precedence::PREC_EQUALITY };
	rules[token_type::TOKEN_LESS] = { nullptr,  std::bind(&Compiler::binary, this, _1), Precedence::PREC_EQUALITY };
	rules[token_type::TOKEN_LESS_EQUAL] = { nullptr,  std::bind(&Compiler::binary, this, _1), Precedence::PREC_EQUALITY };
	rules[token_type::TOKEN_INCREMENT] = { nullptr, std::bind(&Compiler::variable, this, _1), Precedence::PREC_POSTFIX};
	rules[token_type::TOKEN_DECREMENT] = { nullptr, std::bind(&Compiler::variable, this, _1), Precedence::PREC_POSTFIX };
	rules[token_type::TOKEN_IDENTIFIER] = { std::bind(&Compiler::variable , this, _1),  nullptr, Precedence::PREC_NONE};
	rules[token_type::TOKEN_STRING] = { std::bind(&Compiler::string , this, _1),  nullptr, Precedence::PREC_NONE};
	rules[token_type::TOKEN_NUMBER] = { std::bind(&Compiler::number, this, _1),   nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_AND] = { nullptr,  std::bind(&Compiler::and_ , this, _1), Precedence::PREC_AND};
	rules[token_type::TOKEN_CLASS] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_ELSE] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_FALSE] = { std::bind(&Compiler::literal, this, _1),  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_FOR] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_FUN] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_IF] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_NIL] = { std::bind(&Compiler::literal, this, _1),  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_OR] = { nullptr,  std::bind(&Compiler::or_,this , _1), Precedence::PREC_OR};
	rules[token_type::TOKEN_PRINT] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_RETURN] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_SUPER] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_THIS] = { nullptr,  nullptr, Precedence::PREC_NONE };
	rules[token_type::TOKEN_TRUE] = { std::bind(&Compiler::literal, this, _1),  nullptr, Precedence::PREC_NONE };
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
	while (!match(token_type::TOKEN_EOF)) {
		declaration();
	}
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
void Compiler::number(bool can_assign) {
	std::stringstream ss(this->parser->previous.start);
	double value;
	ss >> value;
	this->emit_constant(Value::Number(value));
}

/*
 * Function: number
 * Purpose : runs a function that pushes a constant number into the VM
 */
void Compiler::emit_constant(Value&& value) {
	this->emit_bytes(OpCode::OP_CONSTANT, Compiler::make_constant(std::move(value)));

}

/*
 * Function: make_constant
 * Purpose : pushes a constant number into the VM
 */
uint8_t Compiler::make_constant(Value&& value) {
	int constant = this->compiling_chunk->add_constant(std::move(value));
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
	bool can_assign = precedence <= PREC_ASSIGNMENT;
 	prefix_rule(can_assign);
	while (precedence <= this->get_rule(this->parser->current.type)->precedence) {
		this->parser->advance();
		parse_fn infix_rule = this->get_rule(this->parser->previous.type)->infix;
		if (infix_rule == nullptr) {
			this->parser->error("expect an infix expression");
			return;
		}
		infix_rule(can_assign);
		if (can_assign && match(TOKEN_EQUAL)) {
			parser->error("Invalid assignment target.");
		}
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
void Compiler::postfix(bool can_assign) {
	//pop the top isntructions
	//
	
	variable(can_assign); // Or OP_SET_LOCAL depending on scope
}


void Compiler::unary(bool can_assign) {
	token_type operator_type = this->parser->previous.type;
	this->parse_precedence(Precedence::PREC_UNARY);
	switch (operator_type) {
	case token_type::TOKEN_BANG: this->emit_byte(OP_NOT); break;
	case(token_type::TOKEN_MINUS): {
		this->emit_byte(OpCode::OP_NEGATE);
		break;
	}

	default: return;
	}

}
void Compiler::binary(bool can_assign) {
	token_type operator_type = parser->previous.type;
	ParseRule* rule = this->get_rule(operator_type);
	this->parse_precedence(static_cast<Precedence>(rule->precedence + 1));
	switch (operator_type) {
	case token_type::TOKEN_BANG_EQUAL:    this->emit_bytes(OpCode::OP_EQUAL, OpCode::OP_NOT); break;
	case token_type::TOKEN_EQUAL_EQUAL:   this->emit_byte(OpCode::OP_EQUAL); break;
	case token_type::TOKEN_GREATER:       this->emit_byte(OpCode::OP_GREATER); break;

	case token_type::TOKEN_GREATER_EQUAL: this->emit_bytes(OpCode::OP_LESS, OpCode::OP_NOT); break;
	case token_type::TOKEN_LESS:          this->emit_byte(OpCode::OP_LESS); break;
	case token_type::TOKEN_LESS_EQUAL:    this->emit_bytes(OpCode::OP_GREATER, OpCode::OP_NOT); break;
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


void Compiler::literal(bool can_assign) {
	switch (parser->previous.type) {
	case token_type::TOKEN_FALSE: this->emit_byte(OpCode::OP_FALSE); break;
	case token_type::TOKEN_NIL: emit_byte(OpCode::OP_NIL); break;
	case token_type::TOKEN_TRUE: emit_byte(OpCode::OP_TRUE); break;
	default: return; // Unreachable.
	}
}
/*
 * Function: grouping
 * Purpose : executes paranthesized expressions
 */
void Compiler::grouping(bool can_assign) {
	Compiler::expression();
	this->parser->consume(token_type::TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

void Compiler::string(bool can_assign) {
	emit_constant(Value::Obj(std::shared_ptr<Object>{ string_table->copy_string(this->parser->previous.start + 1, this->parser->previous.length - 2) }));
}

ParseRule* Compiler::get_rule(token_type type) {
	return &this->rules[type];
}
void Compiler::declaration() {
	if (match(token_type::TOKEN_VAR)) {
		var_declaration();
	}
	else
	{
		statement();
	}
	if (this->parser->panic_mode) synchronise();

}

void Compiler::statement() {
	if (match(token_type::TOKEN_PRINT)){
		print_statement();
	}
	else if (match(token_type::TOKEN_LEFT_BRACE)) {
		begin_scope();
		block();
		end_scope();
	}
	else if (match(token_type::TOKEN_IF)) {
		if_statement();
	}
	else if (match(token_type::TOKEN_WHILE)) {
		while_statement();
	}
	else if (match(token_type::TOKEN_FOR)) {
		for_statement();
	}
	else {
 		expression_statement();
	}
}

bool Compiler::match(token_type type) {
	if (!check(type)) return false;
	this->parser->advance();
	return true;
}

bool Compiler::check(token_type type) const{
	return this->parser->current.type == type;
}

void Compiler::print_statement() {
	this->expression();
	this->parser->consume(TOKEN_SEMICOLON, "Expect ';' after value.");
	this->emit_byte(OpCode::OP_PRINT);

}
void Compiler::expression_statement() {
	expression();
	this->parser->consume(token_type::TOKEN_SEMICOLON, "Expect ';' after expression.");
	emit_byte(OP_POP);// add this in the opCode
}

void Compiler::synchronise() {
	parser->panic_mode = false;

	while (parser->current.type != token_type::TOKEN_EOF) {
		if (parser->previous.type == token_type::TOKEN_SEMICOLON) return;
		switch (parser->current.type) {
		case token_type::TOKEN_CLASS:
		case token_type::TOKEN_FUN:
		case token_type::TOKEN_VAR:
		case token_type::TOKEN_FOR:
		case token_type::TOKEN_IF:
		case token_type::TOKEN_WHILE:
		case token_type::TOKEN_PRINT:
		case token_type::TOKEN_RETURN:
			return;

		default:
			;
		}

		this->parser->advance();
	}
}



void Compiler::var_declaration() {

	uint8_t global = parse_variable("Expect variable name.");
	if (match(token_type::TOKEN_EQUAL)) 
 		expression();
	else
		emit_byte(OpCode::OP_NIL);

	this->parser->consume(token_type::TOKEN_SEMICOLON,"Expect ';' after variable declaration.");
	define_variable(global);

}

uint8_t Compiler::parse_variable(std::string message) {

	this->parser->consume(token_type::TOKEN_IDENTIFIER, "variable declaration should start with var");
	declare_variable();
	if (current->scope_depth > 0) return 0;
	return identifier_constant(parser->previous);

}

uint8_t Compiler::identifier_constant(Token& name) {
	auto str_obj = std::make_unique<ObjString>(name.start, name.length);
	return make_constant(Value::Obj(std::move(str_obj)));
}

void Compiler::define_variable(uint8_t global) {
	if (current->scope_depth > 0){
		mark_initialized();
		return;
	}
	emit_bytes(OpCode::OP_DEFINE_GLOBAL, global);
}
void Compiler::variable(bool can_assign) {
	named_variable(parser->previous, can_assign);
}
void Compiler::named_variable(Token name,bool can_assign) {
	uint8_t get_op, set_op;
	int arg = resolve_local(current, &name);
	if (arg != -1) {
		get_op = OpCode::OP_GET_LOCAL;
		set_op = OpCode::OP_SET_LOCAL;
	}
	else {
		arg = identifier_constant(name);
		get_op = OpCode::OP_GET_GLOBAL;
		set_op = OpCode::OP_SET_GLOBAL;
	}


	if (can_assign && match(token_type::TOKEN_EQUAL)) {
		expression();
		emit_bytes(set_op, arg);
	}
	else
		emit_bytes(get_op, arg);
}
void Compiler::block() {
	while (!check(token_type::TOKEN_RIGHT_BRACE) && !check(token_type::TOKEN_EOF)) {
		declaration();
	}
	parser->consume(token_type::TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}
void Compiler::begin_scope() {
	current->scope_depth++;
}

void Compiler::end_scope() {
	current->scope_depth--;
	while (current->local_count > 0 && current->locals[current->local_count - 1].depth > current->scope_depth) {
		emit_byte(OpCode::OP_POP);
		current->local_count--;
	}
}
void Compiler::declare_variable() {
	if (current->scope_depth == 0) return;
	Token* name = &parser->previous;
 	for (int i = current->local_count - 1; i >= 0; i--) {
		Local* local = &current->locals[i];
		if (local->depth != (-1) && local->depth < current->scope_depth) {
			break;
		}
		if (identifier_equal(name, &local->name)) {
			parser->error("Already a variable with this name in this scope.");

		}
	}
	add_local(name);
}
//FIXME the locals array is uninitialized

LocalCompiler::LocalCompiler() : local_count(0), scope_depth(0), locals() {};




void Compiler::add_local(Token name) {
	if (current->local_count == UINT8_COUNT) {
		parser->error("Too many local variables in function.");
		return;
	}
	
	current->locals[current->local_count].name = name;
	current->locals[current->local_count].depth = -1;
	current->local_count++;
}

bool Compiler::identifier_equal(Token* a, Token* b) {
	if (a->length != b->length) return false;
	return strncmp(a->start ,b->start, a->length )==0 ? true : false;
}
int Compiler::resolve_local(std::shared_ptr<LocalCompiler>compiler, Token* name) {
	for (int i = compiler->local_count - 1; i >= 0; i--) {
		Local* local = &compiler->locals[i];
		if (identifier_equal(name, &local->name)) {
			if (local->depth == -1) {
				parser->error("Can't read local variable in its own initializer.");
			}
			return i;
		}

	}
	return -1;
}

void Compiler::mark_initialized() {
	current->locals[current->local_count-1].depth = current->scope_depth;
}

void Compiler::if_statement() {
	parser->consume(token_type::TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
	expression();
	parser->consume(token_type::TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
	int then_jump = emit_jump(OpCode::OP_JUMP_IF_FALSE);
	emit_byte(OpCode::OP_POP);
	statement();
	int else_jump = emit_jump(OpCode::OP_JUMP);
	patch_jump(then_jump);
	emit_byte(OpCode::OP_POP);
	if (match(token_type::TOKEN_ELSE))	statement();
	patch_jump(else_jump);
}

void Compiler::and_(bool can_assign) {
	int end_jump = emit_jump(OpCode::OP_JUMP_IF_FALSE);
	emit_byte(OpCode::OP_POP);
	parse_precedence(Precedence::PREC_AND);
	patch_jump(end_jump);

}
void Compiler::or_(bool can_assign) {
	int else_jump =emit_jump(OpCode::OP_JUMP_IF_FALSE);
	int end_jump = emit_jump(OpCode::OP_JUMP);
	patch_jump(else_jump);
	emit_byte(OpCode::OP_POP);
	parse_precedence(Precedence::PREC_OR);
	patch_jump(end_jump);	
}

void Compiler::while_statement() {
	int loop_start = compiling_chunk->code.size();
	parser->consume(token_type::TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
	expression();
	parser->consume(token_type::TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
	int then_jump = emit_jump(OpCode::OP_JUMP_IF_FALSE);
	emit_byte(OpCode::OP_POP);
	statement();
	emit_loop(loop_start);
	patch_jump(then_jump);
	emit_byte(OpCode::OP_POP);
	

}
void Compiler::for_statement() {
	begin_scope();
	parser->consume(token_type::TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
	if (match(token_type::TOKEN_SEMICOLON)) {
		//do nothing
	}
	else if (match(token_type::TOKEN_VAR)) {
		var_declaration();
	}
	else 
		expression_statement();

	int loop_start = compiling_chunk->code.size();
	int exit_jump = -1;
	if (!match(token_type::TOKEN_SEMICOLON)) {
		expression();
		parser->consume(token_type::TOKEN_SEMICOLON, "Expect ';'.");
		exit_jump = emit_jump(OpCode::OP_JUMP_IF_FALSE);
		emit_byte(OpCode::OP_POP);
	}
	if (!match(token_type::TOKEN_RIGHT_PAREN)) {
		int body_jump = emit_jump(OP_JUMP);
		int increment_start = compiling_chunk->code.size();
		expression();
		emit_byte(OP_POP);
		parser->consume(token_type::TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

		emit_loop(loop_start);
		loop_start = increment_start;
		patch_jump(body_jump);
	}
	statement();


	emit_loop(loop_start);
	if (exit_jump != -1) {
		patch_jump(exit_jump);
		emit_byte(OpCode::OP_POP);
	}
	end_scope();
}

int Compiler::emit_jump(uint8_t instruction) {
	emit_byte(instruction);
	emit_byte(0xff);
	emit_byte(0xff);
	return compiling_chunk->code.size() - 2;

}

void Compiler::patch_jump(int offset) {
	int jump = compiling_chunk->code.size() - offset - 2;
	if (jump > UINT16_MAX) parser->error("Too much code to jump over");
	compiling_chunk->code[offset] = (jump >> 8) & 0xff;
	compiling_chunk->code[offset+1] = jump & 0xff;

}

void Compiler::emit_loop(int loop_start) {
	emit_byte(OpCode::OP_LOOP);
	int offset = compiling_chunk->code.size() - loop_start + 2;
	if (offset > UINT16_MAX) parser->error("Loop body too largre");
	emit_byte(static_cast<uint8_t>(offset >> 8 & 0xff));
	emit_byte(static_cast<uint8_t>(offset & 0xff));

}