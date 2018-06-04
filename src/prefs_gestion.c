#ifdef __CYGWIN__
#include <getopt.h>
#endif
#include <sys/stat.h>
#include "coincoin.h"
#include "spell_coin.h"
#include "site.h"
#include "dock.h"

#include <libintl.h>
#define _(String) gettext (String)

#define TEST_CMDLINE_OPT(optid) { \
  char *errmsg = wmcc_prefs_validate_option(&Prefs,NULL, NULL, optid,optarg,0); \
  if (errmsg) { myprintf(_("Erreur pour l'option de la ligne de commande '%<GRN -%c>'\n: %s\n"),optc,errmsg); \
  exit(1); } }

static void
wmcc_prefs_from_cmdline(int argc, char **argv, GeneralPrefs *The_Prefs)
{
  int optc;

  /* tant que le 'kicker' de KDE ne sera pas capable de sauvegarder
     les options de ligne de commande d'une session a l'autre... */
  if (strstr(argv[0], "wmcoincoin-kde")) {
    myprintf(_("You invoqued wmcoincoin-kde : %<YEL anti KDE-bug mode activated>\n"));
    The_Prefs->use_iconwin = 0;
  }

  while ((optc = getopt(argc, argv, "hd:D:f:F:u:m:v:p:b:g::P:A:wBHx:r:s:C:X:c:o:Wl:L:")) !=-1) {
    switch (optc) {
    case 'h':
      {
	myprintf(_("Home page: %<BLU http:/hules.free.fr/wmcoincoin>\n"));

	myprintf(_("Dock applet pour WindowMaker -- affiche les news de %<GRN D>%<CYA a>%<GRN L>%<CYA inux>%<GRN F>%<CYA rench>%<GRN P>%<CYA age> (%<BLU http://linuxfr.org>)\n"));
	myprintf(_("OPTIONS (la plupart de ces options peuvent etre activees dans le\n"
		 " fichier '%<YEL ~/.wmcoincoin/options>'):\n"));
	myprintf(_(" %<GRN -h>\t\t: mouaif...\n"));
	//	myprintf(_(" %<GRN -d> %<CYA n>\t\t: fixe le delai entre deux checks de la tribune a %<CYA n> secondes (defaut %<grn %d>)\n"), The_Prefs->tribune_check_delay);
	//	myprintf(_(" %<GRN -D> %<CYA n>\t\t: fixe le delai entre deux checks des news a %<CYA n> secondes (default %<grn %d>)\n"), The_Prefs->news_check_delay);
	//	myprintf(_(" %<GRN -u> %<CYA ua>\t\t: change le user-agent\n"));
	//	myprintf(_(" %<GRN -P> %<CYA proxy:port>\t: utilise un proxy\n"));
	//	myprintf(_(" %<GRN -A> %<CYA username:password>: authentification pour le proxy, si necessaire\n"));
	myprintf(_(" %<GRN -m> %<CYA msg>\t\t: change le message poste sur la tribune (defaut: '%<grn %s>')\n"), The_Prefs->coin_coin_message);
	myprintf(_(" %<GRN -v> %<CYA n>\t\t: verbosite (defaut %<grn %d>) ,%<CYA n>=0,1,2,3\n"), The_Prefs->verbosity);
	//	myprintf(_(" %<GRN -p> %<CYA n>\t\t: ne s'interesse qu'aux news postees il y a moins de %<CYA n> jours\n"
	//		 "\t\t (defaut: %<grn %d> jours)\n"), The_Prefs->news_max_nb_days);
	myprintf(_(" %<GRN -b> %<CYA hexcoul>\t: couleur de fond, en RGB hexa (par defaut: %<grn %06x>, un magnifique(!) jaune)\n"), The_Prefs->dock_bgcolor);
	myprintf(_(" %<GRN -c> %<CYA hexcoul>\t: couleur du texte de l'applet, en RGB hexa (par defaut: %<grn %06x>)\n"), The_Prefs->dock_fgcolor);
	myprintf(_(" %<GRN -X> %<CYA file.xpm>\t: Fichier pixmap a mettre en fond du dock (fichier.xpm de 64x64)\n"));
	myprintf(_(" %<GRN -w> \t\t: utilise le mode windowed plutot que le mode icone\n"));
	myprintf(_("\t\tcette option est necessaire pour utiliser wmcoincoin avec le kicker de KDE (<3.0)\n"));
	myprintf(_("\t\tmais comme le kicker de KDE 2.1 a un bug qui l'empeche de sauver les options\n"));
	myprintf(_("\t\tde la ligne de commande d'une session a l'autre, le mieux est d'invoquer\n"));
	myprintf(_("\t\twmcoincoin sous le nom %<YEL wmcoincoin-kde> (c'est un bete lien symbolique)\n"));
	myprintf(_("\t\tet l'option -w sera automatiquement activee...\n"));
	myprintf(_(" %<GRN -B> \t\t: ajoute la bordure de wmcoincoin (pour windowmanager sans dock)\n"));
	myprintf(_(" %<GRN -x> %<CYA x:y>\t\t: essaye de positionner wmcoincoin en (x,y), cette option\n"
		 "\t\tn'est utile que si le windowmanager n'a pas de dock \n"
		 "\t\t(utiliser avec l'option %<GRN -B>)\n"));
#ifndef DISABLE_BALLOONS
	myprintf(_(" %<GRN -H> \t\t: desactive les ballons d'aide\n"));	
#endif
	//	myprintf(_(" %<GRN -r> %<CYA url>\t\t: se connecte sur un autre site dacode que linuxfr (defaut %<grn http://linuxfr.org:80>)\n"));
	myprintf(_(" %<GRN -s> %<CYA v>\t\t: vitesse de defilement du trolloscope (1 = rapide, 100 = 1 tres lent)\n(defaut %<grn %d>)\n"), The_Prefs->default_trollo_speed);
	/*	myprintf(_(" %<GRN -C> %<CYA cookie>\t: indique la valeur de votre cookie 'session_id' (il faut\n"
		 "\t\têtre authentifié). Cela permet à wmCoinCoin de détecter quand il y a une\n"
		 "\t\tnouvelle réponse à un de vos commentaires -- cette option est\n"
		 "\t\t tout à fait facultative\n"));*/
	myprintf(_(" %<GRN -o> %<CYA fichier>\t: indique le nom du fichier d'options a utiliser dans le\n"
		 "\t\t rep ~/.wmcoincoin (defaut '%<grn %s>')\n"), options_file_name);
	myprintf(_(" %<GRN -W>\t\t: ouvre le pinnipede des le lancement de wmcoincoin)\n"));
	myprintf(_(" %<GRN -l> %<CYA locale>\t: change the default locale (use -l fr to force wmcoincoin to speak french) (or set LC_MESSAGES properly)\n"));
	myprintf(_(" %<GRN -L> %<CYA locale_dir>\t: change the default directory of stored translations (default: %<grn %s>)\n"), LOCALEDIR);
	exit(0);
      } break;
      /*
    case 'd': TEST_CMDLINE_OPT(OPT_tribune_delay); break;
    case 'D': TEST_CMDLINE_OPT(OPT_news_delay); break;
    case 'f': TEST_CMDLINE_OPT(OPT_news_font_family); break;
    case 'F': TEST_CMDLINE_OPT(OPT_news_font_size); break; 
    case 'u': TEST_CMDLINE_OPT(OPT_palmipede_useragent); break;
    case 'm': TEST_CMDLINE_OPT(OPT_palmipede_default_message); break;
      */
    case 'v':
      {
	The_Prefs->verbosity = atoi(optarg); assert(The_Prefs->verbosity >= 0 && The_Prefs->verbosity <= 5);
      } break;
      /*
    case 'p':
      {
	The_Prefs->news_max_nb_days = atoi(optarg);
	assert(The_Prefs->news_max_nb_days > 0 && The_Prefs->news_max_nb_days < 100);
      } break;
    case 'b': TEST_CMDLINE_OPT(OPT_dock_bg_color); break;
    case 'c': TEST_CMDLINE_OPT(OPT_dock_fg_color); break;
    case 'X': TEST_CMDLINE_OPT(OPT_dock_bg_pixmap); break;
      */
    case 'g':
      {
	if (optarg == NULL) {
	  The_Prefs->debug = 1;
	} else The_Prefs->debug = atoi(optarg);
	/* | 1 : force X à synchroniser toutes ses op 
           | 2 : utilise les backends dans ~/wmcoincoin/test
           | 4 : ralenti considérablement la maj de l'animation de l'applet (pratique pour des displays exportés de très loin sur un modem)
           | 8 : désactive le fork sur le gethostbyname */
	myprintf(_("Activation of the secret %<YEL debug> mode (dbg=%d)!\n"), The_Prefs->debug);
      } break;
      /*
    case 'P': TEST_CMDLINE_OPT(OPT_http_proxy); break;
    case 'A': TEST_CMDLINE_OPT(OPT_http_proxy_auth); break;
      */
    case 'w': optarg = "0"; TEST_CMDLINE_OPT(OPT_dock_iconwin); printf("dock.iconwin: %d\n", Prefs.use_iconwin);break;
    case 'B': TEST_CMDLINE_OPT(OPT_dock_draw_border); break;
    case 'x': TEST_CMDLINE_OPT(OPT_dock_pos); break;
      /*
    case 'H': TEST_CMDLINE_OPT(OPT_dock_use_balloons); break;
    case 'r': TEST_CMDLINE_OPT(OPT_http_site_url); break;
    case 's':
      {
	The_Prefs->default_trollo_speed = 1 << (atoi(optarg)-1);
	//	printf("%s -> %d\n",optarg, The_Prefs->default_trollo_speed);
	if (The_Prefs->default_trollo_speed < 1 || The_Prefs->default_trollo_speed > 128) {
	  myprintf(_("option '-s' : la vitesse doit se situer en 1 et 8\n"));
	  exit(1);
	}
      } break;
    case 'C' : TEST_CMDLINE_OPT(OPT_http_cookie); break;
      */
    case 'W' : The_Prefs->pinnipede_open_on_start = 1; break; /* auto ouvre le pinnipede au lancement */
    case 'o': break; /* cette option est traitée dans init_default_prefs (cad AVANT la lecture du fichier d'options) */
    case 'l': break; /* cette option est traitée au tout début de la fontion main() */
    case 'L': break; /* cette option est traitée au tout début de la fontion main() */
    case ':':
    case '?':
    default:
      printf("obsolete option : '-%c'\n", optc);
      exit(0); break;   
    }
  }
}

 
static void
check_wmcoincoin_prefs_dir()
{
  char rpath[2048];
  struct stat stbuf;
  int is_ok;

  snprintf(rpath,2048,"%s/.wmcoincoin", getenv("HOME"));
  is_ok = 1;
  if (stat(rpath, &stbuf) == 0) {
    if (!S_ISDIR(stbuf.st_mode)) {
      fprintf(stderr, _("the file '%s' is not a directory !!! trash it please\n"), rpath);
      is_ok = 0;
    }
  } else {
    if (errno == ENOENT) {
      myfprintf(stderr, _("creating '%<YEL %s>'..\n"), rpath);
      if (mkdir(rpath,-1)) {
	myfprintf(stderr, _("can't create the directory '%s'\n[error: '%<red %s>']\n"),
		 rpath, strerror(errno));
	is_ok = 0;
      }
    } else {
      fprintf(stderr, _("There's something strange with the directory '%s' [err=%s]\n"), rpath, strerror(errno));
      is_ok = 0;
    }
  }
  if (is_ok == 0) {
    exit(1);
  }
}

