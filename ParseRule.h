#pragma once
#include "common.h"


class ParseRule {
public:
	parse_fn prefix;
	parse_fn infix;
	Precedence precedence;
	ParseRule(parse_fn pre = nullptr, parse_fn in = nullptr, Precedence prec = Precedence::PREC_NONE);
	
};

