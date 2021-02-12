#include "libc.h"

int isspace(int c) {
  if (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v') {
    return 1;
  }
  return 0;
}

int isalpha(int c) {
  if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z') {
    return 1;
  }
  return 0;
}
