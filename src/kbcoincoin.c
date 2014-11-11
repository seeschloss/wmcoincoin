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
/* stolen from "  Vietnamese Input Method for X(xvim) "  */

#define BASE_BUFSIZE 20
Status
kb_xim_lookup_key(XKeyEvent *event, unsigned idx)
{
  static char *buf = 0;
  static int buf_len, rlen;
  Status status;
  XIC ic = kb_state()->input_context[idx];

  if (! buf) {
    buf_len = BASE_BUFSIZE;
    buf = malloc(buf_len);
  }
  memset(buf,0,buf_len);

  if (!ic) {
    rlen = XLookupString(event, buf, buf_len,
			 &kb_state()->ksym, NULL); //KbState_.compose_status);
    status = (rlen == 0 ? XLookupKeySym : XLookupBoth);
  } else {
    kb_state()->ksym = 0;
    rlen = Xutf8LookupString(ic, event, buf, buf_len, 
			   &kb_state()->ksym, &status);    
    if ((status == XBufferOverflow)) {
      buf_len += BASE_BUFSIZE;
      buf = realloc(buf, buf_len);
      memset(buf, 0, buf_len);
      rlen = Xutf8LookupString(ic, event, buf, buf_len, &kb_state()->ksym, &status);
    }
  }
  //unsigned i;
  switch (status) {
    case XLookupNone:
      //printf("XLookupNone\n");
      rlen = 0;
      return 0;
      break;
    case XLookupKeySym:
      //printf("XLookupKeySym ksym=%d\n", (int)kb_state()->ksym);
      rlen = 0;
      break;
    case XLookupChars:
      //printf("XLookupChars len=%d ", rlen);
      //for (i=0; i < buf_len; ++i) printf("%02x ", (unsigned char)buf[i]);
      //printf("\n");
      break;
    case XLookupBoth:
      //printf("XLookupBoth ksym=%d len=%d ", (int)kb_state()->ksym, rlen);
      //for (i=0; i < buf_len; ++i) printf("%02x ", (unsigned char)buf[i]);
      //printf("\n");
      break;
    default:
      assert(0);
  }
  kb_state()->buff = (unsigned char*)buf;
  kb_state()->status = status;
  kb_state()->klen = rlen;
  return status;
}

void kb_create_input_context_for(Dock *dock, Window win, unsigned idx) {
  /* cette ligne marchait pile poil sur mon qwerty, je la garde en reserve...
     en la reregardant je crois qu'elle etait debile, elle defibnit
     deux fois XNInputSyle avec deux valeurs diffrenetes...
    KbState_.input_context = 
        XCreateIC(dock->input_method, 
	      XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
	      XNInputStyle, XIMStatusNothing,
	      XNClientWindow, ew->win,
	      NULL);
  */

  /* celle-ci est parfaite sur l'azerty...
    KbState_.input_context = 
    XCreateIC(dock->input_method, XNInputStyle, 1032,
	      XNClientWindow, ew->win, XNFocusWindow, ew->win, 0);*/
#ifndef sun
  if (dock->input_method) {
    KbState_.input_context[idx] = 
      XCreateIC(dock->input_method, XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
		XNClientWindow, win, XNFocusWindow, win, NULL);

    if(KbState_.input_context[idx] == NULL) {
      fprintf(stderr, _("Warning : errot in XCreateIC() !\n"));
    }
  }
#endif
}

void kb_release_input_context(unsigned idx) {
#ifndef sun
  if (KbState_.input_context[idx])
    XDestroyIC(KbState_.input_context[idx]);
  KbState_.input_context[idx] = NULL;
#endif
}

void kb_build() {
  memset(&KbState_, 0, sizeof KbState_);
}
