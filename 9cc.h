#define TOKEN_MAX (100)

// values representing the types of tokens
enum {
  TK_NUM = 256, // digit token
  TK_EOF,       // EOF token
};

enum {
  ND_NUM = 256,
};

typedef struct {
  int ty;      // type
  int val;     // value (when ty is TK_NUM)
  char *input; // token string (for an error message)
} Token;

typedef struct Node {
  int ty; /* operator or ND_NUM */
  struct Node *lhs;
  struct Node *rhs;
  int val; /* used when ty is ND_NUM */
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
int consume(int ty);

Node *add();
Node *mul();
Node *term();
void gen(Node *node);

void tokenize(char *p);
