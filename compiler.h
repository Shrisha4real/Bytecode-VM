#pragma once
#include <string>
#include <sstream>
#include<vector>
#include <cstddef>
#include<iostream>
#include<memory>
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

class Chuck;
class Parser;
class Scanner;
class ParseRule;
class StringInterner;

class Compiler {
public:
	  // or whatever signature you're using

	
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

	void number();
	//to compile numbers, 
	// converts the string into a double and pushes into the bytecode stack

	void grouping();
	// compile parenthesised experssion
	// considers that the ( is consumed, processes the reamining part of the expression
	// consume the ) in the end;
	void expression();
	void unary();
	void binary();
	void literal();
	void string();
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


private:
	inline void debug_print_code() {
		if (!this->parser->had_error) {
			Debug::disassemble_chuck(this->compiling_chunk, "code");
		}
	};
};

