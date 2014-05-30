#ifndef KB_COINCOIN_H
#define KB_COINCOIN_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xmd.h>
#include <X11/keysym.h>
#include "coincoin.h"

enum { KB_PALMIPEDE, KB_PINNIPEDE, NB_INPUT_CONTEXTS };

struct KbState {
  KeySym ksym;
  int klen;
  unsigned char *buff;  
  XComposeStatus compose_status;
  XIC input_context[NB_INPUT_CONTEXTS];
  Status status;
};
struct KbState *kb_state();
KeySym kb_lookup_string(Dock *dock, XKeyEvent *event);
KeySym kb_lookup_mb_string(Dock *dock, XKeyEvent *event);
void kb_create_input_context_for(Dock *dock, Window win, unsigned idx);
void kb_release_input_context(unsigned idx);
void kb_build();
Status kb_xim_lookup_key(XKeyEvent *event, unsigned idx);
#endif
