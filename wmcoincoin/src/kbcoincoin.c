#include <libintl.h>
#define _(String) gettext (String)
#include "kbcoincoin.h"

static struct KbState KbState_;

struct KbState *kb_state() { return &KbState_; }

/*KeySym kb_lookup_string(Dock *dock UNUSED, XKeyEvent *event) {
  //EditW *ew = dock->editw;
  KbState_.klen = XLookupString(event, (char*)KbState_.buff, sizeof(KbState_.buff), &KbState_.ksym, &KbState_.compose_status);
  return KbState_.ksym;
}

KeySym kb_lookup_mb_string(Dock *dock UNUSED, XKeyEvent *event) {
  //EditW *ew = dock->editw;
  if (KbState_.input_context) {
    KbState_.klen = XmbLookupString(KbState_.input_context[0], event, (char*) KbState_.buff, sizeof(KbState_.buff), &KbState_.ksym, 0);
  } else {
    KbState_.klen = XLookupString(event, (char*) KbState_.buff, sizeof(KbState_.buff), &KbState_.ksym, 0);
  }
  return KbState_.ksym;
}
*/
/* Wayland port: there is no XIM.  libxwl's XLookupString() already returns
 * UTF-8 text, handles dead keys and Compose sequences (xkbcommon), so the
 * input contexts are gone and kb_xim_lookup_key() is a thin wrapper. */

#define BASE_BUFSIZE 20
Status
kb_xim_lookup_key(XKeyEvent *event, unsigned idx)
{
  static char buf[BASE_BUFSIZE];
  int rlen;
  Status status;
  (void)idx;

  memset(buf, 0, sizeof buf);
  kb_state()->ksym = 0;
  rlen = XLookupString(event, buf, sizeof(buf) - 1, &kb_state()->ksym, NULL);
  if (rlen > 0) status = (kb_state()->ksym != NoSymbol) ? XLookupBoth : XLookupChars;
  else status = (kb_state()->ksym != NoSymbol) ? XLookupKeySym : XLookupNone;

  if (status == XLookupNone) { kb_state()->klen = 0; return 0; }
  if (status == XLookupKeySym) rlen = 0;
  kb_state()->buff = (unsigned char *)buf;
  kb_state()->status = status;
  kb_state()->klen = rlen;
  return status;
}

void kb_create_input_context_for(Dock *dock, Window win, unsigned idx) { (void)dock; (void)win; (void)idx; }
void kb_release_input_context(unsigned idx) { (void)idx; }

void kb_build() {
  memset(&KbState_, 0, sizeof KbState_);
}
