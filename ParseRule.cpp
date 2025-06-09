#include "ParseRule.h"
ParseRule::ParseRule(parse_fn pre, parse_fn in, Precedence prec)
	: prefix(pre), infix(in), precedence(prec) {
}