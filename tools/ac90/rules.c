#include "token.h"
#include "parser.h"
#include "ast.h"

extern int expression(struct parser *self, int mode);
extern int conditional_expression(struct parser *self, int mode);
extern int assignment_expression(struct parser *self, int mode);


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
		if (parser__is_enumeration_constant(self, self->tk)) {
			parser__eat(self);
			return 0;
		}
		break;
	}
	parser__error(self, "constant expected", mode);
	return -1;
}


/*
translation_unit:	external_declaration*
*/
static int external_declaration(struct parser *self, int mode);

int translation_unit_1(struct parser *self, int mode)
{
	if (parser__is_end_of_many(self)) {
		/* failure, this is the end of the main loop */
		return 0;
	}
	/* push again ourself, so we create a loop until it fails */
	parser__push(self, translation_unit_1, mode);
	parser__push(self, external_declaration, mode | 1);
	return 0;
}

int translation_unit(struct parser *self, int mode)
{
	if (self->tk->type != token__ROOT) {
		parser__error(self, "PANIC: bad input", 0);
		return -1;
	}
	parser__eat(self);
	parser__push(self, translation_unit_1, mode);
	parser__push(self, external_declaration, mode | 1);
	return 0;
}

/*
external_declaration:	function_definition | declaration
*/
static int function_definition(struct parser *self, int mode);
static int declaration(struct parser *self, int mode);

static int external_declaration_1(struct parser *self, int mode)
{
	if (parser__clear(self)) {
		return -1;
	}
	parser__push(self, declaration, mode);
	return 0;
}

static int external_declaration(struct parser *self, int mode)
{
	parser__push(self, external_declaration_1, mode);
	parser__push(self, function_definition, mode | 1); /* mode 1: probe before execute */
	return 0;
}

/*
function_definition:	declaration_specifiers? declarator 
			declaration_list? compound_statement
*/
static int declaration_specifiers(struct parser *self, int mode);
static int declarator(struct parser *self, int mode);
static int declaration_list(struct parser *self, int mode);
static int compound_statement(struct parser *self, int mode);

static int function_definition_2(struct parser *self, int mode)
{
	if (parser__clear(self)) {
		return -1;
	}
	parser__push(self, compound_statement, mode);
	return 0;
}

static int function_definition_1(struct parser *self, int mode)
{
	if (parser__clear(self)) {
		return -1;
	}
	parser__push(self, function_definition_2, mode);
	parser__push(self, declaration_list, mode | 1);
	parser__push(self, declarator, mode);
	return 0;
}

static int function_definition(struct parser *self, int mode)
{
	parser__push(self, function_definition_1, mode);
	parser__push(self, declaration_specifiers, mode | 1);
	return 0;
}

/*
declaration:		declaration_specifiers init_declaration_list? ";"
*/
static int init_declaration_list(struct parser *self, int mode);
static int declaration_1(struct parser *self, int mode)
{
	if (parser__clear(self)) {
		return -1;
	}
	if (self->tk->type != token__SEMI) {
		parser__error(self, "missing ';'", mode);
	}
	parser__eat(self);
	return 0;
}

static int declaration(struct parser *self, int mode)
{
	parser__push(self, declaration_1, mode);
	parser__push(self, init_declaration_list, mode | 1);
	parser__push(self, declaration_specifiers, mode);
	return 0;
}

/*
declaration_list:	declaration declaration*
*/
static int declaration_list_1(struct parser *self, int mode)
{
	if (parser__is_end_of_many(self)) {
		return 0;
	}
	parser__push(self, declaration_list_1, mode | 1);
	parser__push(self, declaration, mode | 1);
	return 0;
}

static int declaration_list(struct parser *self, int mode)
{
	parser__push(self, declaration_list_1, mode);
	parser__push(self, declaration, mode | 1);
	parser__push(self, declaration, mode);
	return 0;
}

