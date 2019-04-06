#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// enum value which represents token type
enum {
  TK_NUM = 256, // integer token
  TK_EOF,       // end of token
};

typedef struct {
  int ty;       // token type
  int val;      // integer vakue (if ty == TK_NUM)
  char *input;  // token sentence (and also used error message)
} Token;

// tokenized array (max token size = 100)
Token tokens[100];

// tokenize function
// arg p is input char array
// tokenized result is stored in token array
void tokenize(char *p) {
  int i = 0;
  while (*p) {
    // skip space
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (*p == '+' || *p == '-') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    fprintf(stderr, "tokenization was failed: %s\n", p);
    exit(1);
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

// this function prints error messge
void error(int i) {
  fprintf(stderr, "unexpected token was detected: %s\n", tokens[i].input);
  exit(1);
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "arg count is invalid");
    return 1;
  }

  // execute tokenize
  tokenize(argv[1]);

  // output assembly header
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // check if first token type is number
  if (tokens[0].ty != TK_NUM)
    error(0);
  printf("  mov rax, %d\n", tokens[0].val);

  int i = 1;
  while (tokens[i].ty != TK_EOF)
  {
    if (tokens[i].ty == '+') {
      i++;
      if (tokens[i].ty != TK_NUM)
        error(i);
      printf("  add rax, %d\n", tokens[i].val);
      i++;
      continue;
    }

    if (tokens[i].ty == '-') {
      i++;
      if (tokens[i].ty != TK_NUM)
        error(i);
      printf("  sub rax, %d\n", tokens[i].val);
      i++;
      continue;
    }

    error(i);
  }

  printf("  ret\n");
  return 0;
}