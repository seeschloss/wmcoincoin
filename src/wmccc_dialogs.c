#include "wmccc.h"
#include "wmccc_interface.h"
#include "wmccc_support.h"
#include "myprintf.h"

struct {
  wmccc_dialog_t lst[100];
  int sz;
} dlgstack;

static GtkWidget *getdlg_(wmccc_dialog_t dlg, gboolean destroy) {
  static GtkWidget *w[NB_DLG] = {NULL,};
  if (destroy) {
    gtk_widget_destroy(w[dlg]); w[dlg] = 0;
  } else if (!w[dlg]) {
    switch (dlg) {
      case DLG_SITELIST: {
        w[dlg] = create_sitelist_dialog();
      } break;
      case DLG_NEW_BOARD: {
        w[dlg] = create_new_board_dialog();
      } break;
      case DLG_NEW_RSS: {
        w[dlg] = create_new_rss_dialog();
      } break;
      case DLG_NEW_POP: {
        w[dlg] = create_new_pop_dialog();
      } break;
      case DLG_BRONSON_WIZARD: {
        w[dlg] = create_bronson_wizard();
        g_signal_connect ((gpointer) bronson_wizard("wizard_new_board_bt"), "clicked",
                          G_CALLBACK (run_new_board_dialog), NULL);
        g_signal_connect ((gpointer) bronson_wizard("wizard_new_feed_bt"), "clicked",
                          G_CALLBACK (run_new_rss_dialog), NULL);
        g_signal_connect ((gpointer) bronson_wizard("wizard_new_pop_bt"), "clicked",
                          G_CALLBACK (run_new_pop_dialog), NULL);
        g_signal_connect ((gpointer) bronson_wizard("edit_options_bt"), "clicked",
                          G_CALLBACK (run_edit_dialog), NULL);
      } break;
      case DLG_EDIT_OPTIONS: {
        w[dlg] = create_edit_dialog();
      } break;
      case DLG_CHANGE_BOARD_SETTINGS: {
        w[dlg] = create_change_board_settings_dialog();
      } break;
      case DLG_CHANGE_RSS_SETTINGS: {
        w[dlg] = create_change_rss_settings_dialog();
      } break;
      case DLG_CHANGE_POP_SETTINGS: {
        w[dlg] = create_change_pop_settings_dialog();
      } break;
      case DLG_SITE_COLORS: {
        w[dlg] = create_pinnipede_site_colors_dialog();
      } break;
      case DLG_GLOBAL_PINNIPEDE_OPTIONS: {
        w[dlg] = create_global_pinnipede_options_dialog();
      } break;
      case DLG_PROXY: {
        w[dlg] = create_proxy_dialog();
      } break;
    default: assert(0);
    } /* switch */
  }
  return w[dlg];
}

GtkWidget *getdlg(wmccc_dialog_t dlg) { return getdlg_(dlg,FALSE); }

GtkWidget *releasedlg(wmccc_dialog_t dlg) { return getdlg_(dlg,TRUE); }

void wmccc_dialog_response_cb(GtkWidget *dlg, gint response, int idx);

void global_pinnipede_options_font_bt_cb() {
  GtkFontSelectionDialog *w = GTK_FONT_SELECTION_DIALOG(create_fontselectiondialog());
  /*char oldfnname[512];
    snprintf(oldfnname, sizeof oldfnname, "%s %d", Prefs->pp_fn_family, Prefs->pp_fn_size);*/

  PangoFontDescription *oldpfn = pango_font_description_new();
  pango_font_description_set_family(oldpfn, Prefs->pp_fn_family);
  pango_font_description_set_size(oldpfn, Prefs->pp_fn_size * PANGO_SCALE);
  //printf("old pango font selection : %s\n", pango_font_description_to_string(oldpfn));

  gtk_font_selection_dialog_set_font_name(w, pango_font_description_to_string(oldpfn));
  pango_font_description_free(oldpfn);
  if (gtk_dialog_run(GTK_DIALOG(w)) == GTK_RESPONSE_OK) {
    char *newfnname = gtk_font_selection_dialog_get_font_name(w);
    PangoFontDescription* pfn = pango_font_description_from_string(newfnname);
    //printf("new selection : %s:pixelsize=%d\n", pango_font_description_get_family(pfn), pango_font_description_get_size(pfn)/PANGO_SCALE);
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(getdlg(DLG_GLOBAL_PINNIPEDE_OPTIONS), "pp_fn_family")), 
                       pango_font_description_get_family(pfn));
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(lookup_widget(getdlg(DLG_GLOBAL_PINNIPEDE_OPTIONS), "pp_fn_size")), 
                              pango_font_description_get_size(pfn)/PANGO_SCALE);
    pango_font_description_free(pfn);
  }
  gtk_widget_destroy(GTK_WIDGET(w));
}