/*
declaration_specifiers:	(storage_class_specifier | type_specifier | 
			type_qualifier)  (storage_class_specifier | 
			type_specifier | type_qualifier)*
*/
static int struct_or_union_specifier(struct parser *self, int mode);
static int enum_specifier(struct parser *self, int mode);

static int declaration_specifiers_0a(struct parser *self, int mode)
{
	switch(self->tk->type) {
	/*storage_class_specifier:*/
	case token__AUTO:
	case token__REGISTER:
	case token__STATIC:
	case token__EXTERN:
	case token__TYPEDEF:
		parser__eat(self);
		return 0;
	/*type_qualifier:*/
	case token__CONST:
	case token__VOLATILE:
		parser__eat(self);
		return 0;
	/*type_specifier:*/
	case token__VOID:
	case token__CHAR:
	case token__SHORT:
	case token__INT:
	case token__LONG:
	case token__FLOAT:
	case token__DOUBLE:
	case token__SIGNED:
	case token__UNSIGNED:
		parser__eat(self);
		return 0;
	case token__IDENTIFIER:
		if (!parser__is_typedef_name(self, self->tk)) {
			break;
		}
	case token__TYPEDEF_NAME:
		parser__eat(self);
		return 0;
	case token__STRUCT:
	case token__UNION:
		parser__push(self, struct_or_union_specifier, mode);
		return 0;
	case token__ENUM:
		parser__push(self, enum_specifier, mode);
		return 0;
	}
	parser__error(self, "declaration specifier expected", mode);
	return -1;
}

static int declaration_specifiers_1(struct parser *self, int mode)
{
	if (parser__is_end_of_many(self)) {
		return 0;
	}
	parser__push(self, declaration_specifiers_1, mode);
	parser__push(self, declaration_specifiers_0a, mode | 1);
	return 0;
}

static int declaration_specifiers(struct parser *self, int mode)
{
	parser__push(self, declaration_specifiers_1, mode | 1);
	parser__push(self, declaration_specifiers_0a, mode | 1);
	parser__push(self, declaration_specifiers_0a, mode);
	return 0;
}

/*
storage_class_specifier:	"auto" | "register" | "static" | "extern" |
				"typedef"
*/
/*
type_specifier:		"void" | "char" | "short" | "int" | "long" | "float" |
			"double" | "signed" | "unsigned" |
			struct_or_union_specifier | enum_specifier  |
			typedef_name
*/
/*
type_qualifier:		"const" | "volatile"
*/
/*
struct_or_union_specifier:	struct_or_union 
				(identifier? "{" struct_declaration_list "}") |
				identifier
*/
static int struct_declaration_list(struct parser *self, int mode);
static int struct_or_union_specifier_1(struct parser *self, int mode)
{
	if (self->tk->type != token__RBRACE) {
		parser__error(self, "'}' expected", mode);
		return -1;
	}
	parser__eat(self);
	return 0;
}

static int struct_or_union_specifier(struct parser *self, int mode)
{
	switch(self->tk->type) {
	case token__STRUCT:
	case token__UNION:
		parser__eat(self);
		break;
	default:
		parser__error(self, 
				"'enum' or 'struct' keyword expected", mode);
		return -1;
	}
	if (self->tk->type == token__LBRACE) {
		parser__eat(self);
		parser__push(self, struct_or_union_specifier_1, mode);
		parser__push(self, struct_declaration_list, mode);

	} else if (self->tk->type == token__IDENTIFIER) {
		parser__eat(self);
		if (self->tk->type == token__LBRACE) {
			parser__eat(self);
			parser__push(self, struct_or_union_specifier_1, mode);
			parser__push(self, struct_declaration_list, mode);
		}
	} else {
		parser__error(self, "identifier expected", mode);
		return -1;
	}
	return 0;
}

/*
struct_or_union:	"struct" | "union"
*/
/*
struct_declaration_list:	struct_declaration struct_declaration*
*/
static int struct_declaration(struct parser *self, int mode);

