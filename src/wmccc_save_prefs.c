#include <signal.h>
#include "wmccc.h"
#include "options_list.h"
#include "coin_util.h"
#include <errno.h>

char *
miniuarule_to_string(MiniUARule *r) {
  int n = 0;
  char s[1024];
  int sz = 0;
  s[0] = 0; 
  if (r->site_name) { 
    sz += g_snprintf(s+sz, 1024-sz, "site:\"%s\"", r->site_name); n++; 
  }
  if (r->user_login) {
    sz += g_snprintf(s+sz, 1024-sz, "%slogin:\"%s\"", n?", ":"", r->user_login); n++; 
  }
  if (r->rgx) { 
    sz += g_snprintf(s+sz, 1024-sz, "%sua:\"%s\"", n?", ":"", r->s_rgx); n++; 
  }
  sz +=   g_snprintf(s+sz, 1024-sz, " => ");
  n = 0;
  if (r->color != -1) { 
    sz += g_snprintf(s+sz, 1024-sz, "color%s=\"#%06x\"", r->color_terminal?":":"", r->color); n++;
  }
  if (r->symb != -1) {
    sz += g_snprintf(s+sz, 1024-sz, "%ssymb%s=\"%s\"", n?", ":"", r->symb_terminal?":":"", symboles[r->symb].name); n++; 
  }
  if (r->rua) {
    sz += g_snprintf(s+sz, 1024-sz, "%sua%s=\"%s\"", n?", ":"", r->ua_terminal?":":"", r->rua); 
  }
  return g_strndup(s,1024);
}

void 
prefs_write_to_file_keylist(FILE *f, int opt, KeyList *hk) {
  if (hk == NULL) return;
  fprintf(f, "%s: ", wmcc_options_strings[opt]);
  for (; hk; hk = hk->next) {
    switch (hk->type) {
    case HK_UA: fprintf(f, "UA:"); break;
    case HK_LOGIN: fprintf(f, "LOGIN:"); break;
    case HK_WORD: fprintf(f, "WORD:"); break;
    case HK_ID: fprintf(f, "ID:"); break;
    case HK_THREAD: fprintf(f, "THREAD:"); break;
    case HK_UA_NOLOGIN: fprintf(f, "UA_NOLOGIN:"); break;
    default: g_assert(0); break;
    }
    fprintf(f, "%d:\"%s\"%s\n", hk->num, hk->key, hk->next ? ", \\" : "");
  }
}

void
prefs_savestr(FILE *f, const char *o, int with_dot, const char *s, const char *default_s) {
  if (s && s[0] && (default_s == NULL || strcmp(s,default_s))) {
    if (with_dot) fprintf(f, ".");
    fprintf(f, "%s: %s\n", o, s);
  }
}

void
prefs_savefontstyle(FILE *f, const char *o, int with_dot, FontStyle *fs, FontStyle *default_fs) {
  if (memcmp(fs,default_fs,sizeof(FontStyle))) {
    if (with_dot) fprintf(f, ".");
    fprintf(f, "%s: %s%s%s%s\n", o, 
            (fs->underlined ? "U" : "."), 
            (fs->slanted ? "I" : "."), 
            (fs->bold ? "B" : "."),
            (fs->teletype ? "T" : "."));
  }
}

#define SAVESTR(o,_s, p) if (p->_s && p->_s[0]) { fprintf(f, "%s: %s\n", o, p->_s); }
#define SAVEBOOL(o,_b, p) fprintf(f, "%s: %s\n", o, (p->_b)?"on":"off");
#define SAVEINT(o,_i, p) fprintf(f, "%s: %d\n", o, p->_i);
#define SAVEPOS(o,_x,_y, p) fprintf(f, "%s: %d:%d\n", o, p->_x, p->_y);
#define SAVECOL(o,_c, p) fprintf(f, "%s: %06x\n", o, p->_c);
#define SAVEBICOL(o, _bic, p) fprintf(f, "%s: %06x %06x\n", o, p->_bic.opaque, p->_bic.transp);


#define G_SAVESTR(o,_s) prefs_savestr(f, wmcc_options_strings[o], 0, p->_s, default_p->_s)
#define G_SAVEBOOL(o,_b) if (p->_b != default_p->_b) { SAVEBOOL(wmcc_options_strings[o], _b, p);}
#define G_SAVEINT(o,_i) if (p->_i != default_p->_i) { SAVEINT(wmcc_options_strings[o], _i, p); }
#define G_SAVEPOS(o,_x,_y) if (p->_x != default_p->_x || p->_y != default_p->_y) \
                           { SAVEPOS(wmcc_options_strings[o],_x,_y, p); }
