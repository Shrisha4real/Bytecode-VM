#pragma once
#include <string>
#include <sstream>
#include<vector>
#include <cstddef>
#include<iostream>
#include<memory>
#include"common.h"
#include"debug.h"
#include"Parser.h"
#include <array>
#include <cstring> // for strcmp



class Chunk;
class Parser;
class Scanner;
class ParseRule;
class StringInterner;
class Value;
class LocalCompiler;
class Local;
class Token;

constexpr int UINT8_COUNT = UINT8_MAX + 1;
class Local {
	
public:
	Token name;
	int depth;
	Local() : name(Token{}), depth(0) {};
	Local(Token name) : name(name), depth(0) {};
};

class LocalCompiler {
	
	
public:
	std::array<Local, UINT8_COUNT> locals;
	int local_count;
	int scope_depth;
	LocalCompiler();

};


class Compiler {
public:
	// or whatever signature you're using

	std::shared_ptr<LocalCompiler> current;
	const std::string& source;
	Chunk* compiling_chunk;
	Parser* parser;
	Scanner* scanner;
	std::vector<ParseRule> rules;
	std::shared_ptr<StringInterner>string_table;

	Compiler(const std::string& source, Chunk* chunk, std::shared_ptr<StringInterner>string_table);
	// constructor


	bool compile();
	void emit_byte(uint8_t byte);
	//the function appends a single byte into the chunk

	void emit_bytes(uint8_t byte1, uint8_t byte2);


	//Chunk* current_chunk();
	// returns compiling_chunk

	void end_compiler();
	// rerturns emit_return()

	void emit_return();
	// returns the enum OP_RETURN from "common.h" 

	void number(bool can_assign);
	void grouping(bool can_assign);;
	void expression();
	void unary(bool can_assign);
	void binary(bool can_assign);
	void literal(bool can_assign);
	void string(bool can_assign);
	void parse_precedence(Precedence precedence);
	void emit_constant(Value&& value);
	uint8_t make_constant(Value&& value);
	ParseRule* get_rule(token_type type);
	void declaration();
	void statement();
	bool match(token_type type);
	bool check(token_type type) const;
	void print_statement();
	void expression_statement();
	void synchronise();
	void var_declaration();
	uint8_t parse_variable(std::string message);
	uint8_t identifier_constant(Token& name);
	void define_variable(uint8_t global);
	void variable(bool can_assign);
	void named_variable(Token name, bool can_assign);
	inline void begin_scope();
	void block();
	void end_scope();
	void declare_variable();
	void add_local(Token name);
	bool identifier_equal(Token* a, Token* b);
	void mark_initialized();
	int resolve_local(std::shared_ptr<LocalCompiler> local_compiler, Token* name);
	void if_statement();
	void and_(bool can_assign);
	void or_(bool can_assign);
	int emit_jump(uint8_t instructions);
	void patch_jump(int offset);
private:
	inline void debug_print_code() {
		if (!this->parser->had_error) {
			Debug::disassemble_chuck(this->compiling_chunk, "code");
		}
	}
};