/* alors celle-là est vraiment à chier */
int
copy_file(char *dest, char *src) {
  FILE *in, *out;
  int c;
  
  in = fopen(src, "r");
  if (!in) {
    fprintf(stderr, _("can't read '%s' : %s\n"), src, strerror(errno)); return -1;
  }
  out = open_wfile(dest);
  if (!out) {
    fprintf(stderr, _("can't write '%s' : %s\n"), dest, strerror(errno)); return -1;
  }

  while ((c=fgetc(in)) != EOF) {
    fputc(c, out);
  }
  fclose(in); fclose(out);
  return 0;
}


/* installe un fichier de WMCCDATADIR dans le ~/.wmcoincoin si nécessaire */
char *
check_install_data_file(char *data_file_name, char *dot_wmcc_file_name)
{
  char *dot_name;
  char *data_name;
  struct stat stbuf;
  int use_data = 0;


  dot_name = (dot_wmcc_file_name[0] != '/' && dot_wmcc_file_name[0] != '.') ?
    str_printf("%s/.wmcoincoin/%s", getenv("HOME"), dot_wmcc_file_name) : strdup(dot_wmcc_file_name);
  data_name = str_printf(WMCCDATADIR "/%s", data_file_name);

  if (stat(dot_name, &stbuf)) {
    fprintf(stderr, _("ah ben y'a pas le fichier '%s', on va le creer\n"), dot_name);
    copy_file(dot_name, data_name);
  }

  if (stat(dot_name, &stbuf)) {
    fprintf(stderr, _("\ndesole, demerdez-vous pour copier '%s' dans votre ~/.wmcoincoin ..\n"), data_file_name);
    use_data = 1;
  } else if (!S_ISREG(stbuf.st_mode)) {
    fprintf(stderr, _("\narretez de faire n'importe quoi dans votre ~/.wmcoincoin..\n\n"));
    use_data = 1;
  }

  if (use_data) {
    if (stat(data_name, &stbuf)) {
      fprintf(stderr, _("wmcoincoin a du etre mal installe, je ne trouve pas le fichier '%s' par defaut dans le rep '%s'...\n"),
	      data_name,WMCCDATADIR);
      exit(1);
    } else {
      fprintf(stderr, _("utilisation du fichier se trouvant dans '%s'\n"), data_name);
    }
    free(dot_name); return data_name;
  } else {
    free(data_name); return dot_name;
  }
}

