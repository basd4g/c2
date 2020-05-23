#include "willani.h"

// for debug
static FILE *tokenize_log_file;

// for debug
static char *token_kind_str(Token *token) {
  switch (token->kind) {
    case TK_RESERVED: return("TK_RESERVED");
    case TK_IDENT:    return("TK_INDENT  ");
    case TK_NUM:      return("TK_NUM     ");
    case TK_EOF:      return("TK_EOF     ");
    default : error("unexpected token->kind");
  }
}

// for debug
static void tokenize_log(Token *token) {
  fprintf(tokenize_log_file, "%s, ", token_kind_str(token));
  fprintf(tokenize_log_file, "%.*s\n",token->length ,token->location);
}

bool is_identifer_token(Token *token) {
  return token->kind == TK_IDENT;
}

bool is_number_token(Token *token) {
  return token->kind == TK_NUM;
}

bool equal(Token *token, char *str) {
  return strlen(str) == token->length &&
          !strncmp(token->location, str, token->length);
}

long get_number(Token *token) {
  if (token->kind != TK_NUM)
    error_at(token, "expected a number");
  return token->value;
}

// Create new (tail) token, Connect to the current token and Return new (tail) token.
static Token *new_token(TokenKind kind, Token *current, char *location, int length, long value) {
  Token *newtoken = calloc(1, sizeof(Token));
  newtoken->kind = kind;
  newtoken->location = location;
  newtoken->length = length;
  newtoken->value = value;
  current->next = newtoken;

  // for debug
  tokenize_log(newtoken);

  return newtoken;
}

// To return 0 is not reserved token
static int reserved_token_length(char *p) {
  char tokens[][3] = { "==", "!=", "<=", ">=", "+", "-", "*", "/", "(", ")", ">", "<", ";", "="};
  for (int i=0; i<sizeof(tokens); i++) {
    if ( strncmp(p, tokens[i], strlen(tokens[i])) == 0 )
      return strlen(tokens[i]);
  }
  return 0;
}

static int identifer_token_length(char *p) {
  if (!isalpha(*p)) {
    return 0;
  }
  int i = 1;
  while (isalnum(*(p+i))) {
    i++;
  }
  return i;
}

Token *tokenize(char *p) {
  Token head = {};
  Token *current = &head;

  // for debug
  tokenize_log_file = fopen("tokenize.log","w");
  if (tokenize_log_file == NULL) {
    error("fail to open tokenize.log");
  }

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (isdigit(*p)) {
      char *p_old = p;
      long value = strtol(p, &p, 10); // with update p
      current = new_token(TK_NUM, current, p_old, p - p_old, value);
      continue;
    }

    int ilen = identifer_token_length(p);
    if (ilen != 0) {
      current = new_token(TK_IDENT, current, p, ilen, 0);
      p += ilen;
      continue;
    }

    int rlen = reserved_token_length(p);
    if (rlen != 0) {
      current = new_token(TK_RESERVED, current, p, rlen, 0);
      p += rlen;
      continue;
    }

    error_at(current, "Invalid token");
  }

  new_token(TK_EOF, current, p, 0, 0);

  // for debug
  fclose(tokenize_log_file);

  return head.next;
}
