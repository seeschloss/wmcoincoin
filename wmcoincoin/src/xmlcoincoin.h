#ifndef XMLCOINCOIN_H
#define XMLCOINCOIN_H

typedef struct XMLAttr {
  char *name; int name_len;
  char *value; int value_len;
  struct XMLAttr *next;
} XMLAttr;

typedef struct XMLBlock {
  char *tag; int tag_len;
  char *namespace; int namespace_len;
  XMLAttr *attr;
  char *content; int content_len;
} XMLBlock;

void clear_XMLBlock(XMLBlock *b);
void destroy_XMLBlock(XMLBlock *b);
void print_XMLBlock(XMLBlock *b);
int get_XMLBlock(char *data_start, int data_len, char *tagdescr, XMLBlock *b);

#endif
