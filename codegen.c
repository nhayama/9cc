#include "9cc.h"

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
  case '<':
    printf("  cmp eax, edi\n");
    printf("  setl al\n");
    printf("  movzb eax, al\n");
    break;
  case ND_LE:
    printf("  cmp eax, edi\n");
    printf("  setle al\n");
    printf("  movzb eax, al\n");
    break;
  case ND_EQ:
    printf("  cmp eax, edi\n");
    printf("  sete al\n");
    printf("  movzb eax, al\n");
    break;
  case ND_NE:
    printf("  cmp eax, edi\n");
    printf("  setne al\n");
    printf("  movzb eax, al\n");
    break;
  }

  printf("  push eax\n");
}