static void wmccc_initialize_dialog(wmccc_dialog_t dlg) {
  static int isinit[NB_DLG] = {0,};
  GtkWidget *w = getdlg(dlg);
  if (!isinit[dlg]) {
    g_signal_connect ((gpointer) getdlg(dlg), "response",
                      G_CALLBACK (wmccc_dialog_response_cb),
                      GINT_TO_POINTER(dlgstack.sz));
  }
  if (dlg != DLG_EDIT_OPTIONS && 
      my_lookup_widget(w, "apply_bt") && (dlgstack.sz > 1 || glob.wmcc_pid == 0)) {    
    gtk_widget_hide(my_lookup_widget(w, "apply_bt"));
  }
  switch (dlg) {
    case DLG_SITELIST: prepare_sitelist_dialog(isinit[dlg]); break;
    case DLG_NEW_BOARD: prepare_new_board_dialog(); break;
    case DLG_NEW_RSS: prepare_new_rss_dialog(); break;
    case DLG_NEW_POP: prepare_new_pop_dialog(); break;
    case DLG_BRONSON_WIZARD: prepare_bronson_wizard(isinit[dlg]); break;
    case DLG_EDIT_OPTIONS: prepare_edit_dialog(isinit[dlg]); break;
    case DLG_CHANGE_BOARD_SETTINGS: 
    case DLG_CHANGE_POP_SETTINGS: 
    case DLG_CHANGE_RSS_SETTINGS: 
      prepare_conf_dialog(w);
      assert(lookup_widget(w,"all_names_0_"));
      assert(my_lookup_widget(w,"all_names[0]"));
      break;
    case DLG_SITE_COLORS:  prepare_conf_dialog(w); break;
    case DLG_GLOBAL_PINNIPEDE_OPTIONS: 
      prepare_conf_dialog(w); 
      g_signal_connect(G_OBJECT(lookup_widget(w, "font_bt")), "clicked",
                       G_CALLBACK(global_pinnipede_options_font_bt_cb),
                       NULL);
      break;
    case DLG_PROXY:
      prepare_conf_dialog(w);
      break;
    default: assert(0);
  }
  isinit[dlg] = 1;
}


void wmccc_dialog_response_cb(GtkWidget *dlg, gint response, int idx) {
  //printf("wmccc_dialog_response_cb : response = %d , idx = %d\n", response, idx);
  assert(idx>= 0 && idx < dlgstack.sz);
  //if (idx == dlgstack.sz) { printf("PUTAIN DE BUG!\n"); return; }
  wmccc_dialog_t dlg_type = dlgstack.lst[idx];
  gboolean closedlg = TRUE;
  switch (dlg_type) {
    case DLG_SITELIST: {
      if (response == GTK_RESPONSE_CANCEL) {        
      } else if (response >= 0) {
        wmccc_initialize_dialog(dlg_type);
        closedlg = FALSE;
      }
      if (response != 1) {
        sitelist_update_site_order(); /* ne pas appeler apres un remove, ou explosion */
      }
    } break;
    case DLG_NEW_BOARD: {
      if (response == GTK_RESPONSE_OK) {
        closedlg = validate_new_board_dialog() ? FALSE : TRUE;
      }
    } break;
    case DLG_NEW_RSS: {
      if (response == GTK_RESPONSE_OK) {
        closedlg = validate_new_rss_dialog() ? FALSE : TRUE;
      }
    } break;
    case DLG_NEW_POP: {
      if (response == GTK_RESPONSE_OK) {
        closedlg = validate_new_pop_dialog() ? FALSE : TRUE;
      }
    } break;      
    case DLG_CHANGE_BOARD_SETTINGS:
    case DLG_CHANGE_RSS_SETTINGS:
    case DLG_CHANGE_POP_SETTINGS:
    case DLG_GLOBAL_PINNIPEDE_OPTIONS:
    case DLG_PROXY:
    case DLG_SITE_COLORS: {
      if (response == GTK_RESPONSE_OK || response == GTK_RESPONSE_APPLY) {
        finalize_conf_dialog(dlg);
      }
    } break;
    case DLG_EDIT_OPTIONS: {
      if (response == GTK_RESPONSE_OK || response == GTK_RESPONSE_APPLY) {
        if (edit_options_dialog_commit_changes_to_wmccc() == 0) {
          if (response == GTK_RESPONSE_OK) save_prefs();
          else apply_prefs();
        }
        closedlg = FALSE;
      }
    } break;
    default: break;
  }
  if (closedlg && dlgstack.sz == 1 && response == GTK_RESPONSE_APPLY) {
    closedlg = 0; apply_prefs();
  } else if (closedlg && dlgstack.sz == 1 && response == GTK_RESPONSE_OK) {
    closedlg = save_prefs() == 0;
  }
  //printf(" -> response = %d -- closedlg = %d\n", response, closedlg);
  if (closedlg) {
    if (idx >= 1) {
      dlgstack.sz = idx;
      GtkWidget *parent = getdlg(dlgstack.lst[dlgstack.sz-1]);
      //printf(" -> show dlg %d\n", dlgstack.lst[dlgstack.sz-1]);
      gtk_widget_show(parent); gtk_widget_set_sensitive(parent,TRUE);
    } else { gtk_main_quit(); }
    wmccc_initialize_dialog(dlgstack.lst[dlgstack.sz-1]);
    gtk_widget_hide(dlg); releasedlg(dlg_type);
  }
}

void wmccc_run_dialog(wmccc_dialog_t dlg, gboolean hide_parent) {
  GtkWidget *parent = dlgstack.sz > 0 ? getdlg(dlgstack.lst[dlgstack.sz-1]) : NULL;
  wmccc_initialize_dialog(dlg);
  if (hide_parent && parent) {
    gtk_widget_set_sensitive(parent, FALSE);
    gtk_widget_hide(parent);
  }
  
  g_signal_connect ((gpointer) getdlg(dlg), "response",
                    G_CALLBACK (wmccc_dialog_response_cb),
                    GINT_TO_POINTER(dlgstack.sz));
  dlgstack.lst[dlgstack.sz++] = dlg;
  gtk_widget_show(getdlg(dlg));
}