void
wmcc_prefs_initialize(int argc, char **argv, GeneralPrefs *p)
{
  int i;
  char *errmsg;
  char *options_full_file_name;
  wmcc_prefs_set_default(p);
  ASSIGN_STRING_VAL(options_file_name, "options");
  /* pas très beau : préanalyse de la ligne de command pour détecter l'usage
     d'un fichier d'options autre que celui par défaut */
  for (i=1; i < argc-1; i++) {
    if (strcmp(argv[i], "-o") == 0) {
      assert(argv[i+1]);
      ASSIGN_STRING_VAL(options_file_name, argv[i+1]);
      myprintf(_("using option file '%<yel %s>'\n"), argv[i+1]);
      break;
    }
  }

  /* creation de ~/.wmcoincoin si necessaire */
  check_wmcoincoin_prefs_dir();

  options_full_file_name = check_install_data_file("options", options_file_name);
  assert(options_full_file_name);
  errmsg = wmcc_prefs_read_options(p, options_full_file_name,0); /* lecture de '.wmcoincoin/options' */
  if (errmsg) {
    myprintf(_("Error during while reading '%s':\n%<YEL %s>\n") , options_full_file_name, errmsg);
    free(errmsg); exit(1);
  }
  free(options_full_file_name);
  wmcc_prefs_from_cmdline(argc, argv, p);
}

