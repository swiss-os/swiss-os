
#ifndef GEN1_H_
#define GEN1_H_

struct ast;
struct gen1_node;

struct gen1 {
	struct buf *buf;
	struct ast *tree;
	struct lexer *lexer;
	struct gen1_node *stack;
	int alloced;
	int index;
};


struct gen1 *gen1__new(struct lexer *lexer);
int gen1__dispose(struct gen1 *self);

int gen1__gen(struct gen1 *self, struct ast *tree);

#endif /* GEN1_H_ */
