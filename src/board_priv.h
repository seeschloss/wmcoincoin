#include "global.h"
#include <libintl.h>
#define _(String) gettext (String)

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include "coincoin.h"
#include "http.h"
#include "board_util.h"
#include "xmlcoincoin.h"

/* utilise tres localement, c'est la longueur DANS remote.xml, la longueur réelle sera moindre
   (remplacement de &eacute par 'é' etc... ) */
#define BOARD_UA_MAX_LEN 1000
#define BOARD_MSG_MAX_LEN 15000 /* on peut y arriver avec un bon gros message plein de [][][][]... */
#define BOARD_LOGIN_MAX_LEN 60

void convert_to_utf8(const char *src_encoding, char **psrc);
void release_md5_array(Board *b);
md5_and_time *find_md5_in_md5_array(md5_byte_t md5[16], md5_and_time *m);
void board_decode_message(Board *board, char *dest, const char *src);
board_msg_info *board_log_msg(Board *board, char *ua, char *login, 
                              char *stimestamp, char *_message, int64_t id, 
                              const unsigned char *my_useragent);
RSSBonusInfo *rss_register(Boards *boards, md5_byte_t mimd5[16], char *link, board_msg_info *mi);
int rss_board_update(Board *board, char *path);
void prelog_clear();
void prelog_add(char *ua, char *login, time_t tstamp, char *message, char *link, md5_byte_t md5[16], int already_viewed);
void prelog_commit(Board *board);
