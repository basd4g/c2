#include "parse.h"

static ArrayIndexes *add_descendant(ArrayIndexes *now_descendant, int index) {
  ArrayIndexes *descendant = calloc(1, sizeof(ArrayIndexes));
  descendant->index = index;
  descendant->parent = now_descendant;
  return descendant;
}

static int var_array_length(Var *var, ArrayIndexes *indexes) {
  Type *type = var->type;
  while(indexes) {
    indexes = indexes->parent;
    type = type->ptr_to;
  }
  return type->array_length;
}

static Node *new_node_array_cell(Var *var, ArrayIndexes *indexes) {
  if (!indexes) {
    return new_node_var(var->name, var->length, var);
  }
  return new_node_deref(new_node_add(
    new_node_array_cell(var, indexes->parent),
    new_node_num(indexes->index)
  ));
}

static Node *new_node_zero_padding_array(Var *var, ArrayIndexes *descendant) {
  Node head = {};
  Node *tail = &head;

  ArrayIndexes *indexes = descendant;
  Type *type = var->type;
  while(indexes) {
    indexes = indexes->parent;
    type = type->ptr_to;
    if (!type) {
      error("too deep initializer of array");
    }
  }
  if (type->kind != TYPE_ARRAY) {
    return new_node_assign(new_node_array_cell(var, descendant), new_node_num(0));
  }
  for (int i = 0; i < type->array_length; i++) {
    tail->next = new_node_zero_padding_array(var, add_descendant(descendant, i));
    while (tail->next) {
      tail = tail->next;
    }
  }
  return head.next;
}

Node *init_lvar_stmts(Token **rest, Token *token, Var **lvarsp, ArrayIndexes *descendant) {
  Var *var = *lvarsp;
  Node *node;

  if (!equal(token, "{")) {
    Node *right = assign(&token, token, lvarsp);
    node = new_node_assign(new_node_array_cell(var, descendant), right);
    *rest = token;
    return node;
  }
  token = token->next;

  Node head = {};
  Node *tail = &head;
  int array_length = var_array_length(var, descendant);
  int ct = 0;

  while (!equal(token, "}")) {
    tail->next = init_lvar_stmts(&token, token, lvarsp, add_descendant(descendant, ct++));
    while(tail->next) {
      tail = tail->next;
    }

    if (!equal(token,",")) {
      break;
    }
    token = token->next;

    if (ct > array_length) {
      error_at_token(token, "too many initializer of array");
    }
  }
  if (!equal(token, "}")) {
    error_at_token(token, "expected }");
  }
  token = token->next;

  // Zero padding
  for (;ct< array_length; ct++) {
    tail->next = new_node_zero_padding_array(var, add_descendant(descendant, ct));
    while(tail->next) {
      tail = tail->next;
    }
  }

  node = head.next;
  *rest = token;
  return node;
}
