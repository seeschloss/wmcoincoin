/*
  pdfm c'est pas une insulte, ça veut dire plateau de fruits de mer
  rcsid=$Id: pinnipede_pdfm.c,v 1.2 2002/10/05 18:08:14 pouaite Exp $
*/

#include "pinnipede.h"

/* --------------------------- section plateau de fruit de mer, veuillez insulter lordOric 
   pour tous les bugs liès à cette partie ;) ---------------------------------------------

   Nouveau ! maintenant on peut aussi insulter Shift !
*/

/* 23/03/2002 pitit patch pour protéger les > et < pour le validator */
static char *
pp_boardshot_encode( const char *chaine )
{
  char *tmp;
  char *retour;

  const char *key[] = { "<", ">", "&" };
  const char *subst[] = { "&lt;", "&gt;", "&amp;" };
  const char *key2[] = { "\t&lt;", "\t&gt;" };
  const char *subst2[] = { "<", ">" };
  
  if (chaine == NULL) return NULL;

  /* Passage des <> en &lt; et &gt; */
  tmp = str_multi_substitute( chaine, key, subst, 3 );
  
  /* Restauration des balises '<' et '>' précédés d'une tabulation (cad ceux qui sont
     d'authentiques tags */
  retour = str_multi_substitute( tmp, key2, subst2, 2 );
  free(tmp);
  return retour;
}

static char *make_sid(board_msg_info *mi)
{
  static char s[40];
  snprintf(s, 40, "%s%X", Prefs.site[id_type_sid(mi->id)]->site_name, id_type_lid(mi->id));
  return s;
}

/* --------- patch de lordOric, aka "plateau de fruits de mer" ------------*/
/* Scrinchote d'un message */
static int 
pp_boardshot_save_msg(Boards *boards, board_msg_info *mi, FILE *file, int use_js)
{
  char time[10];
  
  char *tmp;
  char *site_name = Prefs.site[id_type_sid(mi->id)]->site_name; 

  
  assert(file); assert(mi);
	
  snprintf(time, 10, "%02d:%02d:%02d",mi->hmsf[0], mi->hmsf[1], mi->hmsf[2]);
	
  if (use_js == 0) {
    fprintf(file, "<tr class=\"%s-msg\">", site_name);
  } else {
    fprintf(file, "<tr class=\"%s-msg\" id=\"%s\">", site_name, make_sid(mi));
  }
  if (mi->troll_score) {
    fprintf(file, "<td class=\"trollscore\">%d</td>\n", mi->troll_score);
  } else {
    fprintf( file, "<td></td>\n");
  }

  tmp = pp_boardshot_encode( mi->useragent );
  fprintf(file,"<td class=\"hour\" title=\"&lt;%s&gt; %s\">  %s</td>\n", site_name,tmp, time);	 
  free (tmp);
	
  if ( mi->login && strlen(mi->login)) {
    tmp = pp_boardshot_encode( mi->login );
    fprintf(file,"<td class=\"login\">%s</td>\n", tmp);
    free(tmp);
  } else {
    char *p;
    p = mi->miniua.name;
    tmp = pp_boardshot_encode(p);
    fprintf( file, "<td class=\"miniua\">%.12s</td>\n", tmp);
    free(tmp);
  }
	
  {
    const unsigned char *p, *np;
    fprintf( file,"<td>");
    p = mi->msg;
    while (p) {
      unsigned char s[768], attr_s[768];
      int has_initial_space, is_ref;
      board_msg_info *ref_mi;
      if (board_get_tok(&p,&np,s,768, &has_initial_space) == NULL) { break; }
      ref_mi = check_for_horloge_ref(boards, mi->id, s,attr_s, 768, &is_ref, NULL);
      if (has_initial_space) fprintf(file, " ");
      if (is_ref) {
	if (use_js == 0 || ref_mi == NULL) {
	  fprintf(file, "<span style=\"color:blue;\">%s</span>",s);
	} else {
	  fprintf(file, "<a onMouseOver=\"h('%s');\" onMouseOut=\"u('%s','#%06x');\" href=\"\">%s</a>",
		  make_sid(ref_mi), make_sid(ref_mi), Prefs.site[id_type_sid(ref_mi->id)]->pp_bgcolor, s);
	}
      } else {
	tmp = pp_boardshot_encode(s);
	fprintf(file, "%s", tmp); 
	free(tmp);
      }
      p = np;
    }
    fprintf( file,"</td></tr>\n");
  }

  
  return 0;
}