static int struct_declaration_list_1(struct parser *self, int mode)
{
	if (parser__is_end_of_many(self)) {
		return 0;
	}
	parser__push(self, struct_declaration_list_1, mode);
	parser__push(self, struct_declaration, mode | 1);
	return 0;
}

static int struct_declaration_list(struct parser *self, int mode)
{
	parser__push(self, struct_declaration_list_1, mode);
	parser__push(self, struct_declaration, mode | 1);
	parser__push(self, struct_declaration, mode);
	return 0;
}

/*
init_declaration_list:	init_declarator ( "," init_declarator)*
*/
static int init_declarator(struct parser *self, int mode);

static int init_declaration_list_1(struct parser *self, int mode)
{
	if (self->tk->type != token__SEMI) {
		return 0;
	}
	parser__eat(self);
	parser__push(self, init_declaration_list_1, mode);
	parser__push(self, init_declarator, mode);
	return 0;
}

static int init_declaration_list(struct parser *self, int mode)
{
	parser__push(self, init_declaration_list_1, mode);
	parser__push(self, init_declarator, mode);
	return 0;
}

/*
init_declarator:	declarator ("=" initializer)?
*/
static int initializer(struct parser *self, int mode);

static int init_declarator_1(struct parser *self, int mode)
{
	if (self->tk->type != token__ASSIGN) {
		return 0;
	}
	parser__eat(self);
	parser__push(self, initializer, mode);
	return 0;
}

static int init_declarator(struct parser *self, int mode)
{
	parser__push(self, init_declarator_1, mode);
	parser__push(self, declarator, mode);
	return 0;
}

/*
struct_declaration:	specifier_qualifier_list struct_declarator_list ";"
*/
static int struct_declarator_list(struct parser *self, int mode);
static int specifier_qualifier_list(struct parser *self, int mode);

static int struct_declaration_1(struct parser *self, int mode)
{
	if (self->tk->type != token__SEMI) {
		parser__error(self, "';' expected", mode);
		return -1;
	}
	parser__eat(self);
	return 0;
}

static int struct_declaration(struct parser *self, int mode)
{
	parser__push(self, struct_declaration_1, mode);
	parser__push(self, struct_declarator_list, mode);
	parser__push(self, specifier_qualifier_list, mode);
	return 0;
}

/*
specifier_qualifier_list:	(type_specifier | type_qualifier)
				(type_specifier | type_qualifier)*
*/

static int specifier_qualifier_list_0a(struct parser *self, int mode)
{
	switch(self->tk->type) {
	/*type_qualifier:*/
	case token__CONST:
	case token__VOLATILE:
		parser__eat(self);
		return 0;
	/*type_specifier:*/
	case token__VOID:
	case token__CHAR:
	case token__SHORT:
	case token__INT:
	case token__LONG:
	case token__FLOAT:
	case token__DOUBLE:
	case token__SIGNED:
	case token__UNSIGNED:
		parser__eat(self);
		return 0;
	case token__IDENTIFIER:
		if (!parser__is_typedef_name(self, self->tk)) {
			break;
		}
	case token__TYPEDEF_NAME:
		parser__eat(self);
		return 0;
	case token__STRUCT:
	case token__UNION:
		parser__push(self, struct_or_union_specifier, mode);
		return 0;
	case token__ENUM:
		parser__push(self, enum_specifier, mode);
		return 0;
	}
	parser__error(self, "declaration specifier expected", mode);
	return -1;
}


static int specifier_qualifier_list_1(struct parser *self, int mode)
{
	if (parser__is_end_of_many(self)) {
		return 0;
	}
	parser__push(self, specifier_qualifier_list_1, mode);
	parser__push(self, specifier_qualifier_list_0a, mode | 1);
	return 0;
}

static int specifier_qualifier_list(struct parser *self, int mode)
{
	parser__push(self, specifier_qualifier_list_1, mode);
	parser__push(self, specifier_qualifier_list_0a, mode | 1);
	parser__push(self, specifier_qualifier_list_0a, mode);
	return 0;
}

