#ifndef INCLUDED_parse_h_
#define INCLUDED_parse_h_

#include "../willani.h"

//======================================
// strings.c
char get_escape_char(char c);
String *new_string(char *p, int length);


//======================================
// var.c
extern int lvar_byte;
Var *find_in_vars(char *name, int namelen, Var *vars);
void *new_var(Type *type, char *name, int namelen);


//======================================
// new_node.c
Node *new_node_var(char *name, int length, Token *token);
Node *new_node_num(long value, Token *token);
Node *new_node_string(Token *token);
Node *new_node_func_call(char *name, int len, Node *args, Token *token);
Node *new_node_addr(Node *unary_node, Token *token);
Node *new_node_deref(Node *unary_node, Token *token);
Node *new_node_not(Node *left, Token *token);
Node *new_node_bit_not(Node *left, Token *token);
Node *new_node_pre_increment(Node *left, Token *token);
Node *new_node_pre_decrement(Node *left, Token *token);
Node *new_node_post_increment(Node *left, Token *token);
Node *new_node_post_decrement(Node *left, Token *token);
Node *new_node_assign(Node *left, Node *right, Token *token);
Node *new_node_assign_add(Node *left, Node *right, Token *token);
Node *new_node_assign_sub(Node *left, Node *right, Token *token);
Node *new_node_assign_mul(Node *left, Node *right, Token *token);
Node *new_node_assign_div(Node *left, Node *right, Token *token);
Node *new_node_assign_mod(Node *left, Node *right, Token *token);
Node *new_node_add(Node *left, Node *right, Token *token);
Node *new_node_sub(Node *left, Node *right, Token *token);
Node *new_node_mul(Node *left, Node *right, Token *token);
Node *new_node_div(Node *left, Node *right, Token *token);
Node *new_node_mod(Node *left, Node *right, Token *token);
Node *new_node_equal(Node *left, Node *right, Token *token);
Node *new_node_not_equal(Node *left, Node *right, Token *token);
Node *new_node_less_than(Node *left, Node *right, Token *token);
Node *new_node_less_equal(Node *left, Node *right, Token *token);
Node *new_node_member(Node *parent, char *name, int namelen,  Token *token);
Node *new_node_comma(Node *left, Node *right, Token *token);
Node *new_node_bit_or(Node *left, Node *right, Token *token);
Node *new_node_bit_xor(Node *left, Node *right, Token *token);
Node *new_node_bit_and(Node *left, Node *right, Token *token);
Node *new_node_logical_or(Node *left, Node *right, Token *token);
Node *new_node_logical_and(Node *left, Node *right, Token *token);
Node *new_node_ternary(Node *cond, Node *left, Node *right, Token *token);
Node *new_node_block(Node *body, Token *token);
Node *new_node_if(Node *cond, Node *then, Node *els, Token *token);
Node *new_node_switch(Node *cond, Node *cases, Node *body, bool have_default, Token *token);
Node *new_node_while(Node *cond, Node *then, Token *token);
Node *new_node_for(Node *init, Node *cond, Node* increment, Node *then, Token *token);
Node *new_node_return(Node *left, Token *token);
Node *new_node_expr_stmt(Node *expr_node, Token *token);
Node *new_node_continue(Token *token);
Node *new_node_break(Token *token);
Node *new_node_case(Token *token, int case_num);
Node *new_node_default(Token *token);


//======================================
// log.c
void print_node(FILE *file, Node *node);
void parse_log();


//======================================
// stmt.c
Node *stmt(Token **rest, Token *token);
Node *stmt_without_declaration(Token **rest, Token *token);
Node *if_stmt(Token **rest, Token *token);
Node *while_stmt(Token **rest, Token *token);
Node *switch_stmt(Token **rest, Token *token);
Node *for_stmt(Token **rest, Token *token);
Node *block_stmt(Token **rest, Token *token);
Node *expr_stmt(Token **rest, Token *token);
Node *return_stmt(Token **rest, Token *token);
Node *declare_lvar_stmt(Token **rest, Token *token);
void typedef_stmt(Token **rest, Token *token);


//======================================
// expr.c
Node *expr(Token **rest, Token *token);
Node *assign(Token **rest, Token *token);
Node *ternary(Token **rest, Token *token);
Node *equality(Token **rest, Token *token);
Node *bit_or(Token **rest, Token *token);
Node *bit_xor(Token **rest, Token *token);
Node *bit_and(Token **rest, Token *token);
Node *log_or(Token **rest, Token *token);
Node *log_and(Token **rest, Token *token);
Node *relational(Token **rest, Token *token);
Node *add(Token **rest, Token *token);
Node *mul(Token **rest, Token *token);
Node *unary(Token **rest, Token *token);
Node *sizeofunary(Token **rest, Token *token);
Node *primary(Token **rest, Token *token);
Node *primary_identifer(Token **rest, Token *token);
Node *stmt_expr(Token **rest, Token *token);


//======================================
// var_init.c
typedef struct ArrayIndexes ArrayIndexes;
struct ArrayIndexes {
  int index;
  ArrayIndexes *parent;
};

Node *init_lvar_stmts(Token **rest, Token *token, Var *lvar, ArrayIndexes *descendant);


//======================================
// function.c
Function *find_function(char *name, int namelen);
void add_function(Function *func);
bool same_function(Function *f1, Function *f2);
Function *function_definition(Token **rest, Token *token, Type *return_type, char *name, int namelen);


//======================================
// read_type.c
Type *read_new_type_struct(Token **rest, Token *token);
typedef enum {
  ALLOW_STATIC,
  DENY_STATIC,
} AllowStaticOrNot;
bool is_type_tokens(Token *token, AllowStaticOrNot ason);
Type *read_type(Token **rest, Token *token, AllowStaticOrNot allow_static_or_not);
Type *declarator(Token **rest, Token *token, Type *type, char **namep, int *namelenp);
Type *type_suffix(Token **rest, Token *token, Type *ancestor);
Member *read_member(Token **rest, Token *token, int offset);


//======================================
// struct_tag.c
typedef struct StructTag StructTag;

struct StructTag {
  char *name;
  int namelen;
  Type *type;
  StructTag *next;
};

void new_tag(char *name, int namelen, Type *type);
StructTag *find_tag_in_tags(char *name, int namelen, StructTag *tags);

//=====================================
// typedef.c
typedef struct TypeDef TypeDef;
struct TypeDef {
  char *name;
  int namelen;
  Type *type;
  TypeDef *next;
};
TypeDef *find_in_typedefs(char *name, int namelen, TypeDef *tdfs);
void new_typedef(Type *type, char *name, int namelen);

//======================================
// scope.c
typedef struct Scope Scope;
struct Scope {
  Var *vars;
  StructTag *tags;
  TypeDef *tdfs;
  Scope *parent;
};

extern Scope *now_scope;
void scope_in();
void scope_out();
Var *find_var(char *name, int namelen);
TypeDef *find_typedef(char *name, int namelen);
StructTag *find_tag(char *name, int namelen);


#endif
