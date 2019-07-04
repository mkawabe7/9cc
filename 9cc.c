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

// input data
char *user_input;

// focus position
int pos = 0;

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

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
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
void token_error(int i) {
  fprintf(stderr, "unexpected token was detected: %s\n", tokens[i].input);
  exit(1);
}

void error_msg(char *loc, char *msg) {
  int err_pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", err_pos, "");
  fprintf(stderr, "^ %s\n", msg);
  exit(1);
}

// node type definition
enum {
  ND_NUM = 256,   // Node type: integer
};

typedef struct Node {
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val;      // if(ty == ND_NUM): val = integer value
} Node;

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
  if (tokens[pos].ty != ty)
    return 0;
  pos++;
  return 1;
}

Node *mul();
Node *expr();
Node *term();

Node *expr() {
  Node *node = mul();

  for(;;) {
    if (consume('+')) node = new_node('+', node, mul());
    else if (consume('-')) node = new_node('-', node, mul());
    else return node;
  }
}

Node *mul() {
  Node *node = term();

  for(;;) {
    if (consume('*')) node = new_node('*', node, term());
    else if (consume('/')) node = new_node('/', node, term());
    else return node;
  }
}

Node *term() {
  if (consume('(')) {
    Node *node = expr();
    if (!consume(')')) {
      error_msg(tokens[pos].input, ") is not found");
      return node;
    }
  }

  if (tokens[pos].ty == TK_NUM)
    return new_node_num(tokens[pos++].val);
  error_msg(tokens[pos].input, "this token is not number or (");
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "arg count is invalid");
    return 1;
  }

  user_input = argv[1];

  // execute tokenize
  tokenize(user_input);

  // output assembly header
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // check if first token type is number
  if (tokens[0].ty != TK_NUM)
    token_error(0);
  printf("  mov rax, %d\n", tokens[0].val);

  int i = 1;
  while (tokens[i].ty != TK_EOF)
  {
    if (tokens[i].ty == '+') {
      i++;
      if (tokens[i].ty != TK_NUM)
        token_error(i);
      printf("  add rax, %d\n", tokens[i].val);
      i++;
      continue;
    }

    if (tokens[i].ty == '-') {
      i++;
      if (tokens[i].ty != TK_NUM)
        token_error(i);
      printf("  sub rax, %d\n", tokens[i].val);
      i++;
      continue;
    }

    token_error(i);
  }

  printf("  ret\n");
  return 0;
}