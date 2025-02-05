#include "gen1.h"
#include "ast.h"
#include "token.h"
#include "lexer.h"
#include "buf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct gen1_node {
	struct ast_node *node;
	int flags;
};

struct gen1 *gen1__new(struct lexer *lexer)
{
	struct gen1 *self;
	self = malloc(sizeof(*self));
	self->lexer = lexer;
	self->buf = buf__new("gen1.txt", 1024);
	self->stack = malloc(sizeof(self->stack[0]) * 32);
	self->alloced = 32;
	self->index = 0;
	return self;
}

int gen1__dispose(struct gen1 *self)
{
	buf__dispose(self->buf);
	free(self->stack);
	free(self);
	return 0;
}

int gen1__push(struct gen1 *self, struct ast_node *n, int flags)
{
	if (self->index + 1 >= self->alloced) {
		self->alloced <<= 1;
		self->stack = realloc(self->stack, 
				self->alloced * sizeof(self->stack[0]));
	}
	self->stack[self->index].node = n;
	self->stack[self->index].flags = flags;
	self->index++;
	return 0;
}

struct gen1_node *gen1__pop(struct gen1 *self)
{
	if (self->index <= 0) {
		return NULL;
	}
	self->index--;
	return &(self->stack[self->index]);
}

int gen1__append(struct gen1 *self, char *txt)
{
	buf__append_txt(self->buf, txt, -1);
	return 0;
}

int gen1__append_value(struct gen1 *self, struct token *tk)
{
	return gen1__append(self, lexer__get_value(self->lexer, tk));	
}

int gen1__gen(struct gen1 *self, struct ast *tree)
{
	int r = -1;
	struct gen1_node *top;
	struct ast_node *node;
	int flags;

	self->tree = tree;

	gen1__push(self, tree->root, 0);
	
	while (r) {
		top = gen1__pop(self);
		if (!top) {
			break;
		}
		if (!top->node) {
			continue;
		}
		flags = top->flags;
		node = top->node;
		switch (node->type) {
		case token__ROOT:
			gen1__push(self, node->next, 0);
			break;
		case token__PRODUCTION:
			if (flags == 0) {
				if (node->next) {
					gen1__push(self, node->next, 0);
				}
				gen1__push(self, node, 1);
				gen1__push(self, node->u.production.children,0);
				gen1__append_value(self, node->tk);
				gen1__append(self, ": ");
			} else {
				gen1__append(self, "\n\n");
			}
			break;
		case token__TERM:
			if (node->next) {
				gen1__push(self, node->next, 0);
			}
			gen1__append_value(self, node->tk);
			gen1__append(self, " ");
			break;
		case token__OP1:
			if (flags == 0) {
				if (node->next) {
					gen1__push(self, node->next, 0);
				}
				gen1__push(self, node, 1);
				gen1__push(self, node->u.op1.child, 0);
			} else {
				if (node->type == token__MUL) {
					gen1__append(self, "*");
				} else if (node->type == token__QMARK) {
					gen1__append(self, "?");
				}
			}
			break;
		case token__OP2:
			if (flags == 0) {
				if (node->next) {
					gen1__push(self, node->next, 0);
				}
				gen1__push(self, node, 1);
				gen1__push(self, node->u.op2.left, 0);
			} else {
				if (node->type == token__PIPE) {
					gen1__append(self, " | ");
				}
				gen1__push(self, node->u.op2.right, 0);
			}
			break;
		case token__LPAREN:
		case token__RPAREN:
			if (flags == 0) {
				if (node->next) {
					gen1__push(self, node->next, 0);
				}
				gen1__append(self, " ( ");
				gen1__push(self, node, 1);
				gen1__push(self, node->u.lparen.children, 0);
			} else {
				gen1__append(self, " ) ");
			}
			break;
		default:
			gen1__append(self, "@@");
			gen1__append_value(self, node->tk);
			gen1__append(self, "@@");
			r = 0;
		}
	}
	printf("%s", buf__getstr(self->buf));
	return 0;
}


