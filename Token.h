#pragma once
#include "common.h"


class Token {

public:
    token_type type;
    const char* start;
    int line;
    int length;
    Token() : type(token_type::TOKEN_EOF), start(nullptr), line(0), length(0) {};
    Token(token_type type) : type(type), start(nullptr), line(0), length(0) {}



};