/* man fonction_a_la_con */
static int
str_copy_if_changed(char **a, char *b) {
  if (*a == NULL && b == NULL) {
    return 0;
  } if (*a == NULL && b) {
    *a = strdup(b);
    return 1;
  } else if (*a && b == NULL) {
    FREE_STRING(*a); return 1;
  } else if (strcmp(*a,b)) {
    FREE_STRING(*a); *a = strdup(b); return 1;
  }
  return 0;
}

static int
int_copy_if_changed(int *a, int b) {
  if (*a != b) {
    *a = b; return 1;
  } else return 0;
}

static int
bic_copy_if_changed(BiColor *a, BiColor b) {
  if (a->opaque != b.opaque || a->transp != b.transp) {
    *a = b; return 1;
  } else return 0;
}

static int
transp_copy_if_changed(TransparencyInfo *a, TransparencyInfo b) {
  int different = 0;
  
  if (a->type != b.type) different = 1;
  else {
    switch (b.type) {
    case FULL_TRANSPARENCY:
      break;
    case SHADING:
      if (a->shade.luminosite != b.shade.luminosite ||
	  a->shade.assombrissement != b.shade.assombrissement) different = 1;
      break;
    case TINTING:
      if (a->tint.white != b.tint.white ||
	  a->tint.black != b.tint.black) different = 1;
      break;
    default: assert(0);
    }
  }
  if (different) { *a = b; }
  return different;
}

static int
font_style_copy_if_changed(FontStyle *a, FontStyle b)
{
  if (a->underlined != b.underlined ||
      a->slanted != b.slanted ||
      a->bold != b.bold ||
      a->teletype != b.teletype) {
    a->underlined = b.underlined;
    a->slanted = b.slanted;
    a->bold = b.bold;
    a->teletype = b.teletype;
    return 1;
  } else return 0;
}

static int
key_list_copy_if_changed(KeyList **a, KeyList *b)
{
  KeyList *hk;
  int changed = 0;

  hk = b;
  /* ajoute les nouvelle */
  while (hk) {
    KeyList *hka;
    if ((hka=key_list_find(*a, hk->key, hk->type)) == NULL) {
      changed = 1;
      *a = key_list_add(*a, hk->key, hk->type, hk->num, hk->from_prefs);
    } else if (hka->num != hk->num) {
      changed = 1;
      /* on l'enleve puis la rajoute pour assurer un tri dans le bon ordre */
      *a = key_list_remove(*a, hk->key, hk->type);
      *a = key_list_add(*a, hk->key, hk->type, hk->num, hk->from_prefs);
    }
    hk = hk->next;
  }
  hk = *a;
  /* vire les anciennes */
  while (hk) {
    KeyList *hkb;
    if (hk->from_prefs) {
      if ((hkb = key_list_find(b, hk->key, hk->type)) == NULL) {
	changed = 1;
	*a = key_list_remove(*a, hk->key, hk->type);
	hk = *a; continue;
      }
    }
    hk = hk->next;
  }
  //printf(_("key_list changed: %d\n"), changed);
  return changed;
}


static int
string_list_copy_if_changed(char ***a, int *na, char * const *b, const int nb)
{
  int changed = 0, i;
  
  if (*na != nb) changed = 1;
  else {
    for (i=0; i < *na; i++) {
      if (strcasecmp((*a)[i], b[i]) != 0) { changed = 1; break; }
    }
  }
  if (changed) {
    free(*a);
    *a = calloc(nb, sizeof(char*));
    for (i=0; i < nb; i++) {
      (*a)[i] = strdup(b[i]);
    }
  }
  return changed;
}

#define INT_OPT_CHANGED(_a, _b, _x) (_a._x != (int)_b._x)
#define G_INT_OPT_CHANGED(_x) INT_OPT_CHANGED(Prefs,newPrefs,_x)
#define SP_INT_OPT_CHANGED(_x) INT_OPT_CHANGED((*p),(*np),_x)

#define STR_OPT_CHANGED(_a,_b,_x) ((_a._x == NULL && _b._x) || \
                             (_a._x && _b._x == NULL) || \
                             ((_a._x != NULL && _b._x != NULL) \
                              && strcmp(_a._x, _b._x)))
#define G_STR_OPT_CHANGED(_x) STR_OPT_CHANGED(Prefs,newPrefs,_x)
#define SP_STR_OPT_CHANGED(_x) STR_OPT_CHANGED((*p),(*np),_x)

