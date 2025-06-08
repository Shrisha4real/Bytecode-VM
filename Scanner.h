#pragma once
#include<string>
#include <cctype>
#include "Token.h"
#include "common.h"

class Scanner {
    
public:
    std::string_view source;
    const char* start;
    const char* current;
    int line;
    Scanner(const std::string& source);
    Token scan_token();
    bool is_at_end() const;
    friend class Token;
    char advance();
    bool match(char expected);
    void remove_white_spaces();
    char peek();
    char peek_next();
    Token string();
    Token number();
    Token identifier();
    token_type identifier_type();
    Token make_token(token_type type);
    Token error_token(const char* message);

    token_type check_keyword(int start, int length, const char* rest, token_type type);
};
