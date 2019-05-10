#include "9cc.h"

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

