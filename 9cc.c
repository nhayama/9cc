#include "9cc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int pos = 0;
Vector *vec_tokens;

Node *code[100];

Token *get_token(int position) {
  return (Token *)vec_tokens->data[position];
}

Node *new_node(int ty, Node *lhs, Node *rhs) {
  Node *node = malloc(sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

Node *new_node_ident(char name) {
  Node *node = malloc(sizeof(Node));
  node->ty = ND_IDENT;
  node->name = name;
  return node;
}

Map *new_map() {
  Map *map = (Map *)malloc(sizeof(Map));
  map->keys = new_vector();
  map->vals = new_vector();
  return map;
}

void map_put(Map *map, char *key, void *val) {
  vec_push(map->keys, (void *)key);
  vec_push(map->vals, val);
}

void *map_get(Map *map, char *key) {
  for (int i = map->keys->len - 1; i >= 0; i--)
    if (strcmp((char *)map->keys->data[i], key) == 0)
      return map->vals->data[i];
  return NULL;
}

int consume(int ty) {
  if (get_token(pos)->ty != ty)
    return 0;
  pos++;
  return 1;
}

void program() {
  int i = 0;
  while (get_token(pos)->ty != TK_EOF)
    code[i++] = stmt();
  code[i] = NULL;
}

Node *stmt() {
  Node *node;

  if (consume(TK_RETURN)) {
    node = malloc(sizeof(Node));
    node->ty = ND_RETURN;
    node->lhs = assign();
  } else {
    node = assign();
  }

  if (!consume(';')) {
    fprintf(stderr, "not ';' token: %s\n",
	    get_token(pos)->input);
    exit(1);
  }
  return node;
}

Node *assign() {
  Node *node = add();
  while (consume('='))
    node = new_node('=', node, assign());
  return node;
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume('+'))
      node = new_node('+', node, mul());
    else if (consume('-'))
      node = new_node('-', node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = term();

  for (;;) {
    if (consume('*'))
      node = new_node('*', node, mul());
    else if (consume('/'))
      node = new_node('/', node, mul());
    else
      return node;
  }
}

Node *term() {
  if (consume('(')) {
    /* Node *node = add(); */
    Node *node = assign();
    if (!consume(')')) {
      fprintf(stderr, "unmatched parentheses: %s\n",
	      get_token(pos)->input);
      exit(1);
    }
    return node;
  }

  if (get_token(pos)->ty == TK_NUM)
    return new_node_num(get_token(pos++)->val);

  if (get_token(pos)->ty == TK_IDENT)
    return new_node_ident(get_token(pos++)->input[0]);

  fprintf(stderr, "cannot parse a token: %s\n",
	  get_token(pos)->input);
  exit(1);
}

void gen_lval(Node *node) {
  if (node->ty != ND_IDENT) {
    fprintf(stderr, "a left value is not an identifier\n");
    exit(1);
  }

  // push an address of a variable (name is node->name)
  int offset = ('z' - node->name + 1) * 8;
  printf("  mov eax, ebp\n");
  printf("  sub eax, %d\n", offset);
  printf("  push eax\n");
}

void gen(Node *node) {
  if (node->ty == ND_RETURN) {
    gen(node->lhs);
    printf("  pop eax\n");
    printf("  mov esp, ebp\n");
    printf("  pop ebp\n");
    printf("  ret\n");
    return;
  }

  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  if (node->ty == ND_IDENT) {
    gen_lval(node);
    // push a value of a variable
    printf("  pop eax\n");
    printf("  mov eax, [eax]\n");
    printf("  push eax\n");
    return;
  }

  if (node->ty == '=') {
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop edi\n");
    printf("  pop eax\n");
    printf("  mov [eax], edi\n");
    printf("  push edi\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop edi\n");
  printf("  pop eax\n");

  switch (node->ty) {
  case '+':
    printf("  add eax, edi\n");
    break;
  case '-':
    printf("  sub eax, edi\n");
    break;
  case '*':
    printf("  imul eax, edi\n");
    break;
  case '/':
    printf("  cdq\n");
    printf("  idiv edi\n");
    break;
  }

  printf("  push eax\n");
}

int is_alnum(char c) {
  return (('a' <= c && c <= 'z') || \
	  ('A' <= c && c <= 'Z') || \
	  ('0' <= c && c <= '9') || \
	  (c == '_'));
}

void tokenize(char *p) {
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    Token *token = malloc(sizeof(Token));

    if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      token->ty = TK_RETURN;
      token->input = p;
      vec_push(vec_tokens, (void *)token);
      i++;
      p += 6;
      continue;
    }

    if (*p == '+' || *p == '-' || \
	*p == '*' || *p == '/' || \
	*p == '(' || *p == ')' || \
	*p == '=' || *p == ';') {
      token->ty = *p;
      token->input = p;
      vec_push(vec_tokens, (void *)token);
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      token->ty = TK_NUM;
      token->input = p;
      token->val = strtol(p, &p, 10);
      vec_push(vec_tokens, (void *)token);
      i++;
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      token->ty = TK_IDENT;
      token->input = p;
      vec_push(vec_tokens, (void *)token);
      i++;
      p++;
      continue;
    }

    fprintf(stderr, "cannot tokenize: %s\n", p);
    exit(1);
  }

  Token *token_eof = malloc(sizeof(Token));
  token_eof->ty = TK_EOF;
  token_eof->input = p;
  vec_push(vec_tokens, (void *)token_eof);
}

Vector *new_vector() {
  Vector *vec = malloc(sizeof(Vector));
  vec->data = malloc(sizeof(void *) * 16);
  vec->capacity = 16;
  vec->len = 0;
  return vec;
}

void vec_push(Vector *vec, void *elem) {
  if (vec->capacity == vec->len) {
    vec->capacity *= 2;
    vec->data = realloc(vec->data,
			sizeof(void *) * vec->capacity);
  }
  vec->data[vec->len++] = elem;
}

int expect(int line, int expected, int actual) {
  if (expected == actual)
    return 0;
  fprintf(stderr, "%d: %d expected, but got %d\n",
	  line, expected, actual);
  exit(1);
}

void runtest() {
  test_vector();
  test_map();

  printf("OK\n");
}

void test_vector() {
  Vector *vec = new_vector();
  expect(__LINE__, 0, vec->len);

  for (int i = 0; i < 100; i++)
    vec_push(vec, (void *)i);

  expect(__LINE__, 100, vec->len);
  expect(__LINE__, 0, (int)vec->data[0]);
  expect(__LINE__, 50, (int)vec->data[50]);
  expect(__LINE__, 99, (int)vec->data[99]);
}

void test_map() {
  Map *map = new_map();
  expect(__LINE__, 0, (int)map_get(map, "foo"));

  map_put(map, "foo", (void *)2);
  expect(__LINE__, 2, (int)map_get(map, "foo"));

  map_put(map, "bar", (void *)4);
  expect(__LINE__, 4, (int)map_get(map, "bar"));

  map_put(map, "foo", (void *)6);
  expect(__LINE__, 6, (int)map_get(map, "foo"));
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "too many or too few arguments\n");
    return 1;
  }

  int opt;
  while ((opt = getopt(argc, argv, "t")) != -1) {
    if (opt == 't') {
      runtest();
      return 0;
    }
  }

  vec_tokens = new_vector();

  tokenize(argv[1]);
  program();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // prologue
  printf("  push ebp\n");
  printf("  mov ebp, esp\n");
  printf("  sub esp, 208\n");

  for (int i = 0; code[i]; i++) {
    gen(code[i]);

    // now the top of the stack is an evaluation result
    // in the last statement
    printf("  pop eax\n");
  }

  // epilogue
  printf("  mov esp, ebp\n");
  printf("  pop ebp\n");
  printf("  ret\n");
  return 0;
}