/*
struct_declarator_list:		struct_declarator ( "," struct_declarator)*
*/
static int struct_declarator(struct parser *self, int mode);

static int struct_declarator_list_1(struct parser *self, int mode)
{
	if (self->tk->type != token__COMMA) {
		return 0;
	}
	parser__eat(self);
	parser__push(self, struct_declarator_list_1, mode);
	parser__push(self, struct_declarator, mode);
}

static int struct_declarator_list(struct parser *self, int mode)
{
	parser__push(self, struct_declarator_list_1, mode);
	parser__push(self, struct_declarator, mode);
	return 0;
}
/*
struct_declarator:	declarator | 
			(declarator? ":" constant_expression)
*/
static int constant_expression(struct parser *self, int mode);

static int struct_declarator_1(struct parser *self, int mode)
{
	if (self->tk->type == token__COLON) {
		parser__eat(self);
		parser__push(self, constant_expression, mode);
	} 
	return 0;
}

static int struct_declarator(struct parser *self, int mode)
{
	if (self->tk->type == token__COLON) {
		parser__eat(self);
		parser__push(self, constant_expression, mode);
	} else {
		parser__push(self, struct_declarator_1, mode);
		parser__push(self, declarator, mode);
	}
	return 0;
}

/*
enum_specifier:		"enum" identifier | 
			(identifier? "{" enumerator_list "}")
*/
static int enumerator_list(struct parser *self, int mode);

static int enum_specifier_1(struct parser *self, int mode)
{
	if (self->tk->type != token__RBRACE) {
		parser__error(self, "'}' expected", mode);
		return -1;
	}
	parser__eat(self);
	return 0;
}

static int enum_specifier(struct parser *self, int mode)
{
	if (self->tk->type != token__ENUM) {
		parser__error(self, "'enum' keyword expected", mode);
		return -1;
	}
	parser__eat(self);
	if (self->tk->type == token__LBRACE) {
		parser__eat(self);
		parser__push(self, enum_specifier_1, mode);
		parser__push(self, enumerator_list, mode);
	} else if (self->tk->type == token__IDENTIFIER) {
		parser__eat(self);
		if (self->tk->type == token__LBRACE) {
			parser__eat(self);
			parser__push(self, enum_specifier_1, mode);
			parser__push(self, enumerator_list, mode);
		}
	} else {
		parser__error(self, "enum specifier expected", mode);
		return -1;
	}
	return 0;
}

/*
enumerator_list:	enumerator ( "," enumerator)*
*/
static int enumerator(struct parser *self, int mode);

static int enumerator_list_1(struct parser *self, int mode)
{
	if (self->tk->type == token__COMMA) {
		parser__eat(self);
		parser__push(self, enumerator_list_1, mode);
		parser__push(self, enumerator, mode);
	}
	return 0;
}

static int enumerator_list(struct parser *self, int mode)
{
	parser__push(self, enumerator_list_1, mode);
	parser__push(self, enumerator, mode);
}

/*
enumerator:		identifier ("=" constant_expression)?
*/
static int enumerator(struct parser *self, int mode)
{
	if (self->tk->type == token__IDENTIFIER) {
		parser__eat(self);
		if (self->tk->type == token__ASSIGN) {
			parser__eat(self);
			parser__push(self, constant_expression, mode);
		}
	} else {
		parser__error(self, "identifier expected", mode);
		return -1;
	}		
	return 0;
}

/*
declarator:		pointer? direct_declarator
*/
static int direct_declarator(struct parser *self, int mode);
static int pointer(struct parser *self, int mode);

static int declarator_1(struct parser *self, int mode)
{
	if (parser__clear(self)) {
		return -1;
	}
	parser__push(self, direct_declarator, mode);
	return 0;
}

static int declarator(struct parser *self, int mode)
{
	parser__push(self, declarator_1, mode);
	parser__push(self, pointer, mode | 1);
	return 0;
}