/* Tribuneshot CSS : La feuille de style du chouette plateau de fruits de mer ;) */
/* ndpouaite: pour tout reclamation, taper sur monsieur CapsLock (aka shift) */
int
pp_boardshot_css_file(const char *cssfname) 
{
  FILE *file;
  int j;

  /*
  file = fopen( cssfname, "r");
  file_exist = (file!=NULL);
  if ( file_exist ) fclose( file ); 
  */  
  file = fopen(cssfname, "w");
  if ( ! file ) {
    printf(_("Error while opening %s\n"), cssfname);
    return -1;
  }


  // Pas de décoration pour les liens
  fprintf( file, "a {\n text-decoration:none;\n }\n\n");
  fprintf( file, "A:HOVER {\n" /* piqué sur la homep de pycc ! */
	  "text-decoration: underline overline;\n"
	  "background-color: #fcfcda;\n"
	  "color: #1010da;\n}\n");
  // Titres de niveau 2 centrés
  fprintf( file, "h2 {\n text-align : center;\n }\n\n");

  // Tableau sans marge, ni bord et prenant toute la largeur
  fprintf( file, "table {\n border:none;\n"
		  	"width:100%%;\n"
			"padding:0px;\n"
			"margin:0px;\n"
			"border-spacing:0px;\n"
	   "}\n\n" );
  
  // On boucle sur les sites pour créés les propriétés CSS
  for (j = 0; j < MAX_SITES; j++) {
    char *site_name = NULL;
    if (Prefs.site[j] == NULL) continue;
    site_name = Prefs.site[j]->site_name;

    // Propriétés des message
    fprintf( file, ".%s-msg {\n", site_name);
    fprintf( file, "	background : #%06x; \n", Prefs.site[j]->pp_bgcolor); 
    fprintf( file, "	color : #%06x; \n", Prefs.site[j]->pp_fgcolor.opaque); 
    fprintf( file, "    vertical-align: top; \n");
    fprintf( file, "} \n\n");
    
    // Propriété des liens sous la souris
    fprintf( file, ".%s-msg a:hover {\n", site_name);
    fprintf( file, "	color: #%06x; \n",  Prefs.site[j]->pp_url_color.opaque); 
    fprintf(file, "} \n\n");
    
    // Propriétés des liens visités
    fprintf( file, ".%s-msg a:visited {\n", site_name);
    fprintf( file, "	color: #%06x; \n",  Prefs.site[j]->pp_visited_url_color.opaque); 
    fprintf(file, "} \n\n");
    
    // Propriétés des liens normaux
    fprintf( file, ".%s-msg a:link {\n", site_name);
    fprintf( file, "	color: #%06x; \n",  Prefs.site[j]->pp_url_color.opaque); 
    fprintf(file, "} \n\n");
    
    // Propriété des textes barrés
    //    fprintf( file, ".%s-msg s {\n", site_name);
    //    fprintf( file, "	color: #%06x; \n",  Prefs.site[j]->pp_strike_color.opaque); 
    //    fprintf(file, "} \n\n");
    
    // Propriété de l'ua
    fprintf( file, ".%s-msg .ua {\n", site_name);
    fprintf( file, "	color : #%06x; \n", Prefs.site[j]->pp_useragent_color.opaque); 	fprintf(file, "} \n\n");
    
    // Propriétés des scores au trolloscope
    fprintf( file, ".%s-msg .trollscore {\n", site_name);
    fprintf( file, "	color : #%06x; \n", Prefs.site[j]->pp_trollscore_color.opaque); 
    fprintf( file, "	font-weight : bold; \n");
    fprintf(file, "} \n\n");
    
    // Propriété des horloges
    fprintf( file, ".%s-msg .hour {\n", site_name);
    fprintf( file, "	color : #%06x; \n", Prefs.site[j]->pp_tstamp_color.opaque); 	
    fprintf(file, "} \n\n");
    
    // Propriété des logins
    fprintf( file, ".%s-msg .login {\n", site_name);
    fprintf( file, "	color : #%06x; \n", Prefs.site[j]->pp_login_color.opaque); 
    fprintf( file, "	font-weight : bold;\n"); 
    fprintf( file, "	text-align : center;\n"); 
    fprintf(file, "} \n\n");

    // Propriété des miniua
    fprintf( file, ".%s-msg .miniua {\n", site_name);
    fprintf( file, "	color : #%06x; \n", Prefs.site[j]->pp_useragent_color.opaque); 
    fprintf( file, "	text-align : center;\n"); 
    fprintf(file, "} \n\n");
  }
  
  fclose( file);
  return 0;
}