#define G_SAVECOL(o,_c) if (p->_c != default_p->_c) { SAVECOL(wmcc_options_strings[o],_c, p); }
#define G_SAVEBICOL(o, _bic) if (p->_bic.opaque != default_p->_bic.opaque || p->_bic.transp != default_p->_bic.transp) { SAVEBICOL(wmcc_options_strings[o], _bic, p); }
#define G_SAVEFONTSTYLE(o, _fs) prefs_savefontstyle(f, wmcc_options_strings[o], 0, &(p->_fs), &(default_p->_fs))
#define G_SAVESTRLST(o,lst,nb) if (p->nb) { int i; fprintf(f, "%s: ", wmcc_options_strings[o]); \
      for (i=0; i < (int)p->nb; i++) { fprintf(f, "\"%s\"%s", p->lst[i], (i==(int)p->nb-1)?"\n":", \\\n"); } }
#define G_SAVEKL(o, hk) { prefs_write_to_file_keylist(f, o, p->hk); }

#define DOTIFY(o) (wmcc_options_strings[o]+1)

#define SP_SAVESTR(o,_s) prefs_savestr(f, DOTIFY(o), 1, sp->_s, default_sp->_s)
#define SP_SAVEBOOL(o,_b) if (sp->_b != default_sp->_b) { fprintf(f, ".");  SAVEBOOL(DOTIFY(o),_b, sp); }
#define SP_SAVEINT(o,_i) if (sp->_i != default_sp->_i) { fprintf(f, "."); SAVEINT(DOTIFY(o),_i, sp); }
#define SP_SAVEPOS(o,_x,_y) if (sp->_x != default_sp->_x || sp->_y != default_sp->_y) { fprintf(f, "."); SAVEPOS(DOTIFY(o),_x,_y, sp); }
#define SP_SAVECOL(o,_c) if (sp->_c != default_sp->_c) { fprintf(f, "."); SAVECOL(DOTIFY(o),_c, sp); }
#define SP_SAVEBICOL(o, _bic) if (sp->_bic.opaque != default_sp->_bic.opaque || sp->_bic.transp != default_sp->_bic.transp) { fprintf(f, "."); SAVEBICOL(DOTIFY(o), _bic, sp); }
#define SP_SAVEFONTSTYLE(o, _fs) prefs_savefontstyle(f, DOTIFY(o), 1, &(sp->_fs), &(default_sp->_fs))
void
auth_prefs_write_to_file(GeneralPrefs *p, FILE *f) {
  int site_num;
  for (site_num = 0; site_num < p->nb_sites; site_num++) {
    SitePrefs *sp = p->site[site_num];
    char *s = sp->user_cookie;
    while (s && *s) {
      char *ck, *s2;
      s2 = strchr(s, '\n');
      if (s2 == NULL) {
        ck = strdup(s);
      } else ck = g_strndup(s, s2-s);	
      g_assert(ck);
      if (*ck) {
        fprintf(f, "\"%s\" cookie: \"%s\"\n", sp->all_names[0], sp->user_cookie);
      }
      g_free(ck);
      s = s2 ? s2+1 : NULL;
    }
  }
}

