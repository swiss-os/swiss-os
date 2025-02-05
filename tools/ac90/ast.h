
#ifndef AST_H_
#define AST_H_

struct ast_op2 {
	int precedence;
	struct ast_node *left;
	struct ast_node *right;
};

struct ast_op1 {
	int precedence;
	struct ast_node *child;
};


struct ast_term {
	void *dummy;
};


struct ast_lparen {
	struct ast_node *children;
};

struct ast_production {
	struct ast_node *children;
};


struct ast_node
{
	int type;
	struct token *tk;
	struct ast_node *parent;
	struct ast_node *next;

	union  {
		struct ast_term term;
		struct ast_lparen lparen;
		struct ast_op1 op1;
		struct ast_op2 op2;
		struct ast_production production;
	} u;
};

struct ast {
	struct lexer *lexer;
	struct ast_node *root;
	struct ast_node *current;

	struct ast_node *visit;
	struct ast_node **stack;
	int alloced;
	int index;
};


struct ast *ast__new(struct lexer *lexer);
int ast__dispose(struct ast *self);

int ast__gen1(struct ast *self);

int ast__begin_production(struct ast *self, struct token *tk);
int ast__end_production(struct ast *self, struct token *tk);
int ast__term(struct ast *self, struct token *tk);
int ast__op1(struct ast *self, struct token *tk, int precedence);
int ast__op2(struct ast *self, struct token *tk, int precedence);
int ast__lparen(struct ast *self, struct token *tk);
int ast__rparen(struct ast *self, struct token *tk);

#endif /* AST_H_ */
