#include <ctype.h>
#include "coin_util.h"
#include "myprintf.h"
#include "xmlcoincoin.h"

/* alors attention ce qui suit est un parseur xml plutot rustique dont la
   robustesse doit encore etre demontree
   Il entre directement dans le top five des trucs les plus immondes de wmcc.
*/

void clear_XMLBlock(XMLBlock *b) {
  b->attr = NULL; b->tag = NULL; b->content = NULL;
  b->tag_len = b->content_len = 0;
}

void destroy_XMLBlock(XMLBlock *b) {
  XMLAttr *a, *a_next;
  for (a = b->attr; a; a = a_next) {
    a_next = a->next; free(a);
  }
  clear_XMLBlock(b);
}

void print_XMLBlock(XMLBlock *b) {
  XMLAttr *a;
  char *s = str_ndup(b->namespace ? b->namespace : "",b->namespace_len); myprintf("XMLBlock: tag = %<CYA %s>:",s); free(s);
  s = str_ndup(b->tag,b->tag_len);  myprintf("%<YEL %s>\n", s); free(s);
  for (a = b->attr; a; a = a->next) {
    s = str_ndup(a->name, a->name_len); myprintf("  Attribute: %s = ", s); free(s);
    s = str_ndup(a->value, a->value_len); myprintf("%<GRN %s>\n", s); free(s);
  }
  s = str_ndup(b->content, MIN(b->content_len,100));
  myprintf("Content: %<CYA %100s>", s);
  if (b->content_len > 100) {
    myprintf("[...]%<CYA %10s>", s + b->content_len - 10);
  }
  free(s);
  printf("\n");
}

static int
isxmlchar(unsigned char c) {
  return (isalnum(c) || strchr("_.-:?",c));
}

/* bonjour je viens de découvrir les namespace en xml .. */
int
XMLBlock_search_tag(char *start, char *end, char *tagdescr, XMLBlock *b) {
  int ok = 0;
  do {
    char *p, *divns;
    while (start < end && *start != '<') ++start;    
    ++start;
    while (start < end && isspace(*start)) ++start;
    p = start; divns = NULL;
    while (p < end && isxmlchar(*p)) {
      if (*p == ':') divns = p;
      ++p;
    }
    if (p >= end) break; /* valable aussi pour start .. */
    if (tagdescr[0] == '*' && tagdescr[1] == ':') {
      if (divns) {
        ok = str_case_startswith(divns, tagdescr+1);
      } else {
        ok = str_case_startswith(start, tagdescr);
      }
    } else ok = str_case_startswith(start, tagdescr);
    if (ok) {
      if (divns) {
        b->tag = divns+1; b->tag_len = p-divns-1;
        b->namespace = start; b->namespace_len = divns - start;
      } else {
        b->tag = start; b->tag_len = p-start;
        b->namespace = NULL; b->namespace_len = 0;
      }
      return 0;
    }
  } while (start < end);
  return 1;
}

/* le XML sai trai trai facile */
int
get_XMLBlock(char *data_start, int data_len, char *tagdescr, XMLBlock *b) {
  char *data_end = data_start + data_len;
  char *p;
  int lev;
  int self_closed = 0;
  destroy_XMLBlock(b);
  /* on cherche le debut, sai pas compliquai */
  /* update: enfin si, avec les namespace ça devient tout de suite 
     un peu plus complliquai */
  
  if (XMLBlock_search_tag(data_start, data_end, tagdescr, b)) return -1;
  p = b->tag + b->tag_len;
  b->content = strchr(p, '>'); 
  if (!b->content || b->content >= data_end) return -2; 
  if (tagdescr[0] == '!' || tagdescr[0] == '?' || b->content[-1] == '/') self_closed = 1;
  b->content++; 

  /* on enregistre les attributs a grands coups de pelle */
  while (*p != '>' && p < b->content) {
    XMLAttr attr;
    while (p < b->content && isspace(*p)) ++p;
    if (p == b->content) break;
    attr.name = p;
    while (p < b->content && isxmlchar(*p)) ++p;
    if (p == b->content) break;
    attr.name_len = p-attr.name;
    while (p < b->content && (isspace(*p) || *p == '=')) ++p;
    if (*p != '"') break;
    attr.value = ++p;
    while (p < b->content && *p != '"') ++p;
    if (p == b->content) break;
    attr.value_len = p - attr.value;
    ++p;
    attr.next = b->attr;
    ALLOC_OBJ(b->attr, XMLAttr);
    memcpy(b->attr, &attr, sizeof(XMLAttr));
  }
  /* on est content */
  if (!self_closed) {
    while(b->content < data_end && isspace(*b->content)) b->content++;
    if (str_case_startswith(b->content, "<![CDATA[")) { /* facile .. */
      b->content += 9;
      p = strstr(b->content, "]]>");
      if (p) {
        b->content_len = p - b->content; p+=3;
      } else return -6;
    } else {        
      p = b->content;      
      lev = 1;
      /* on cherche la sortie */
      while (p < data_end) {
        if (p[0] == '<') {
          if (p[1] == '/') --lev;
          else if (p[1]=='!') { /* raaha putain de commentaires */
            if (str_case_startswith(p+2,"[CDATA[")) { /* merci shift pour ton backend velu */
              p = strstr(p, "]]>");
              if (!p || p >= data_end) return -7;
              p += 2; 
            } else if (strncmp(p+2, "--",2) == 0) {
              p = strstr(p, "-->");
              if (!p || p >= data_end) return -8;
              p += 2;
            }
          } else if (p[1] == '?') { /* xml processing instructions ... tant que j'y suis .. */
            p = strstr(p, "?>");
            if (!p || p >= data_end) return -9;
            p += 1;
          } else ++lev;
        } else if (p[0] == '/' && p[1] == '>') --lev;
        if (lev == 0) break;
        ++p;
      }
      if (p < data_end && lev == 0) {
        b->content_len = p - b->content;
      } else return -5;
    }
    /* on a fini */
    while (*p == '<' || *p == '/' || isspace(*p)) ++p;
    if (b->namespace_len) {
      if (str_ncasecmp(p, b->namespace, b->namespace_len) != 0) return -9;
      p+=b->namespace_len;
      if (*p != ':') return -10;
      ++p;
    }
    if (str_ncasecmp(p, b->tag, b->tag_len) != 0) return -3;
    while (p < data_end && *p != '>') ++p;
    if (p < data_end && *p == '>') {
      //print_XMLBlock(b);
      return p+1 - data_start;
    } else return -4;
  } else return b->content - data_start;
}
