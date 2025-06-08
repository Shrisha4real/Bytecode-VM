#pragma once
#include "common.h"


class Token {

public:
    token_type type;
    const char* start;
    int line;
    int length;
};

