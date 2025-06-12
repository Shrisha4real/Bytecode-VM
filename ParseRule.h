#pragma once
#include "common.h"


class ParseRule {
public:
	parse_fn prefix;//the function pointer to compile a prefix expression starting with a token of that type,
	parse_fn infix;//the function pointer to compile an infix expression whose left operand is followed by a token of that type
	Precedence precedence;//the precedence of an infix expression that uses that token as an operator.
	ParseRule(parse_fn pre = nullptr, parse_fn in = nullptr, Precedence prec = Precedence::PREC_NONE);
	
};