/* Tribuneshot : un chouette plateau de fruits de mer ;) */
int
pp_boardshot_kikoooo(Dock *dock, int save_all, int overwrite, int use_js) 
{
  Pinnipede *pp = dock->pinnipede;
  Boards *boards = dock->sites->boards;
  char *file_name;
  FILE *file;
  time_t time_shot;
  int file_exist,j;
  board_msg_info *msg;
  char *cssfname, *shortcssfname;

  file_name = str_substitute(Prefs.board_scrinechote, "~", getenv("HOME"));
  if (strlen(file_name)==0) return 1;
  file = fopen( file_name, "r");
  file_exist = (file!=NULL);
  if ( file_exist ) fclose( file ); 
  

  file = fopen( file_name, overwrite ? "w" : "a");
  if ( ! file ) {
    char errmsg[512];
    snprintf(errmsg, 512, _("Error while opening %s\n"), file_name);
    msgbox_show(dock, errmsg);
    free(file_name);
    return -1;
  }

  cssfname = strdup(file_name);
  {
    char *tmp;
    char *p = cssfname + strlen(cssfname)-1;
    while (p > cssfname && *p != '.' && *p != '/') p--;
    if (*p == '.') *p = 0;
    tmp = str_printf("%s.css", cssfname); free(cssfname);
    cssfname = tmp;
  }
  shortcssfname = strdup(cssfname);
  {
    char *pp;
    char *p = shortcssfname + strlen(shortcssfname)-1;
    while (p > shortcssfname && (*p == '/')) p--;
    pp = p;
    while (p > shortcssfname && *p != '/') p--;
    if (*p == '/') p++;
    if (strlen(p)) {
      *(pp+1) = 0;
      pp = shortcssfname;
      shortcssfname = strdup(p); free(pp);
    }
  }

  if (!file_exist || overwrite) {
    fprintf(file, 
	    "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
	    "<html><head><title>Scrinechote tribune</title>"
	    "<link rel=\"stylesheet\" type=\"text/css\" href=\"%s\" >"
	    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-15\">"
	    "</head>", shortcssfname);
    fprintf(file, "<body>\n");
    if (use_js) { /* g honte */
      fprintf(file,
	      "<script language=\"JavaScript\" type=\"text/javascript\">\n"
	      "function h(id) {\n"
	      "document.getElementById(id).style.background=\"#%06x\";\n}\n"
	      "function u(id,c)\n{"
	      "document.getElementById(id).style.background=c;\n}\n"
	      "</script>\n", Prefs.pp_emph_color.opaque);
    }
  }
  
  time( &time_shot );
  fprintf( file, "<br><br><center><h2> *** Scrinechote - %s *** </h2></center><br>", ctime( &time_shot ) );
  // Légende des couleurs
  for (j = 0; j < MAX_SITES; j++) {
    if (Prefs.site[j] && Prefs.site[j]->check_board) {
      char *site_name = Prefs.site[j]->site_name;
      int i, in_tabs = 0;
      for (i=0; i < pp->nb_tabs; i++) 
	if (pp->tabs[i].site->prefs == Prefs.site[j])
	  in_tabs = pp->tabs[i].selected;
      if (save_all || in_tabs)
	fprintf( file, "<span class=\"%s-msg\"> %s </span>", site_name, site_name);
    }
  }
  fprintf( file, "<table>");
  
  if (save_all) {
    msg = boards->first;
  } else {
    id_type id = get_first_id_filtered(boards, &pp->filter);
    msg = boards_find_id(boards, id);
  }
  while ( msg ) {
    pp_boardshot_save_msg(boards, msg, file, use_js);
    if (save_all) { 
      msg = msg->g_next;
    } else {
      get_next_id_filtered(boards, msg->id, &msg, &pp->filter);
    }
  }
  
  fprintf( file, "</table><br>\n");
  fclose( file);
  
  {
    char infomsg[512];
    snprintf(infomsg,512,"Scrinechote of the board (%s and %s) saved at %s", file_name, cssfname, ctime(&time_shot));
    msgbox_show(dock, infomsg);
  }
  // NEWSHIFT On créé le css
  pp_boardshot_css_file(cssfname);

  free(file_name);
  free(cssfname);
  return 0;
}