/*
direct_declarator:	direct_declarator1 | direct_declarator2

direct_declarator2:	(direct_declarator "[" constant_expression? "]") |
			(direct_declarator "(" parameter_type_list  ")") |
			(direct_declarator "(" identifier_list?  ")") 

direct_declarator1:	identifier | ("(" declarator ")")
*/
static int parameter_type_list(struct parser *self, int mode);
static int identifier_list(struct parser *self, int mode);

static int direct_declarator_2(struct parser *self, int mode)
{
	if (self->tk->type == token__RPAREN) {
		parser__eat(self);
	} else {
		parser__error(self, "')' expected", mode);
		return -1;
	}
	return 0;
}

/* (direct_declarator "[" constant_expression? "]")  */
static int direct_declarator_1_3(struct parser *self, int mode)
{
	if (parser__clear(self)) {
		return -1;
	}
	if (self->tk->type != token__RBRACK) {
		parser__error(self, "']' expected", mode);
		return -1;
	}
	parser__eat(self);
	return 0;
}

/* (direct_declarator "(" identifier_list?  ")"  */
static int direct_declarator_1_2(struct parser *self, int mode)
{
	if (parser__clear(self)) {
		return -1;
	}
	if (self->tk->type != token__LPAREN) {
		parser__error(self, "')' expected", mode);
		return -1;
	}
	parser__eat(self);
	return 0;
}

/* (direct_declarator "(" parameter_type_list  ")") */
static int direct_declarator_1_1(struct parser *self, int mode) 
{
	if (self->tk->type != token__LPAREN) {
		parser__error(self, "')' expected", mode);
		return -1;
	}
	parser__eat(self);

	return 0;
}

static int direct_declarator_1(struct parser *self, int mode)
{
	if (parser__is_loop(self)) {
		return -1;
	}
	if (self->tk->type == token__LPAREN) {
		parser__eat(self);
		if (self->tk->type != token__IDENTIFIER ||
			parser__is_typedef_name(self, self->tk)) 
		{
			parser__push(self, direct_declarator_1_1, mode);
			parser__push(self, parameter_type_list, mode);
		} else {
			parser__push(self, direct_declarator_1_2, mode);
			parser__push(self, identifier_list, mode | 1);
		}
	} else if (self->tk->type == token__LBRACK) {
		parser__eat(self);
		parser__push(self, direct_declarator_1_3, mode);
		parser__push(self, constant_expression, mode | 1);
	} else {
		parser__push(self, declarator, mode);
	}
	return 0;
}

static int direct_declarator(struct parser *self, int mode)
{
	if (self->tk->type == token__IDENTIFIER) {
		parser__push(self, direct_declarator_1, mode);
		/* advance token after push so the infinite 
		 * loop will be trigered on first call */
		parser__eat(self); 
	} else if (self->tk->type == token__LPAREN) {
		parser__eat(self);
		parser__push(self, direct_declarator_2, mode);
		parser__push(self, declarator, mode);
	} else {
		parser__error(self, "identifier or '(' expected", mode);
		return -1;
	}
	return 0;
}
/*
pointer:		"*" type_qualifier_list? ("*" type_qualifier_list?)*
*/
static int pointer(struct parser *self, int mode)
{
	if (self->tk->type == token__STAR) {
		parser__eat(self);
	} else {
		parser__error(self, "'*' expected", mode);
		return -1;
	}
	while (1) {
		switch(self->tk->type) {
		/*type_qualifier:*/
		case token__CONST:
		case token__VOLATILE:
			parser__eat(self);
			break;
		case token__STAR:
			parser__eat(self);
			break;
		default:
			return 0;
		}
	}
	return 0;
}

/*
type_qualifier_list:	type_qualifier type_qualifier*
*/
/*
parameter_type_list:	parameter_list ( "," "...")?
*/
static int parameter_list(struct parser *self, int mode);

