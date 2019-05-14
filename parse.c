#include "9cc.h"

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
  Node *node = equality();
  while (consume('='))
    node = new_node('=', node, assign());
  return node;
}

Node *equality() {
  Node *node = relational();
  for (;;) {
    if (consume(TK_EQ))
      node = new_node(ND_EQ, node, relational());
    else if (consume(TK_NE))
      node = new_node(ND_NE, node, relational());
    else
      return node;
  }
}

Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume('<'))
      node = new_node('<', node, add());
    else if (consume(TK_LE))
      node = new_node(ND_LE, node, add());
    else if (consume('>'))
      node = new_node('<', add(), node);
    else if (consume(TK_GE))
      node = new_node(ND_LE, add(), node);
    else
      return node;
  }
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
  Node *node = unary();

  for (;;) {
    if (consume('*'))
      node = new_node('*', node, unary());
    else if (consume('/'))
      node = new_node('/', node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume('+'))
    return term();
  else if (consume('-'))
    return new_node('-', new_node_num(0), term());
  else
    return term();
}

Node *term() {
  if (consume('(')) {
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

int is_alnum(char c) {
  return (('a' <= c && c <= 'z') || \
	  ('A' <= c && c <= 'Z') || \
	  ('0' <= c && c <= '9') || \
	  (c == '_'));
}

void tokenize(char *p) {
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
      p += 6;
      continue;
    }

    if (strncmp(p, "==", 2) == 0) {
      token->ty = TK_EQ;
      token->input = p;
      vec_push(vec_tokens, (void *)token);
      p += 2;
      continue;
    }

    if (strncmp(p, "!=", 2) == 0) {
      token->ty = TK_NE;
      token->input = p;
      vec_push(vec_tokens, (void *)token);
      p += 2;
      continue;
    }

    if (strncmp(p, "<=", 2) == 0) {
      token->ty = TK_LE;
      token->input = p;
      vec_push(vec_tokens, (void *)token);
      p += 2;
      continue;
    }

    if (strncmp(p, ">=", 2) == 0) {
      token->ty = TK_GE;
      token->input = p;
      vec_push(vec_tokens, (void *)token);
      p += 2;
      continue;
    }

    if (*p == '+' || *p == '-' || \
	*p == '*' || *p == '/' || \
	*p == '(' || *p == ')' || \
	*p == '<' || *p == '>' || \
	*p == '=' || *p == ';') {
      token->ty = *p;
      token->input = p;
      vec_push(vec_tokens, (void *)token);
      p++;
      continue;
    }

    if (isdigit(*p)) {
      token->ty = TK_NUM;
      token->input = p;
      token->val = strtol(p, &p, 10);
      vec_push(vec_tokens, (void *)token);
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      token->ty = TK_IDENT;
      token->input = p;
      vec_push(vec_tokens, (void *)token);
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
