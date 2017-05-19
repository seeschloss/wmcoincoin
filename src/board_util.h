#ifndef BOARD_UTIL_H
#define BOARD_UTIL_H

#include "coincoin.h"

board_msg_info *board_find_id(const Board *board, int id);
board_msg_info *board_find_md5(const Board *board, md5_byte_t md5[16]);
board_msg_info *boards_find_id(const Boards *boards, id_type id);
id_type boards_last_id(const Boards *b);
board_msg_info* board_find_previous_from_id(const Board *board, int id);
board_msg_info *board_find_previous(const Board *board, board_msg_info *mi);
char *board_get_tok(const unsigned char **p, const unsigned char **np, 
		    unsigned char *tok, int max_toklen, int *has_initial_space);
int board_msg_is_ref_to_me(Boards *boards, const board_msg_info *ref_mi);
board_msg_info *check_for_horloge_ref(Boards *boards, id_type caller_id, 
		      const unsigned char *ww, unsigned char *commentaire, 
		      int comment_sz, int *is_a_ref, int *ref_num);
board_msg_info *check_for_id_ref(Boards *boards, id_type caller_id, 
		      const unsigned char *ww, unsigned char *commentaire, 
		      int comment_sz, int *is_a_ref);

void board_msg_find_refs(Board *board, board_msg_info *mi);
KeyList *board_key_list_cleanup(Boards *boards, KeyList *first);
KeyList *board_key_list_test_mi(Boards *boards, board_msg_info *mi, KeyList *klist);
KeyList *board_key_list_test_mi_num(Boards *boards, board_msg_info *mi, KeyList *klist, int num);
KeyList *board_key_list_get_mi_positive_list(Boards *boards, board_msg_info *mi, 
					     KeyList *klist, int is_plop_list);

#endif