#define INT_OPT_COPY(_a,_b,_x) {_a._x = (int)_b._x; }
#define G_INT_OPT_COPY(_x) INT_OPT_COPY(Prefs,newPrefs,_x)
#define SP_INT_OPT_COPY(_x) INT_OPT_COPY((*p),(*np),_x)

#define STR_OPT_COPY(_a,_b,_x) {FREE_STRING(_a._x); if (_b._x) _a._x = strdup(_b._x); }
#define G_STR_OPT_COPY(_x) STR_OPT_COPY(Prefs,newPrefs,_x)
#define SP_STR_OPT_COPY(_x) STR_OPT_COPY((*p),(*np),_x)

#define STR_OPT_COPY_IF_CHANGED(_a,_b,_x) (str_copy_if_changed(&_a._x, _b._x))
#define G_STR_OPT_COPY_IF_CHANGED(_x) STR_OPT_COPY_IF_CHANGED(Prefs,newPrefs,_x)
#define SP_STR_OPT_COPY_IF_CHANGED(_x) STR_OPT_COPY_IF_CHANGED((*p),(*np),_x)

#define INT_OPT_COPY_IF_CHANGED(_a,_b,_x) (int_copy_if_changed(&_a._x, _b._x))
#define G_INT_OPT_COPY_IF_CHANGED(_x) INT_OPT_COPY_IF_CHANGED(Prefs,newPrefs,_x)
#define SP_INT_OPT_COPY_IF_CHANGED(_x) INT_OPT_COPY_IF_CHANGED((*p),(*np),_x)

#define BIC_OPT_CHANGED(_a,_b,_x) ((_a._x.opaque != (int)_b._x.opaque) || \
                             (_a._x.transp != (int)_b._x.transp))
#define G_BIC_OPT_CHANGED(_x) BIC_OPT_CHANGED(Prefs,newPrefs,_x)
#define SP_BIC_OPT_CHANGED(_x) BIC_OPT_CHANGED((*p),(*np),_x)

#define BIC_OPT_COPY_IF_CHANGED(_a,_b,_x) (bic_copy_if_changed(&_a._x, _b._x))
#define G_BIC_OPT_COPY_IF_CHANGED(_x) BIC_OPT_COPY_IF_CHANGED(Prefs,newPrefs,_x)
#define SP_BIC_OPT_COPY_IF_CHANGED(_x) BIC_OPT_COPY_IF_CHANGED((*p),(*np),_x)

#define TRANSP_OPT_COPY_IF_CHANGED(_a,_b,_x) (transp_copy_if_changed(&_a._x, _b._x))
#define G_TRANSP_OPT_COPY_IF_CHANGED(_x) TRANSP_OPT_COPY_IF_CHANGED(Prefs,newPrefs,_x)

#define KEY_LIST_COPY_IF_CHANGED(_a,_b,_x) (key_list_copy_if_changed(&_a._x, _b._x))
#define G_KEY_LIST_COPY_IF_CHANGED(_x) KEY_LIST_COPY_IF_CHANGED(Prefs,newPrefs,_x)

#define FONTSTYLE_COPY_IF_CHANGED(_a,_b,_x) (font_style_copy_if_changed(&_a._x, _b._x))
#define SP_FONTSTYLE_COPY_IF_CHANGED(_x) FONTSTYLE_COPY_IF_CHANGED((*p),(*np),_x)

#define STRING_LIST_COPY_IF_CHANGED(_a,_b,_x,_nb) (string_list_copy_if_changed(&_a._x, &_a._nb, \
   _b._x, _b._nb))
#define G_STRING_LIST_COPY_IF_CHANGED(_x,_nb) STRING_LIST_COPY_IF_CHANGED(Prefs,newPrefs,_x,_nb)
char *get_wmcc_tmp_options_filename() {
  return str_printf("%s/.wmcoincoin/wmcc_%u.tmp", getenv("HOME"), getpid());
}

char *get_wmcc_options_filename() {
  
  return (options_file_name[0] != '/' && options_file_name[0] != '.') ? 
    str_printf("%s/.wmcoincoin/%s", getenv("HOME"), options_file_name) : 
    strdup(options_file_name);  
}

