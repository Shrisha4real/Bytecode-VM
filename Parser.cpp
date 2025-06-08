#include "Parser.h"

Parser::Parser(Scanner* scanner) : had_error(false) , panic_mode(false) , current(), previous() , scanner(scanner) {
    
};

void Parser::advance() {
	this->previous = this->current;
	while (true) {
        this->current = (this->scanner)->scan_token();
		if (this->current.type == token_type::TOKEN_ERROR) break;
		this->error_at_current(this->current.start);

	}
}

void Parser::error_at_current(const std::string message) {
	this->error_at(&(this->current), message);

}

void Parser::error(const std::string message) {
	this->error_at(&(this->previous), message);

}

void Parser::error_at(Token* token, const std::string message) {
    if (this->panic_mode) return;
    this->panic_mode =true;
    std::cerr << "[line " << token->line << "] Error";


    if (token->type == TOKEN_EOF) {
        std::cerr << " at end";
    }
    else if (token->type == TOKEN_ERROR) {
        // Nothing.
    }
    else {
        // Using std::string to safely handle potentially non-null-terminated 'start'
        // and print a substring.
        std::cerr << " at '" << std::string(token->start, token->length) << "'";
    }

    std::cerr << ": " << message << std::endl; // std::endl adds a newline and flushes the buffer
    this->had_error = true;
}


void Parser::consume(token_type type, const std::string message) {
    if (this->current.type == type) {
        this->advance();
        return;
    }
    this->error_at_current(message);
}