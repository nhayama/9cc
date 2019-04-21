#include "9cc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int pos = 0;
Vector *vec_tokens;

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

int consume(int ty) {
  if (get_token(pos)->ty != ty)
    return 0;
  pos++;
  return 1;
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
    Node *node = add();
    if (!consume(')')) {
      fprintf(stderr, "unmatched parentheses: %s\n",
	      get_token(pos)->input);
      exit(1);
    }
    return node;
  }

  if (get_token(pos)->ty == TK_NUM)
    return new_node_num(get_token(pos++)->val);

  fprintf(stderr, "cannot parse a token: %s\n",
	  get_token(pos)->input);
  exit(1);
}

void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
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

void tokenize(char *p) {
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    Token *token = malloc(sizeof(Token));

    if (*p == '+' || *p == '-' || \
	*p == '*' || *p == '/' || \
	*p == '(' || *p == ')') {
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
  Vector *vec = new_vector();
  expect(__LINE__, 0, vec->len);

  for (int i = 0; i < 100; i++)
    vec_push(vec, (void *)i);

  expect(__LINE__, 100, vec->len);
  expect(__LINE__, 0, (int)vec->data[0]);
  expect(__LINE__, 50, (int)vec->data[50]);
  expect(__LINE__, 99, (int)vec->data[99]);

  printf("OK\n");
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
  Node *node = add();

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  gen(node);

  printf("  pop eax\n");
  printf("  ret\n");
  return 0;
}
