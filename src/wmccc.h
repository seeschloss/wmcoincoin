#ifndef WMCCC_H
#define WMCCC_H

#define IN_WMCCC
#include <gtk/gtk.h>
#include "wmccc_gtk_compat.h"
#include "prefs.h"
#include "options_list.h"
#include "coin_util.h"


#ifdef __GNUC__
# define UNUSED __attribute((unused))
#else
# define UNUSED
#endif


typedef struct _strucGlob {
  GtkWidget *main_win;
  int current_site;
  int updating_labels;
  
  pid_t wmcc_pid;
  char *options_file;
  char *tmp_options_file;
  int nb_selected_sites;
  SitePrefs *selected_sites[MAX_SITES];
  GdkColor modif_widget_color;
} strucGlob;

#ifndef GLOBALS_HERE
extern GeneralPrefs *Prefs;
extern strucGlob glob;
#else
GeneralPrefs *Prefs;
strucGlob glob;
#endif

void quick_message(gchar *message_fmt, ...);
void prefs_write_to_file(GeneralPrefs *p, FILE *f);


typedef enum { DLG_SITELIST, DLG_NEW_BOARD, DLG_NEW_RSS, DLG_NEW_POP, 
               DLG_BRONSON_WIZARD, DLG_EDIT_OPTIONS, 
               DLG_CHANGE_BOARD_SETTINGS, DLG_CHANGE_RSS_SETTINGS, DLG_CHANGE_POP_SETTINGS, 
               DLG_SITE_COLORS, DLG_GLOBAL_PINNIPEDE_OPTIONS, DLG_PROXY, NB_DLG } wmccc_dialog_t;
GtkWidget *getdlg(wmccc_dialog_t dlg);
GtkWidget *my_lookup_widget(GtkWidget *widget, const gchar *widget_name_);
void wmccc_run_dialog(wmccc_dialog_t dlg, gboolean hide_parent);
void wmccc_set_dialog_hints(GtkWidget *dialog, GtkWidget *parent);
int edit_options_dialog_commit_changes_to_wmccc(void);
void sitelist_update_site_order(void);
int prepare_sitelist_dialog(int isinit);
int prepare_bronson_wizard(int isinit);
int prepare_new_board_dialog(void);
int prepare_new_rss_dialog(void);
int prepare_new_pop_dialog(void);
int prepare_edit_dialog(int isinit);
int prepare_conf_dialog(GtkWidget *w);
int finalize_conf_dialog(GtkWidget *w);
int validate_new_board_dialog(void);
int validate_new_rss_dialog(void);
int validate_new_pop_dialog(void);
int apply_prefs(void);
int save_prefs(void);
void run_new_board_dialog(void);
void run_new_rss_dialog(void);
void run_new_pop_dialog(void);
void run_edit_dialog(void);
GtkWidget *bronson_wizard(const char *widget_name);
#endif
