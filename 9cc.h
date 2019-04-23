#define TOKEN_MAX (100)

// values representing the types of tokens
enum {
  TK_NUM = 256, // digit token
  TK_IDENT,     // identifier
  TK_EOF,       // EOF token
};

enum {
  ND_NUM = 256,
  ND_IDENT,
};

typedef struct {
  int ty;      // type
  int val;     // value (when ty is TK_NUM)
  char *input; // token string (for an error message)
} Token;

typedef struct Node {
  int ty;    /* operator or ND_NUM */
  struct Node *lhs;
  struct Node *rhs;
  int val;   /* used when ty is ND_NUM */
  char name; /* used when ty is ND_IDENT */
} Node;

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_ident(char name);
int consume(int ty);

void program();
Node *stmt();
Node *assign();
Node *add();
Node *mul();
Node *term();
void gen(Node *node);

Vector *new_vector();
void vec_push(Vector *vec, void *elem);
int expect(int line, int expected, int actual);
void runtest();

void tokenize(char *p);
Token *get_token(int position);