static int parameter_type_list_1(struct parser *self, int mode)
{
	if (self->tk->type == token__COMMA) {
		parser__eat(self);
		if (self->tk->type == token__ELLIPSIS) {
			parser__eat(self);
		} else {
			parser__error(self, "'...' expected", mode);
		}
	}
	return 0;
}
static int parameter_type_list(struct parser *self, int mode)
{
	parser__push(self, parameter_type_list_1, mode);
	parser__push(self, parameter_list, mode);
	return 0;
}

/*
parameter_list:		parameter_declaration ( "," parameter_declaration)*
*/
static int parameter_declaration(struct parser *self, int mode);
static int parameter_list_1(struct parser *self, int mode)
{
	if (self->tk->type == token__COMMA) {
		if (self->tk->next->type == token__ELLIPSIS) {
			return 0;
		}
		parser__eat(self);
	} else {
		return 0;
	}
	parser__push(self, parameter_list_1, mode);
	parser__push(self, parameter_declaration, mode);
	return 0;
}

static int parameter_list(struct parser *self, int mode)
{
	parser__push(self, parameter_list_1, mode);
	parser__push(self, parameter_declaration, mode);
	return 0;
}

/*
parameter_declaration:	declaration_specifiers 
			declarator | abstract_declarator?
*/
static int abstract_declarator(struct parser *self, int mode);
static int parameter_declaration_1(struct parser *self, int mode)
{
	struct token *tk;

	if (self->tk->type == token__STAR || self->tk->type == token__LPAREN) {
		tk = self->tk;
		while (tk->type == token__STAR ||
			tk->type == token__CONST ||
			tk->type == token__VOLATILE ||
			tk->type == token__LPAREN
			)
		{
			tk = tk->next;
		}
		if (self->tk->type == token__IDENTIFIER) {
			parser__push(self, declarator, mode);
		} else {
			parser__push(self, abstract_declarator, mode);
		}
	} else if (self->tk->type == token__IDENTIFIER) {
		parser__push(self, declarator, mode);
	} else if (self->tk->type == token__LBRACK) {
		parser__push(self, abstract_declarator, mode);
	}
}

static int parameter_declaration(struct parser *self, int mode)
{
	parser__push(self, parameter_declaration_1, mode);
	parser__push(self, declaration_specifiers, mode);
	return 0;
}

/*
identifier_list:	identifier ( "," identifier)*
*/
static int identifier_list(struct parser *self, int mode)
{
	if (self->tk->type != token__IDENTIFIER) {
		parser__error(self, "identifier expected", mode);
		return -1;
	}
	parser__eat(self);
	while (self->tk->type == token__LPAREN) {
		parser__eat(self);
		if (self->tk->type != token__IDENTIFIER) {
			parser__error(self, "identifier expected", mode);
			return -1;
		}
		parser__eat(self);
	}
	return 0;
}

/*
initializer:		assignment_expression |
			("{" initializer_list  ","? "}")
*/
static int initializer_list(struct parser *self, int mode);
static int initializer_1(struct parser *self, int mode)
{
	if (self->tk->type == token__COMMA) {
		parser__eat(self);
	}
	if (self->tk->type != token__RBRACE) {
		parser__error(self, "'}' expected", mode);
		return -1;
	}
	parser__eat(self);
	return 0;
}

static int initializer(struct parser *self, int mode)
{
	if (self->tk->type == token__LBRACE) {
		parser__eat(self);
		parser__push(self, initializer_1, mode);
		parser__push(self, initializer_list, mode);
	} else {
		parser__push(self, assignment_expression, mode);
	}
	return 0;
}

/*
initializer_list:	initializer ( "," initializer)*
*/
static int initializer_list_1(struct parser *self, int mode)
{
	if (self->tk->type == token__COMMA && 
		self->tk->next->type != token__RBRACE) 
	{
		parser__eat(self);
		parser__push(self, initializer_list_1, mode);
		parser__push(self, initializer, mode);
	}
	return 0;
}

static int initializer_list(struct parser *self, int mode)
{
	parser__push(self, initializer_list_1, mode);
	parser__push(self, initializer, mode);
	return 0;
}

