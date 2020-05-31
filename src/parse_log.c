#include "willani.h"

FILE *lvar_logfile;
FILE *parse_logfile;

static char *node_kind_str(Node *node) {
  switch (node->kind) {
    case ND_ADD: return("+");
    case ND_SUB: return("-");
    case ND_MUL: return("*");
    case ND_DIV: return("/");
    case ND_EQ:  return("==");
    case ND_NE:  return("!=");
    case ND_LT:  return("<");
    case ND_LE:  return("<=");
    case ND_ASSIGN: return("=");
    case ND_LVAR: return("Variable");
    case ND_NUM: return("Integer");
    case ND_ADDR: return("&");
    case ND_DEREF: return("*");
    case ND_RETURN: return("return");
    case ND_IF: return("if");
    case ND_WHILE: return("while");
    case ND_FOR: return("for");
    case ND_BLOCK: return("{}");
    case ND_FUNC_CALL: return("func call");
    case ND_EXPR_STMT: return(";");
    case ND_DECLARE_LVAR: return("int");
    default : error("unexpected node->kind");
  }
}

void print_node(FILE *file, Node *node) {
  if (node->kind == ND_NUM) {
    fprintf(file, "%ld\n",node->value);
    return;
  }

  if (node->kind == ND_LVAR) {
    fprintf(file, "%.*s\n",node->lvar->length, node->lvar->name);
    return;
  }
  if (node->kind == ND_FUNC_CALL) {
    fprintf(file, "%.*s\n",node->fncl->length, node->fncl->name);
    return;
  }
  fprintf(file, "%s\n", node_kind_str(node));
}

static void parse_log_node(Node *node, int depth) {
  if (node == NULL) {
    return;
  }
  fprintf(parse_logfile, "%*s",depth*2, "");
  if (node->type)
    fprintf(parse_logfile, "[%s]:", node->type->kind == TYPE_INT ? "int" : "ptr" );
  print_node(parse_logfile, node);
}

static void parse_log_nodes(Node *node, int depth) {
  if (node == NULL) {
    return;
  }
  parse_log_node(node, depth);

  parse_log_nodes(node->left, depth+1);
  parse_log_nodes(node->right, depth+1);
  parse_log_nodes(node->init, depth+1);
  parse_log_nodes(node->cond, depth+1);
  parse_log_nodes(node->increment, depth+1);
  parse_log_nodes(node->then, depth+1);
  parse_log_nodes(node->els, depth+1);
  parse_log_nodes(node->body, depth+1);
  parse_log_nodes(node->next, depth);
}

static void parse_lvar(Function *func) {
  LVar *lvar = func->lvar;
  char *func_name = func->name;
  int func_name_len = func->namelen;
  LVar *args =  func->args;

  fprintf(lvar_logfile, "%.*s:\n", func_name_len, func_name);
  for( LVar *cur = lvar; cur; cur = cur->next) {
    fprintf(lvar_logfile, "  %.*s (offset: %d)", cur->length, cur->name, cur->offset);
    if (cur == args) {
      fprintf(lvar_logfile, " (argument)");
      args = cur->next;
    }

    fprintf(lvar_logfile, "\n");
  }
}

void parse_log_func(Function *func) {
  parse_lvar(func);
  parse_log_nodes(func->node, 0);
}

void parse_log(Function *func) {
  lvar_logfile = fopen("lvar.log","w");
  if (lvar_logfile == NULL) {
    error("fail to open lvar.log");
  }
  parse_logfile = fopen("parse.log","w");
  if (parse_logfile == NULL) {
    error("fail to open parse.log");
  }

  for (Function *current = func; current; current = current->next) {
    parse_log_func(current);
  }
 
  fclose(parse_logfile);
  fclose(lvar_logfile);
}
