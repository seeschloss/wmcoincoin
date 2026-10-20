#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "coincoin.h" 

const char *totoz_bookmark_filename() {
  static char *fname = NULL;
  if (fname == NULL) {
    fname = str_printf("%s/.wmcoincoin/totoz/bookmarks.txt", getenv("HOME"));
  }
  return fname;
}
const char *totoz_bookmark_url_html() {
  static char *fname = NULL;
  if (fname == NULL) {
    fname = str_printf("file://%s/.wmcoincoin/totoz/bookmarks.html", getenv("HOME"));
  }
  return fname;
}

static const char *totoz_bookmark_filename_html() {
  static char *fname = NULL;
  if (fname == NULL) {
    fname = str_printf("%s/.wmcoincoin/totoz/bookmarks.html", getenv("HOME"));
  }
  return fname;
}

TotozBookmarkItem* totoz_bookmark_search(Dock *dock, const char *name) {
  TotozBookmark *bm = dock->totoz_bm;
  TotozBookmarkItem *bi = bm->first;
  while (bi) {
    if (strcmp(name, bi->name) == 0) break;
    bi = bi->next;
  }
  return bi;
}

TotozBookmarkItem*
totoz_bookmark_insert(Dock *dock, const char *name) {
  TotozBookmark *bm = dock->totoz_bm;
  TotozBookmarkItem *bi = totoz_bookmark_search(dock, name);
  if (bi) {
    bi->popularity++;
  } else {
    ALLOC_OBJ(bi, TotozBookmarkItem);
    bi->name = strdup(name); bi->popularity = 0;
    bi->next = bm->first; bm->first = bi;
  }
  return bi;
}

static void
totoz_bookmark_destroy(Dock *dock) {
  TotozBookmark *bm = dock->totoz_bm;
  if (bm) {
    TotozBookmarkItem *bi, *bin;
    for (bi = bm->first; bi; bi = bin) {
      bin = bi->next; if (bi->name) free(bi->name);
      free(bi);
    }
    free(bm);
    dock->totoz_bm=NULL;
  }
}

static void 
totoz_bookmark_append_file(Dock *dock, const char *fname) {
  FILE *f = fopen(fname,"r");
  char line[512];
  if (!f) return;
  while (fgets(line, sizeof(line), f)) {
    char *p;
    int pop;
    TotozBookmarkItem *bi;
    str_trim(line);
    if (line[0] == '#' || strlen(line) == 0) continue;
   
    pop = 0;
    p = strrchr(line, ']'); 
    if (p == NULL || p == line
        || strncmp(line,"[:",2) || p - line < 3) {
      printf("wrong totoz in '%s' here: '%s'\n", fname, line); continue;
    }
    if (p[1]) { sscanf(p+1, "%d",&pop); p[1] = 0; }
    bi = totoz_bookmark_insert(dock, line);
    bi->popularity = MAX(bi->popularity, pop);
  }
  fclose(f);
}

void
totoz_bookmark_load(Dock *dock) {
  struct stat st;
  stat(totoz_bookmark_filename(), &st);
  
  totoz_bookmark_destroy(dock);
  ALLOC_OBJ(dock->totoz_bm, TotozBookmark);
  dock->totoz_bm->first = NULL; 
  dock->totoz_bm->last_modif = st.st_mtime;
  totoz_bookmark_append_file(dock, totoz_bookmark_filename());
}

void totoz_bookmark_save(Dock *dock, int merge_first) {
  TotozBookmark *bm = dock->totoz_bm;
  TotozBookmarkItem *bi;
  FILE *f;
  struct stat st;
  stat(totoz_bookmark_filename(), &st);

  if (st.st_mtime != bm->last_modif && merge_first) {
    /* au cas ou le fichier a été edité par ailleurs, on merge
       aux bookmarks */
    totoz_bookmark_append_file(dock, totoz_bookmark_filename()); 
  }
  f = fopen(totoz_bookmark_filename(),"w");
  if (!f) { 
    fprintf(stderr, "could not write to %s : %s\n", totoz_bookmark_filename(), strerror(errno)); 
    return;
  }
  fprintf(f, "# picture bookmark for wmcoincoin\n");
  fprintf(f, "# you can edit manually this file, or use the pinnipede to add new entries\n");
  for (bi = bm->first; bi; bi = bi->next) {
    fprintf(f, "%s %d\n", bi->name, bi->popularity);
  }
  fclose(f);
}

void totoz_bookmark_save_html(Dock *dock) {
  TotozBookmark *bm = dock->totoz_bm;
  TotozBookmarkItem *bi;
  FILE *f;
  f = fopen(totoz_bookmark_filename_html(),"w");
  if (!f) return;
  fprintf(f, 
          "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">"
          "<html>\n"
          "<head>\n"
          " <title>Totoz Bookmarks</title>"
          " <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-15\">"
          " <style type=\"text/css\" title=\"Page default\">\n"
          "  body {background-color:#eee;color: black; font-size: .6em; }\n"
          "  .thumbs {}\n"
          "  .thumbs li {list-style:none; float: left; margin: 1em; border:1px solid #aaa; background-color: white;width:13%%;min-height:3em;text-align:center;padding-top:0.25em; padding-bottom:0.25em;}\n"
          "  .thumbs li a {text-decoration:none;}\n"
          "  .thumbs li a:hover {text-decoration:underline;color:red}\n"
          "  .thumbs li a img {border: none;padding:0.1em;padding-right:0.2em;}\n"
          " </style>\n"
          "</head>\n"
          "<body>\n");
  fprintf(f, "<h2>Source file for these bookmarks: <a href=\"file://%s\">~/.wmcoincoin/totoz/bookmarks.txt</a></h2>\n",
          totoz_bookmark_filename());
  fprintf(f, "<div class=\"thumbs\">\n <ul>\n");
  
  for (bi = bm->first; bi; bi = bi->next) {
    assert(strlen(bi->name)>3); /* ben oui, [:dkskj] */
    char *realfname = pp_totoz_realfname(bi->name,0); assert(realfname);
    char *browserfname = str_substitute(realfname, "%", "%25"); /* ah ben oui sinon le browser il interprete les % ... */
    char *teufname = str_ndup(bi->name+2, strlen(bi->name)-3);
    fprintf(f, "  <li><a href=\"http://totoz.eu/%s.gif\"><img src=\"%s.gif\" alt=\"%s\"></a>%s</li>\n", 
            teufname, browserfname, bi->name, bi->name);
    free(teufname); free(browserfname); free(realfname);
  }
  fprintf(f, " </ul>\n</div>\n");
  fprintf(f, "</body>\n</html>");
  fclose(f);
}