void
prefs_write_to_file(GeneralPrefs *p, FILE *f) {
  char default_ua[1024];
  GeneralPrefs _default_p, *default_p;
  SitePrefs _default_sp, *default_sp;
  int site_num;

  /* prefs par defaut, pour reperer les valeurs modifiées */
  memset(&_default_p, 0, sizeof(_default_p));
  memset(&_default_sp, 0, sizeof(_default_sp));
  wmcc_prefs_set_default(&_default_p);
  wmcc_site_prefs_set_default(&_default_sp, 1);
  default_p = &_default_p; default_sp = &_default_sp;

  coincoin_default_useragent(default_ua, 1024);
  G_SAVEINT(OPT_verbosity_underpants, verbosity_underpants);
  G_SAVEINT(OPT_verbosity_http, verbosity_http);
  G_SAVESTR(OPT_font_encoding, font_encoding);
  G_SAVEINT(OPT_tribunenews_max_refresh_delay, max_refresh_delay);
  G_SAVEINT(OPT_tribunenews_switch_off_coincoin_delay, switch_off_coincoin_delay);
  G_SAVEBOOL(OPT_tribune_troll_detector, enable_troll_detector);
  G_SAVEBOOL(OPT_pinnipede_show_seconds, pp_show_sec_mode);
#ifdef HAVE_WMCOINCOIN_PLAYER  
  G_SAVEBOOL(OPT_board_sfw_pictures, board_sfw_pictures);
  G_SAVEBOOL(OPT_board_auto_dl_pictures, board_auto_dl_pictures);
  G_SAVEBOOL(OPT_board_enable_hfr_pictures, board_enable_hfr_pictures);
#endif
  G_SAVESTR(OPT_tribune_post_cmd, post_cmd[0]);
  G_SAVEBOOL(OPT_tribune_post_cmd_enabled, post_cmd_enabled[0]);
  G_SAVESTR(OPT_tribune_post_cmd2, post_cmd[1]);
  G_SAVEBOOL(OPT_tribune_post_cmd2_enabled, post_cmd_enabled[1]);
  G_SAVESTR(OPT_tribune_archive, board_scrinechote);
  G_SAVECOL(OPT_dock_bg_color, dock_bgcolor);
  G_SAVESTR(OPT_dock_bg_pixmap, dock_bgpixmap);
  G_SAVESTR(OPT_dock_skin_pixmap, dock_skin_pixmap);
  G_SAVECOL(OPT_dock_fg_color, dock_fgcolor);
  G_SAVEBOOL(OPT_dock_draw_border,draw_border);
  G_SAVEBOOL(OPT_dock_iconwin,use_iconwin);
  G_SAVEBOOL(OPT_palmipede_override_wmanager, palmipede_override_redirect);
  G_SAVEBOOL(OPT_palmipede_enable_scroll_wheel_for_boulets, palmipede_enable_scroll_wheel_for_boulets);
  G_SAVEBOOL(OPT_dock_use_balloons,use_balloons);
  G_SAVEBOOL(OPT_dock_auto_swallow,auto_swallow);
  G_SAVESTR(OPT_dock_balloons_font_family,balloon_fn_family);
  G_SAVEINT(OPT_dock_balloons_font_size,balloon_fn_size);

  G_SAVEBOOL(OPT_balltrap_enable, hunt_opened);
  G_SAVEINT(OPT_balltrap_max_ducks, hunt_max_duck);

  G_SAVEPOS(OPT_dock_pos, dock_xpos, dock_ypos);

  G_SAVEBOOL(OPT_dock_start_in_boss_mode, start_in_boss_mode);
  G_SAVESTR(OPT_palmipede_default_message, coin_coin_message);
  G_SAVESTR(OPT_http_browser,browser_cmd);
  G_SAVESTR(OPT_http_browser2,browser2_cmd);
  G_SAVESTR(OPT_http_gogole_search_url, gogole_search_url);
  G_SAVESTR(OPT_http_wikipedia_search_url, wikipedia_search_url);
  G_SAVEINT(OPT_http_timeout, http_timeout);
  G_SAVEINT(OPT_http_inet_ip_version, http_inet_ip_version);
  G_SAVESTR(OPT_pinnipede_font_family,pp_fn_family);
  G_SAVEINT(OPT_pinnipede_font_size,pp_fn_size);
  G_SAVEBOOL(OPT_dock_disable_xft_antialiasing, disable_xft_antialiasing);
  G_SAVEBOOL(OPT_pinnipede_start_in_transparency_mode,pp_start_in_transparency_mode);
  G_SAVEBOOL(OPT_pinnipede_use_fake_real_transparency,use_fake_real_transparency);
  G_SAVEBOOL(OPT_pinnipede_use_classical_tabs,pp_use_classical_tabs);
  G_SAVEBOOL(OPT_pinnipede_use_colored_tabs,pp_use_colored_tabs);
  G_SAVEINT(OPT_pinnipede_tabs_position,pp_tabs_pos);
  G_SAVEBOOL(OPT_pinnipede_hungry_boitakon,hungry_boitakon);

  if (memcmp(&p->pp_transparency, &default_p->pp_transparency, sizeof(p->pp_transparency))) {
    fprintf(f, "%s: ", wmcc_options_strings[OPT_pinnipede_transparency]);
    switch (p->pp_transparency.type) {
    case FULL_TRANSPARENCY: fprintf(f, "full\n"); break;
    case SHADING: fprintf(f, "shading %d %d\n", 
			  p->pp_transparency.shade.luminosite,
			  p->pp_transparency.shade.assombrissement); break;
    case TINTING: fprintf(f, "tinting %06x %06x\n", 
			  p->pp_transparency.tint.black,
			  p->pp_transparency.tint.white); break;
    default: g_assert(0); break;
    }
  }
  G_SAVEBICOL(OPT_pinnipede_emph_color, pp_emph_color);
  G_SAVEBICOL(OPT_pinnipede_sel_bgcolor, pp_sel_bgcolor);
  G_SAVEBICOL(OPT_pinnipede_popup_fgcolor, pp_popup_fgcolor);
  G_SAVEBICOL(OPT_pinnipede_popup_bgcolor, pp_popup_bgcolor);
  G_SAVEBICOL(OPT_pinnipede_buttonbar_bgcolor, pp_buttonbar_bgcolor);
  G_SAVEBICOL(OPT_pinnipede_buttonbar_fgcolor, pp_buttonbar_fgcolor);
  G_SAVEBICOL(OPT_pinnipede_buttonbar_msgcnt_color, pp_buttonbar_msgcnt_color);
  G_SAVEBICOL(OPT_pinnipede_buttonbar_updlcnt_color, pp_buttonbar_updlcnt_color);
  G_SAVEBICOL(OPT_pinnipede_buttonbar_progressbar_color, pp_buttonbar_progressbar_color);
  G_SAVEBICOL(OPT_pinnipede_hilight_my_msg_color, pp_my_msg_color);
  G_SAVEBICOL(OPT_pinnipede_hilight_answer_my_msg_color, pp_answer_my_msg_color);
  G_SAVEBICOL(OPT_pinnipede_hilight_keyword_color0, pp_keyword_color[0]);
  G_SAVEBICOL(OPT_pinnipede_hilight_keyword_color1, pp_keyword_color[1]);
  G_SAVEBICOL(OPT_pinnipede_hilight_keyword_color2, pp_keyword_color[2]);
  G_SAVEBICOL(OPT_pinnipede_hilight_keyword_color3, pp_keyword_color[3]);
  G_SAVEBICOL(OPT_pinnipede_hilight_keyword_color4, pp_keyword_color[4]);
  G_SAVEBICOL(OPT_pinnipede_plopify_color, pp_plopify_color);
  G_SAVEBICOL(OPT_scrollcoin_bg_color,sc_bg_color);
  G_SAVEBICOL(OPT_scrollcoin_bg_light_color,sc_bg_light_color);
  G_SAVEBICOL(OPT_scrollcoin_bg_dark_color,sc_bg_dark_color);
  G_SAVEBICOL(OPT_scrollcoin_arrow_normal_color,sc_arrow_normal_color);
  G_SAVEBICOL(OPT_scrollcoin_arrow_emphasized_color,sc_arrow_emphasized_color);
  G_SAVEBICOL(OPT_scrollcoin_bar_color,sc_bar_color);
  G_SAVEBICOL(OPT_scrollcoin_bar_light_color,sc_bar_light_color);
  G_SAVEBICOL(OPT_scrollcoin_bar_dark_color, sc_bar_dark_color);
  G_SAVEPOS(OPT_pinnipede_location,pp_xpos, pp_ypos);
  G_SAVEPOS(OPT_pinnipede_dimensions,pp_width, pp_height);
  G_SAVEBOOL(OPT_pinnipede_buttons,pp_minibar_on);
  G_SAVEBOOL(OPT_pinnipede_show_tags,pp_html_mode);
  G_SAVEBOOL(OPT_pinnipede_show_seconds,pp_show_sec_mode);
  G_SAVEINT(OPT_pinnipede_nick_mode,pp_nick_mode);
  G_SAVEBOOL(OPT_pinnipede_show_troll_score,pp_trollscore_mode);
  G_SAVEKL(OPT_pinnipede_plop_keywords,plopify_key_list);
  G_SAVEKL(OPT_pinnipede_hilight_keywords,hilight_key_list);
  G_SAVESTRLST(OPT_pinnipede_plop_words,plop_words,nb_plop_words);
  G_SAVEBOOL(OPT_palmipede_use_id_references, palmipede_use_id_references);

  {
    MiniUARule *r;
    for (r = p->miniuarules.first; r; r=r->next) {
      char *s = miniuarule_to_string(r);
      fprintf(f, "%s: %s\n", wmcc_options_strings[OPT_board_miniua_rule], s);
      g_free(s);
    }
  }

  {
    URLReplacement *ur;
    for (ur = p->url_repl.first; ur; ur=ur->next) {
      fprintf(f, "%s: \"%s\" => \"%s\"\n", wmcc_options_strings[OPT_pinnipede_url_replace], 
	      ur->key, ur->repl);
    }
  }

  G_SAVEBOOL(OPT_pinnipede_auto_open,pinnipede_open_on_start);

  G_SAVEBOOL(OPT_spell_enable,ew_do_spell);
  G_SAVESTR(OPT_spell_cmd,ew_spell_cmd);
  G_SAVESTR(OPT_spell_dict,ew_spell_dict);

  //g_print("nb_sites = %d\n", Prefs->nb_sites);
  for (site_num = 0; site_num < Prefs->nb_sites; site_num++) {
    SitePrefs *sp = Prefs->site[site_num];
    int i;
    //g_print("site = %s\n", sp->all_names[0]);
    switch (sp->backend_type) {
    case BACKEND_TYPE_BOARD:
      fprintf(f, "%s: ", wmcc_options_strings[OPT_board_site]); break;
    case BACKEND_TYPE_RSS:
      fprintf(f, "%s: ", wmcc_options_strings[OPT_rss_site]); break;
    case BACKEND_TYPE_POP:
      fprintf(f, "%s: ", wmcc_options_strings[OPT_pop_site]); break;
    default: assert(0);
    }
    for (i=0; i < MAX(4,sp->nb_names); i++) if (sp->all_names[i] && strlen(sp->all_names[i])) { fprintf(f, "%s \"%s\" ", i==0?"":",",sp->all_names[i]); }
    fprintf(f,"\n");
    
    SP_SAVEBOOL(OPTS_check_board, check_board);
    SP_SAVEBOOL(OPTSG_rss_ignore_description, rss_ignore_description);
    if (sp->locale != default_sp->locale) 
      fprintf(f, ".%s: %s\n", DOTIFY(OPTSG_locale), sp->locale == locFR ? "fr" : "en");
    SP_SAVEINT(OPTSG_tribune_delay, board_check_delay);
    SP_SAVEINT(OPTSG_tribune_max_messages, board_max_msg);
    SP_SAVEINT(OPTSG_backend_flavour, backend_flavour);
    SP_SAVESTR(OPTSG_tribune_wiki_emulation, board_wiki_emulation);
    SP_SAVESTR(OPTSG_palmipede_username, user_name);
    SP_SAVESTR(OPTSG_palmipede_userlogin, user_login);
    if (sp->user_agent == NULL || 
        (strcmp(sp->user_agent, default_ua) && strcmp(sp->user_agent, default_sp->user_agent))) {
      SP_SAVESTR(OPTSG_palmipede_useragent, user_agent);
    }
    SP_SAVEINT(OPTSG_palmipede_msg_max_length, palmi_msg_max_len);
    SP_SAVEINT(OPTSG_palmipede_useragent_max_length, palmi_ua_max_len);
    SP_SAVESTR(OPTSG_backend_url, backend_url);
    SP_SAVESTR(OPTSG_post_url, post_url);
    SP_SAVESTR(OPTSG_post_template, post_template);
    if (sp->proxy_name && strlen(sp->proxy_name)) {
      fprintf(f, ".%s: %s:%d\n", DOTIFY(OPTSG_http_proxy), sp->proxy_name, sp->proxy_port);
      if (sp->proxy_auth_user && strlen(sp->proxy_auth_user) &&
	  sp->proxy_auth_pass && strlen(sp->proxy_auth_pass)) {
	fprintf(f, ".%s: %s:%s\n", DOTIFY(OPTSG_http_proxy_auth), sp->proxy_auth_user, sp->proxy_auth_pass);
      } 
    }
    SP_SAVEBOOL(OPTSG_http_proxy_use_nocache, proxy_nocache);
    SP_SAVEBOOL(OPTSG_http_use_if_modified_since, use_if_modified_since);
    SP_SAVECOL(OPTSG_pinnipede_bg_color, pp_bgcolor);
    SP_SAVEBICOL(OPTSG_pinnipede_fg_color, pp_fgcolor);
    SP_SAVEBICOL(OPTSG_pinnipede_clock_color, pp_tstamp_color);
    SP_SAVEBICOL(OPTSG_pinnipede_useragent_color, pp_useragent_color);
    SP_SAVEBICOL(OPTSG_pinnipede_login_color, pp_login_color);
    SP_SAVEBICOL(OPTSG_pinnipede_url_color, pp_url_color);
    SP_SAVEBICOL(OPTSG_pinnipede_visited_url_color, pp_visited_url_color);
    SP_SAVEBICOL(OPTSG_pinnipede_trollscore_color, pp_trollscore_color);
    SP_SAVEBICOL(OPTSG_pinnipede_strike_color, pp_strike_color);
    SP_SAVEFONTSTYLE(OPTSG_pinnipede_clock_style, pp_clock_style);
    SP_SAVEFONTSTYLE(OPTSG_pinnipede_login_style, pp_login_style);
    SP_SAVEFONTSTYLE(OPTSG_pinnipede_useragent_style, pp_ua_style);
    SP_SAVEBOOL(OPTSG_pinnipede_mark_id_gaps, mark_id_gaps);
    SP_SAVEBOOL(OPTS_balltrap_enable, hunt_opened_on_site);
  }
}