/* c'est un peu bourrin comme approche mais ça devrait marcher..*/
void
wmcc_prefs_relecture(Dock *dock, int whatfile)
{
  GeneralPrefs newPrefs;
  char *errmsg;
  char *options_full_file_name;

  memset(&newPrefs, 0, sizeof(GeneralPrefs));
  wmcc_prefs_set_default(&newPrefs);
  if (whatfile == 1) {
    options_full_file_name = check_install_data_file("options", options_file_name);
  } else {
    options_full_file_name = get_wmcc_tmp_options_filename();
  }
  errmsg = wmcc_prefs_read_options(&newPrefs, options_full_file_name,0);
  if (errmsg) {
    char *s = str_printf(_("Error while rereading options [%s]<br>%s"),
			 options_full_file_name, errmsg); free(errmsg);
    msgbox_show(dock, s); free(s);
  } else {
    int redraw_pinni = 0;
    int rebuild_pinni = 0;
    int rebuild_palmi = 0;
    int close_palmi = 0;
    int reset_dock_pix = 0;
    int i;

    myprintf(_("rereading of options '%<YEL %s>' successful\n"), options_full_file_name);
    /*printf("VIREZ MOIVIREZ MOIVIREZ MOIVIREZ MOIVIREZ MOIVIREZ MOIVIREZ MOI\n"
           "sites=%s,%s,%s..\n", newPrefs.site[0]->site_name,newPrefs.site[1]->site_name,newPrefs.site[2]->site_name);
    */
    if (editw_ismapped(dock->editw)) editw_unmap(dock, dock->editw);

    /* recopie bête de certaines options modifiables sans difficultes */
    
    /* parametres de refresh */
    G_INT_OPT_COPY(max_refresh_delay);
    G_INT_OPT_COPY(switch_off_coincoin_delay);
    G_INT_OPT_COPY(verbosity_underpants);
    G_INT_OPT_COPY(verbosity_http);
    G_INT_OPT_COPY(http_inet_ip_version);
    G_INT_OPT_COPY(use_balloons);
    G_INT_OPT_COPY(pp_use_classical_tabs);
    G_INT_OPT_COPY(pp_use_colored_tabs);

    /* l'ensemble des options http */
    G_INT_OPT_COPY(http_timeout);
    G_STR_OPT_COPY(browser_cmd);
    G_STR_OPT_COPY(browser2_cmd);

    G_INT_OPT_COPY(enable_troll_detector);
    G_INT_OPT_COPY(board_sfw_pictures);
    G_INT_OPT_COPY(board_auto_dl_pictures);
    G_INT_OPT_COPY(board_enable_hfr_pictures);
    G_INT_OPT_COPY(board_enable_hfr_pictures);

    /* spell */
    if (G_INT_OPT_COPY_IF_CHANGED(ew_do_spell) +
	G_STR_OPT_COPY_IF_CHANGED(ew_spell_cmd) +
	G_STR_OPT_COPY_IF_CHANGED(ew_spell_dict)) {
      check_if_should_kill_ispell(1);
    }

    for (i=0; i < NB_BIGORNO; ++i)
      G_STR_OPT_COPY(post_cmd[i]);
    G_STR_OPT_COPY(board_scrinechote);

    G_INT_OPT_COPY(palmipede_enable_scroll_wheel_for_boulets);
    G_INT_OPT_COPY(palmipede_use_id_references);

    /* à faire: plop_words */

    /* maintenant les options generales qui demande un peu de boulot */

    if (G_INT_OPT_COPY_IF_CHANGED(palmipede_override_redirect))
      close_palmi = 1;

    if (G_INT_OPT_COPY_IF_CHANGED(pp_tabs_pos) +
        G_BIC_OPT_COPY_IF_CHANGED(sc_bg_color) +
        G_BIC_OPT_COPY_IF_CHANGED(sc_bg_light_color) +
	G_BIC_OPT_COPY_IF_CHANGED(sc_bg_dark_color) + 
	G_BIC_OPT_COPY_IF_CHANGED(sc_arrow_normal_color) +
	G_BIC_OPT_COPY_IF_CHANGED(sc_arrow_emphasized_color) +
	G_BIC_OPT_COPY_IF_CHANGED(sc_bar_color) +
	G_BIC_OPT_COPY_IF_CHANGED(sc_bar_light_color) +
	G_BIC_OPT_COPY_IF_CHANGED(sc_bar_dark_color)) {
      rebuild_pinni = 1;
    }

    /* déplacement du dock dans le cas ou la fenetre est en override redirect */
    if ((G_INT_OPT_COPY_IF_CHANGED(dock_xpos) + G_INT_OPT_COPY_IF_CHANGED(dock_ypos)) && 
	Prefs.draw_border && Prefs.use_iconwin==0) {
      XMoveWindow(dock->display, DOCK_WIN(dock), newPrefs.dock_xpos, newPrefs.dock_ypos);
    }

    if (G_STR_OPT_COPY_IF_CHANGED(dock_bgpixmap) + G_INT_OPT_COPY_IF_CHANGED(dock_bgcolor)
	+ G_INT_OPT_COPY_IF_CHANGED(dock_fgcolor)) {
      reset_dock_pix = 1;
    }
    
    if (G_INT_OPT_COPY_IF_CHANGED(disable_xft_antialiasing)) {
      rebuild_pinni = 1;
      rebuild_palmi = 1;
    }
    
    /* test sur les grosses options du pinnipede */
    if (G_STR_OPT_COPY_IF_CHANGED(pp_fn_family) +
	G_INT_OPT_COPY_IF_CHANGED(pp_fn_size) +
	G_INT_OPT_COPY_IF_CHANGED(board_totoz_server) +
        G_INT_OPT_COPY_IF_CHANGED(pp_show_sec_mode) +
        G_INT_OPT_COPY_IF_CHANGED(pp_html_mode)) {
      rebuild_pinni = 1;
    }

    if (G_KEY_LIST_COPY_IF_CHANGED(plopify_key_list) +
	G_INT_OPT_COPY_IF_CHANGED(hungry_boitakon)) {
      boards_update_boitakon(dock->sites->boards);
      redraw_pinni = 1;
    }

    //G_INT_OPT_COPY_IF_CHANGED(hunt_opened);
    G_INT_OPT_COPY_IF_CHANGED(hunt_max_duck);

    /* les options plus light se négocient avec un bon gros refresh */
    if (G_BIC_OPT_COPY_IF_CHANGED(pp_buttonbar_bgcolor) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_buttonbar_fgcolor) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_buttonbar_msgcnt_color) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_buttonbar_updlcnt_color) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_buttonbar_progressbar_color) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_emph_color) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_sel_bgcolor) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_popup_fgcolor) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_popup_bgcolor) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_my_msg_color) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_answer_my_msg_color) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_keyword_color[0]) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_keyword_color[1]) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_keyword_color[2]) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_keyword_color[3]) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_keyword_color[4]) +
        G_BIC_OPT_COPY_IF_CHANGED(pp_plopify_color) +
	G_TRANSP_OPT_COPY_IF_CHANGED(pp_transparency) +
        G_INT_OPT_COPY_IF_CHANGED(use_fake_real_transparency) +
	G_KEY_LIST_COPY_IF_CHANGED(hilight_key_list) +
	G_STRING_LIST_COPY_IF_CHANGED(plop_words,nb_plop_words)) {
      redraw_pinni = 1;
    }

    /* --------- test des options par site --------- */

    /* modification d'un site ? */
    
    for (i=0; i < MAX_SITES; i++) {
      SitePrefs *p, *np;
      int inp;
      if (Prefs.site[i] == NULL) continue;
      p = Prefs.site[i];
      //np = wmcc_prefs_find_site(&newPrefs, p->site_name); 
      //if (np == NULL) continue;
      inp = wmcc_prefs_find_site_id(&newPrefs, p->site_name); 
      if (inp == -1) continue;
      np = newPrefs.site[inp];

      /* test du changement d'ordre dans les sites */
      if (inp != i) {
        rebuild_pinni = 1;
      }

      SP_INT_OPT_COPY(rss_ignore_description);
      SP_INT_OPT_COPY(board_check_delay);
      SP_INT_OPT_COPY(board_max_msg);
      SP_INT_OPT_COPY(backend_type);
      SP_INT_OPT_COPY(proxy_nocache);
      SP_INT_OPT_COPY(use_if_modified_since);
      SP_STR_OPT_COPY(user_agent);
      SP_STR_OPT_COPY(proxy_auth_user);
      SP_STR_OPT_COPY(proxy_auth_pass);
      SP_INT_OPT_COPY(locale);
      SP_INT_OPT_COPY(hunt_opened_on_site);

      if (SP_STR_OPT_COPY_IF_CHANGED(proxy_name) +
	  SP_STR_OPT_COPY_IF_CHANGED(backend_url)) {
	myprintf(_("You changed the site/proxy, gethostbyname soon in progress\n"));
      }
      
      SP_INT_OPT_COPY(proxy_port);
      SP_STR_OPT_COPY(user_name);
      SP_STR_OPT_COPY(post_url);
      SP_STR_OPT_COPY(post_template);
      SP_STR_OPT_COPY(user_cookie);
      SP_STR_OPT_COPY(user_login);
      //SP_INT_OPT_COPY(board_auto_refresh);

      if (SP_INT_OPT_COPY_IF_CHANGED(palmi_msg_max_len) +
	  SP_INT_OPT_COPY_IF_CHANGED(palmi_ua_max_len)) {
	rebuild_palmi = 1;
      }

      if (SP_INT_OPT_COPY_IF_CHANGED(mark_id_gaps) +
	  SP_BIC_OPT_COPY_IF_CHANGED(pp_fgcolor) +
	  SP_INT_OPT_COPY_IF_CHANGED(pp_bgcolor) +
	  SP_BIC_OPT_COPY_IF_CHANGED(pp_tstamp_color) +
	  SP_BIC_OPT_COPY_IF_CHANGED(pp_useragent_color) +
	  SP_BIC_OPT_COPY_IF_CHANGED(pp_trollscore_color) +
	  SP_BIC_OPT_COPY_IF_CHANGED(pp_login_color) +
	  SP_BIC_OPT_COPY_IF_CHANGED(pp_url_color) +
	  SP_BIC_OPT_COPY_IF_CHANGED(pp_visited_url_color) +
	  SP_BIC_OPT_COPY_IF_CHANGED(pp_strike_color) +
	  SP_FONTSTYLE_COPY_IF_CHANGED(pp_login_style) +
	  SP_FONTSTYLE_COPY_IF_CHANGED(pp_ua_style) +
	  SP_FONTSTYLE_COPY_IF_CHANGED(pp_clock_style)) {
	redraw_pinni = 1;
      }

      if (SP_INT_OPT_COPY_IF_CHANGED(check_board)) {
	Site *site;
	rebuild_pinni = 1;

	site = sl_find_site_by_name(dock->sites, p->site_name);
	assert(site);

	if (site->prefs->check_board == 0 && site->board) {
	  board_destroy(site->board); site->board = NULL;
	} else if (site->prefs->check_board && site->board == NULL) {
	  site->board = board_create(site, dock->sites->boards);
	}
      }
    }

    /* suppression de sites ? */
    for (i=0; i < MAX_SITES; i++) {
      if (Prefs.site[i] && 
	  wmcc_prefs_find_site(&newPrefs, Prefs.site[i]->site_name) == NULL) {
	Site *site;
	
	site = sl_find_site_by_name(dock->sites, Prefs.site[i]->site_name); assert(site);

	printf(_("site removed: '%s'\n"), site->prefs->site_name);

	sl_delete_site(dock->sites, site);
	wmcc_site_prefs_destroy(Prefs.site[i]);
	Prefs.site[i] = NULL;
	Prefs.nb_sites--;

	rebuild_pinni = 1;
	rebuild_palmi = 1;
      }
    }


    /* création de nouveaux sites ? */
    for (i=0; i < MAX_SITES; i++) {
      if (newPrefs.site[i] &&
	  wmcc_prefs_find_site(&Prefs, newPrefs.site[i]->site_name) == NULL) {
	SitePrefs *sp;
	int j;

	for (j=0; j < MAX_SITES; j++) {
	  if (Prefs.site[j] == NULL) break;
	}
	assert(j < MAX_SITES);
	assert(Prefs.nb_sites< MAX_SITES-1);

	ALLOC_OBJ(sp, SitePrefs);
	Prefs.site[j] = sp; Prefs.nb_sites++;
	wmcc_site_prefs_copy(sp, newPrefs.site[i]);

	printf(_("new site added: '%s'\n"), sp->site_name);

	assert(sl_find_site_by_name(dock->sites, sp->site_name) == NULL);
	sl_insert_new_site(dock->sites, sp);

	rebuild_pinni = 1;
      }
    }

    /* remise dans l'ordre de dock->sites->list */
    {
      Site * slist[MAX_SITES], *s;
      int i;
      memset(slist, 0, sizeof slist);
      for (s = dock->sites->list; s; s = s->next) {
        int nid = wmcc_prefs_find_site_id(&newPrefs, s->prefs->site_name);
        assert(nid != -1);
        slist[nid] = s;
      }
      dock->sites->list = NULL;
      for (i=MAX_SITES-1; i >= 0; --i) {
        if (slist[i]) { slist[i]->next = dock->sites->list; dock->sites->list = slist[i]; }
      }
    }


    /* dans tous les cas, on reevalue la liste triée
       des messages , y'a des fois ou c'est pas necessaire --> a ameliorer */
    boards_init_sites(dock->sites);

    
    /* et allez ! pour les regles de miniua, on swap tout connement (comme ça les anciennes seront
       proprement détruites */
    {
      MiniUARule *tmp = Prefs.miniuarules.first;
      Prefs.miniuarules.first = newPrefs.miniuarules.first;
      newPrefs.miniuarules.first = tmp;
    }
    /* et clac ! comme c'est gruik et j'aime ça, on fait pareil pour les URLReplacement */
    {
      URLReplacement *tmp = Prefs.url_repl.first;
      Prefs.url_repl.first = newPrefs.url_repl.first;
      newPrefs.url_repl.first = tmp;
    }


    /* MISE A JOUR DU COINCOIN */
    if (reset_dock_pix) { 
      if ((errmsg=dock_build_pixmap_porte(dock))) {
	fprintf(stderr, "%s", errmsg);
      }
      close_palmi = 1;
    }
    
    if (close_palmi) {
      int showed;
      if ((showed = editw_ismapped(dock->editw))) editw_unmap(dock, dock->editw);
      editw_reload_colors(dock, dock->editw);
      if (showed) editw_show(dock, NULL, 0);
    }

    if (rebuild_pinni) {
      // int showed = pp_ismapped(dock);
      //      pp_destroy(dock);
      pp_rebuild(dock,1);
      //      if (showed) pp_show(dock);
    } if (redraw_pinni) {
      pp_rebuild(dock,0);
      //pp_set_prefs_colors(dock);
      /*
      if (pp_ismapped(dock)) {
	pp_unmap(dock);
	pp_show(dock);
      }
      */
    }

    if (rebuild_palmi) {
      editw_rebuild(dock);
    }
  }
  

  wmcc_prefs_destroy(&newPrefs);
  free(options_full_file_name);
}
