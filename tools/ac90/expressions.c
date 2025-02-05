#include "parser.h"
#include "token.h"
#include "ast.h"

extern int type_name(struct parser *self, int mode);

static int identifier(struct parser *self, int mode)
{
	if (self->tk->type == token__IDENTIFIER) {
		parser__eat(self);
		return 0;
	}
	parser__error(self, "identifier expected", mode);
	return -1;
}

/*
constant:		integer_constant | character_constant |
			floating_constant | enumeration_constant
*/
static int enumeration_constant(struct parser *self, int mode);

static int constant(struct parser *self, int mode) 
{
	switch (self->tk->type) {
	case token__INTEGER_CONSTANT:
	case token__LONG_CONSTANT:
	case token__UNSIGNED_CONSTANT:
	case token__UNSIGNED_LONG_CONSTANT:
	case token__FLOATING_CONSTANT:
	case token__LONG_DOUBLE_CONSTANT:
	case token__FLOAT_CONSTANT:
	case token__CHARACTER_CONSTANT:
	case token__ENUMERATION_CONSTANT:
		parser__eat(self);
		return 0;
	case token__IDENTIFIER:
		return enumeration_constant(self, mode);
	}
	parser__error(self, "constant expected", mode);
	return -1;
}

static int enumeration_constant(struct parser *self, int mode)
{
	return 0;
}

/*
expression:		assignment_expression ( "," assignment_expression)* 
*/
int expression(struct parser *self, int mode) 
{
	return 0;
}

/*
assignment_expression:	conditional_expression |
			((unary_expression assignment_operator)* 
			  conditional_expression)
*/
int assignment_expression(struct parser *self, int mode) 
{
	return 0;
}


/*
assignment_operator:	"=" | "*=" | "/=" | "%=" | "+=" | "-=" | "<<=" |
			">>=" | "&=" | "^=" | "|="
*/
/*
conditional_expression:	logical_or_expression 
			("?" expression ":" ("?" expression ":")* 
			  logical_or_expression)? 
*/
int conditional_expression(struct parser *self, int mode) 
{
	return 0;
}

/*
logical_or_expression:	logical_and_expression ("||" logical_and_expression)*
*/
/*
logical_and_expression:	inclusive_or_expression | 
			("&&" inclusive_or_expression)* 
*/
/*
inclusive_or_expression:	exclusive_or_expression 
				("|" exclusive_or_expression)*
*/
/*
exclusive_or_expression:	and_expression ("^" and_expression)*
*/
/*
and_expression:		equality_expression ("&" equality_expression)*
*/
/*
equality_expression:	relational_expression 
			("==" | "!=" relational_expression)*
*/
/*
relational_expression:	shift_expression 
			("<" | ">" | "<=" | ">=" shift_expression)*
*/
/*
shift_expression:	additive_expression ("<<" | ">>" additive_expression)*
*/
/*
additive_expression:	multiplicative_expression 
			("+" | "-" multiplicative_expression)*
*/
/*
multiplicative_expression:	cast_expression
				("*" | "/" | "%" cast_expression)
*/
/*
cast_expression:	("(" type_name ")")* unary_expression
*/
/*
unary_expression:	unary_expression1 | unary_expression2
*/
/*
unary_expression1:	postfix_expression |
			(unary_operator cast_expression) |
			("sizeof" "(" type_name ")")
*/
/*
unary_expression2:	"++" | "--" | "sizeof" unary_expression 
*/
/*
unary_operator:		"&" | "*" | "+" | "-" | "~" | "!"
*/
/*
postfix_expression:	primary_expression |
			(postfix_expression
			 ("[" expression "]") |
			 ("(" argument_expression_list? ")") |
			 ("." identifier) |
			 ("->" identifier) |
			 "++" | "--")
*/
/*
primary_expression:	identifier | constant | string_literal | 
			("(" expression ")")
*/
static int primary_expression_1(struct parser *self, int mode) 
{
	if (self->tk->type == token__RPAREN) {
		parser__eat(self);
		return 0;
	}
	parser__error(self, "')' expected", 0);
	return -1;
}

static int primary_expression(struct parser *self, int mode) 
{
	switch (self->tk->type) {
	case token__INTEGER_CONSTANT:
	case token__LONG_CONSTANT:
	case token__UNSIGNED_CONSTANT:
	case token__UNSIGNED_LONG_CONSTANT:
	case token__FLOATING_CONSTANT:
	case token__LONG_DOUBLE_CONSTANT:
	case token__FLOAT_CONSTANT:
	case token__CHARACTER_CONSTANT:
	case token__STRING_LITERAL:
		parser__eat(self);
		return 0;
	case token__ENUMERATION_CONSTANT:
	case token__IDENTIFIER:
		parser__eat(self);
		return 0;
	case token__LPAREN:
		parser__eat(self);
		parser__push(self, primary_expression_1, 0);
		parser__push(self, expression, 0);
		return 0;
	}
	parser__error(self, "primary expression expected", mode);
	return -1;
}



/*
argument_expression_list:	assignment_expression 
				( "," assignment_expression)*
*/