int
save_prefs_as(gchar *filename, int do_backup) {
  FILE *f, *f_auth;
  char *tmpfname, *tmpfname_auth;
  char *filename_auth = g_strdup_printf("%s.auth", filename);
  if (do_backup) {
    tmpfname = g_strdup_printf("%s_wmccc_tmp", filename);
    tmpfname_auth = g_strdup_printf("%s_wmccc_tmp.auth", filename);
  } else {
    tmpfname = strdup(filename);
    tmpfname_auth = strdup(filename_auth);
  }


  f = open_wfile(tmpfname);f_auth = open_wfile(tmpfname_auth);
  if (f == NULL) {
    char *errmsg = g_strdup_printf("Can't save '%s' : %s", tmpfname, strerror(errno));
    quick_message(errmsg);
    g_free(errmsg);
    return -1;
  } else if (f_auth == NULL) {
    char *errmsg = g_strdup_printf("Can't save '%s' : %s", tmpfname_auth, strerror(errno));
    quick_message(errmsg);
    g_free(errmsg);
    return -1;
  } else {
    char *backup[4], *backup_auth[4];
    int i;
    fprintf(f, "### -*- mode: wmccoptions -*-\n### edited by wmccc -- look for *.wmccc.*.bak for backups\n");
    prefs_write_to_file(Prefs, f);
    fclose(f);
    fprintf(f_auth, "### -*- mode: wmccoptions -*-\n#\n");
    auth_prefs_write_to_file(Prefs, f_auth);
    fclose(f_auth);
    if (do_backup) {
      backup[0] = filename; backup_auth[0] = filename_auth;
      backup[1] = g_strdup_printf("%s.wmccc.bak", filename);
      backup[2] = g_strdup_printf("%s.wmccc.2.bak", filename);
      backup[3] = g_strdup_printf("%s.wmccc.3.bak", filename);
      backup_auth[1] = g_strdup_printf("%s.wmccc.auth.bak", filename);
      backup_auth[2] = g_strdup_printf("%s.wmccc.auth.2.bak", filename);
      backup_auth[3] = g_strdup_printf("%s.wmccc.auth.3.bak", filename);
      for (i=3; i >= 1; i--) {
	rename(backup[i-1], backup[i]);
	rename(backup_auth[i-1], backup_auth[i]);
      }
    
      if (rename(tmpfname, filename) == -1) {
	char *errmsg = g_strdup_printf("Couldn't rename '%s' to '%s' : %s", tmpfname, filename, strerror(errno));
	quick_message(errmsg);
	g_free(errmsg);
	rename(backup[1], backup[0]);
      }
      if (rename(tmpfname_auth, filename_auth) == -1) {
	char *errmsg = g_strdup_printf("Couldn't rename '%s' to '%s' : %s", tmpfname_auth, filename_auth, strerror(errno));
	quick_message(errmsg);
	g_free(errmsg);
	rename(backup_auth[1], backup_auth[0]);
      }
      for (i=1; i < 4; i++) {
	g_free(backup[i]);
	g_free(backup_auth[i]);
      }
    }
  }
  g_free(tmpfname);
  g_free(tmpfname_auth);
  g_free(filename_auth);
  return 0;
}

int apply_prefs() {
  if (save_prefs_as(glob.tmp_options_file, 0) == 0) {
    g_assert(glob.wmcc_pid > 0);
    kill(glob.wmcc_pid, SIGUSR2);
    return 0;
  } else g_assert(0);
  return 1;
}

int save_prefs() {
  return save_prefs_as(glob.options_file,1);
}