/*
type_name:		specifier_qualifier_list abstract_declarator?
*/
int type_name_1(struct parser *self, int mode) 
{
	if (parser__clear(self)) {
		return -1;
	}
	return 0;
}

int type_name(struct parser *self, int mode) 
{
	parser__push(self, type_name_1, mode);
	parser__push(self, abstract_declarator, mode | 1);
	parser__push(self, specifier_qualifier_list, mode);
	return 0;
}

/*
abstract_declarator:	pointer | (pointer? direct_abstract_declarator)
*/
static int direct_abstract_declarator(struct parser *self, int mode);
static int abstract_declarator_1(struct parser *self, int mode)
{
	if (self->tk->type == token__LPAREN || 
		self->tk->type == token__LBRACK) 
	{
		parser__push(self, direct_abstract_declarator, mode);
	}
	return 0;
}

static int abstract_declarator(struct parser *self, int mode)
{
	if (self->tk->type == token__STAR) {
		parser__push(self, abstract_declarator_1, mode);
		parser__push(self, pointer, mode);
	} else {
		parser__push(self, abstract_declarator_1, mode);
	}
	return 0;
}

/*
direct_abstract_declarator:	("(" abstract_declarator ")") |
				(("(" abstract_declarator ")")? 
				  (("[" constant_expression? "]") |
				   ("(" parameter_type_list? ")")))*
*/
static int direct_abstract_declarator_3(struct parser *self, int mode)
{
	printf("FIXME\n");
}


static int direct_abstract_declarator_2(struct parser *self, int mode)
{
	printf("FIXME\n");
}


// FIXME
static int direct_abstract_declarator_1(struct parser *self, int mode)
{
	if (self->tk->type != token__RPAREN) {
		parser__error(self, "')' expected", mode);
		return -1;
	}
	parser__eat(self);
	if (self->tk->type == token__LPAREN) {
		parser__eat(self);
		parser__push(self, direct_abstract_declarator_3, mode);
	} else if (self->tk->type == token__LBRACK) {
		parser__eat(self);
		parser__push(self, direct_abstract_declarator_2, mode);
	}
	return 0;
}

static int direct_abstract_declarator(struct parser *self, int mode)
{
	if (self->tk->type == token__LPAREN) {
		parser__eat(self);
		parser__push(self, direct_abstract_declarator_1, mode);
		parser__push(self, abstract_declarator, mode);
	} else if (self->tk->type == token__LBRACK) {
		parser__eat(self);
		parser__push(self, direct_abstract_declarator_2, mode);
	}
	return 0;
}

/*
typedef_name:		identifier
*/
/*
statement:		labeled_statement |
			expression_statement |		
			compound_statement |		
			selection_statement |		
			iteration_statement |		
			jump_statement
*/
/*
labeled_statement:	(identifier ":" statement) |
			("case" constant_expression ":" statement) |
			("default" ":" statement)
*/
/*
expression_statement:	expression? ";"
*/
/*
compound_statement:	"{" declaration_list? statement_list? "}"
*/
static int compound_statement(struct parser *self, int mode)
{
	return 0;
}
/*
statement_list:		statement statement*
*/
/*
selection_statement:	("if" "(" expression ")" statement 
			       ("else" statement)?) |
			("switch" "(" expression ")" statement)
*/
/*
iteration_statement:	("while" "(" expression ")" statement) |
			("do" statement "while" "(" expression ")" ";") |
			("for" "(" expression? ";" expression? ";" expression?
			  ")" statement)
*/
/*
jump_statement:		("goto" identifier ";") |
			("continue" ";") |
			("break" ";") |
			("return" expression? ";")
*/
static int jump_statement(struct parser *self, int mode)
{
	return 0;
}

/*
argument_expression_list:	assignment_expression 
				( "," assignment_expression)*
*/
static int argument_expression_list(struct parser *self, int mode)
{
	return expression(self, mode);
}

/*
constant_expression:	conditional_expression
*/
static int constant_expression(struct parser *self, int mode)
{
	return conditional_expression(self, mode);
}

