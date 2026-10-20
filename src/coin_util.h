#ifndef COINCOIN_UTIL
#define COINCOIN_UTIL
#include "global.h"
#include <stdlib.h>
#include <stdio.h>
#include "md5.h"
//#include "raster.h"

#ifndef MAX
# define MAX(x,y) ((x)>(y)?(x):(y))
#endif
#ifndef MIN
# define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef SQR
#  define SQR(a) ((a)*(a))
#endif
#ifndef ABS
#  define ABS(a) ((a)>0?(a):-(a))
#endif
#define ALLOC_OBJ(p, t) { p = (t*) calloc(1, sizeof(t)); assert(p);}
#define FREE_OBJ(p) free(p);
#define ALLOC_VEC(p, n, t) { p = (t*) calloc(n, sizeof(t)); assert(p); }
#define SET_VEC(p, v, i0, i1) { int _i; \
                  for (_i = (i0); _i <= (i1); _i++) p[_i] = v; }
#define FREE_VEC(p) free(p);

/* alloc de tableau, p[0] est le pointeur de base, p[ni] est NULL */
#define ALLOC_ARR(p, ni, nj, t) { int _i; \
               p = (t**) calloc(ni+1, sizeof(t*)); assert(p); \
               p[0] = (t*) calloc((ni)*(nj), sizeof(t)); assert(p[0]); \
               for (_i = 1; _i < ni; _i++) { p[_i] = p[0]+((nj)*_i); } p[ni] = (t*)NULL;}
#define FREE_ARR(p) { free(p[0]); free(p); }
#define BASE_ARR(p) (p[0])

#ifndef NO_BLAHBLAH
# define BLAHBLAH(n,x) if (Prefs.verbosity >= n) { x; fflush(stdout); }
#else
# define BLAHBLAH(n,x)
#endif

#define MARK myprintf("%<YEL ******************************************>\n");




/* macro pour troll_data.h et str_hache */
#define CVINT(a,b,c,d) (a + (b<<8) + (c<<16) + (d<<24))

#define IS_INSIDE(x, y, xmin, ymin, xmax, ymax) ((x) >= (xmin) && (x) <= (xmax) && (y) >= (ymin) && (y) <= (ymax))


/* construit un 'nom' à partir des premiers mots du useragent */
void make_short_name_from_ua(const unsigned char *ua, unsigned char *name, int name_sz);
int mark_html_tags(char *s, int sz);
int convert_to_ascii(char *dest, const char *src, int dest_sz);
int str_is_empty(const char *s);
int xstrcmp(const char *a, const char *b);
char *str_multi_str(const char *src, const char **keys, int nb_keys, int *key_idx);
char *str_multi_substitute(const char *src, const char **keys, const char **substitutions, int nkeys);
char *str_substitute(const char *src, const char *key, const char *substitution);
char *shell_quote(const char *src);
int str_hache(const unsigned char *s, int max_len);
int str_hache_nocase(const unsigned char *s, int max_len);
void md5_digest(const char *s, md5_byte_t md5[16]);
const char * md5txt(md5_byte_t md5[16]);
unsigned char chr_noaccent_tolower(unsigned char c);
void str_noaccent_tolower(unsigned char *s);
void str_tolower(unsigned char *s);
unsigned char *str_noaccent_casestr(const unsigned char *meule, const unsigned char *aiguille);
char *str_printf(const char *fmt, ...) __attribute__ ((format (printf, 1, 2)));
#define STR_REPRINTF(_s, _xx); { char *__old = _s; _s = str_printf _xx; if (__old) free(__old); }
char *str_cat_printf(char *s, const char *fmt, ...) __attribute__ ((format (printf, 2, 3)));
unsigned char *str_case_str(const unsigned char *meule, const unsigned char *aiguille);
int str_ncasecmp(const unsigned char *a, const unsigned char *b, unsigned n);
int str_startswith(const unsigned char *a, const unsigned char *b);
int str_case_startswith(const unsigned char *a, const unsigned char *b);
char *str_ndup(const char *in, int n);
char *str_dup_unquoted(const char *s);
char* str_cat(char *s1, const char *s2);
char* str_ncat(char *s1, const char *s2, int n);
void str_trunc_nice(char *s, int approx_max_len);
time_t get_timezone();
int str_to_time_t(const char *s, time_t *tt);
void time_t_to_tstamp(time_t tim, char tstamp[15]);
char *str_fget_line(FILE *f);
void str_trim(unsigned char *s);
void str_rtrim_lf(unsigned char *s);
char *str_preencode_for_http(const char *in);
FILE *open_wfile(const char *fname);
int is_url(const char *s);
void url_au_coiffeur(unsigned char *url, int coupe);
void dump_backtrace();
void convert_to_iso8859(const char *src_encoding, char **psrc);
void convert_from_iso8859(const char *dest_encoding, char **psrc);
typedef struct strbuf_ {
  char *str;
  size_t len, max_sz;
} strbuf;
void strbuf_init(strbuf *sb, const char *s);
void strbuf_reserve_len(strbuf *sb, size_t len);
void strbuf_putc(strbuf *sb, char c);
void strbuf_cat(strbuf *sb, const char *s);
void strbuf_free(strbuf *sb);
void strbuf_ncat(strbuf *sb, const char *s, size_t n);
void strbuf_free(strbuf *sb);
typedef struct SplittedURL {
  char data[1000];
  enum { HTTP_URL, HTTPS_URL, POP3_URL, APOP_URL, FILE_URL } type;
  char *host;
  int port;
  int path_len;
  char *path[100];
} SplittedURL;
int split_url(const char *url, SplittedURL *d);
char *shorten_path(const char *s_);
#endif
