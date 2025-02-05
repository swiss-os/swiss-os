#include "ast.h"
#include "token.h"
#include "gen1.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct ast_node *ast_node__new(struct token *tk)
{
	struct ast_node *self;
	self = malloc(sizeof(*self));
	memset(self, 0, sizeof(*self));
	self->tk = tk;
	if (tk) {
		self->type = tk->type;
	} else {
		//self->type = token__GLUE;
	}
	return self;
}

int ast_node__dispose(struct ast_node *self)
{
	// FIXME
	free(self);
	return 0;
}

struct ast *ast__new(struct lexer *lexer)
{
	struct ast *self;
	self = malloc(sizeof(*self));
	self->lexer = lexer;
	self->root = ast_node__new(NULL);
	self->root->type = token__ROOT;
	self->current = self->root;

	self->stack = malloc(sizeof(self->stack[0]) * 32);
	self->alloced = 32;
	self->index = 0;
	return self;
}

int ast__dispose(struct ast *self)
{
	ast_node__dispose(self->root);
	free(self->stack);
	free(self);
	return 0;
}

int ast__begin_production(struct ast *self, struct token *tk)
{
	struct ast_node *n;
	n = ast_node__new(tk);
	n->type = token__PRODUCTION;
	n->parent = self->root;
	self->current->next = n;
	self->current = n;
	return 0;
}

int ast__end_production(struct ast *self, struct token *tk)
{
	struct ast_node *n;
	n = self->current;
	while (n->type != token__PRODUCTION && n->parent) {
		n = n->parent;
	}
	if (n->type != token__PRODUCTION) {
		return -1;
	}
	self->current = n;
	return 0;
}

int ast__term(struct ast *self, struct token *tk)
{
	struct ast_node *n;
	n = ast_node__new(tk);
	n->type = token__TERM;
	n->parent = self->current;
	switch (self->current->type) {
	case token__OP1:
		n->parent = self->current->parent;
		self->current->next = n;
		break;
	case token__OP2:
		self->current->u.op2.right = n;
		break;
	case token__TERM:
		n->parent = self->current->parent;
		self->current->next = n;
		break;
	case token__PRODUCTION:
		self->current->u.production.children = n;
		break;
	case token__LPAREN:
		self->current->u.lparen.children = n;
		break;
	case token__RPAREN:
		n->parent = self->current->parent;
		self->current->next = n;
		break;
	default:
		ast_node__dispose(n);
		return -1;
	}
	self->current = n;	
	return 0;
}

int ast__op1(struct ast *self, struct token *tk, int precedence)
{
	struct ast_node *n;
	n = ast_node__new(tk);
	n->type = token__OP1;
	n->parent = self->current->parent;
	self->current->parent = n;
	n->u.op1.precedence = precedence;
	n->u.op1.child = self->current;
	self->current = n;
	return 0;
}

int ast__op2(struct ast *self, struct token *tk, int precedence)
{
	struct ast_node *n;
	n = ast_node__new(tk);
	n->type = token__OP2;
	n->parent = self->current->parent;
	self->current->parent = n;
	n->u.op2.precedence = precedence;
	n->u.op2.left = self->current;
	self->current = n;
	return 0;
}


int ast__lparen(struct ast *self, struct token *tk)
{
	if (ast__term(self, tk)) {
		return -1;
	}
	self->current->type = token__LPAREN;
	return 0;
}

int ast__rparen(struct ast *self, struct token *tk)
{	struct ast_node *n;
	n = self->current;
	while (n->type != token__LPAREN && n->parent) {
		n = n->parent;
	}
	if (n->type != token__LPAREN) {
		return -1;
	}
	n->type = token__RPAREN;
	self->current = n;
	return 0;
}

int ast__push(struct ast *self, struct ast_node *n)
{
	if (self->index + 1 >= self->alloced) {
		self->alloced <<= 1;
		self->stack = realloc(self->stack, 
				self->alloced * sizeof(self->stack[0]));
	}
	self->stack[self->index] = n;
	self->index++;
	return 0;
}

struct ast_node *ast__pop(struct ast *self)
{
	if (self->index <= 0) {
		return NULL;
	}
	self->index--;
	return self->stack[self->index];
}

int ast__gen1(struct ast *self)
{
	struct gen1 *gen1;
	gen1 = gen1__new(self->lexer);
	gen1__gen(gen1, self);
	gen1__dispose(gen1);
	return 0;
}


