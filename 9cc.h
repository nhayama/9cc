#define TOKEN_MAX (100)

// values representing the types of tokens
enum {
  TK_NUM = 256, // digit token
  TK_RETURN,
  TK_EQ,        // ==
  TK_NE,        // !=
  TK_LE,        // <=
  TK_GE,        // >=
  TK_IDENT,     // identifier
  TK_EOF,       // EOF token
};
 
enum {
  ND_NUM = 256,
  ND_RETURN,
  ND_EQ,
  ND_NE,
  ND_LE,
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

typedef struct {
  Vector *keys;
  Vector *vals;
} Map;

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *new_node_ident(char name);
int consume(int ty);

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);

void program();
Node *stmt();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();

void gen(Node *node);
void gen_lval(Node *node);

Vector *new_vector();
void vec_push(Vector *vec, void *elem);
int expect(int line, int expected, int actual);

void runtest();
void test_vector();
void test_map();

int is_alnum(char c);
void tokenize(char *p);
Token *get_token(int position);
