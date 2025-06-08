#pragma once
#include <iostream>
#include "Token.h"
#include "Scanner.h"
class Parser
{
public:
	Token current;
	Token previous;
	bool had_error;
	bool panic_mode;
	Scanner* scanner;
	Parser(Scanner* scanner);
	void advance();// reads the tokens from the code either untill an error is reached or the end of code is reached
	void error_at_current(const std::string message);//throw an error
	void error(const std::string message);//throw an error
	void error_at(Token* token, const std::string message);// throw an error at a particual point
	void consume(token_type type, const std::string message);//checks if the consumed token is the one requrired , if yes, then return, if no, return an error
	//void expression();
};

