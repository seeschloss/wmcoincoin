#!/bin/sh
IN=${1:-options}
OUT=${2:-src/options_list.h}
echo '/* ce fichier est genere autmatiquement à partir du fichier "options"' > "$OUT"
echo '   par le script "genoptlst.sh" NE PAS EDITER !!! */' >> "$OUT"
printf '%b\n' '#ifndef OPTIONS_LIST_H\n' >> "$OUT"
printf '%b\n' '#define OPTIONS_LIST_H\n\n' >> "$OUT"
echo 'typedef enum {' >> "$OUT"

#options basiques
sed -e 's/#[^a-zA-Z].*//' "$IN" | sed -e 's/#%.*//'| grep ':' | sed -e 's/:.*//' | sed -e 's/\./_/g' | sed -e 's/#*/OPT_/' -e 's/^OPT_OBSOLETE#/OBSOLETE_OPT_/'  | sed -e 's/$/,/'  | uniq >> "$OUT"

#options dependant du site
grep '#\.[a-z]' "$IN" | sed -e 's/#%.*//' | grep ':' | sed -e 's/:.*//' | sed -e 's/\./_/g' | sed -e 's/#_/OPTS_/' | sed -e 's/$/,/' -e 's/#OBSOLETE/OBSOLETE_/' | uniq >> "$OUT"

#options dependant du site ou changement des valeurs par defaut
grep '#\[\.\][a-z]' "$IN" | sed -e 's/#%.*//' | grep ':' | sed -e 's/:.*//' | sed -e 's/\./_/g' | sed -e 's/#\[_\]/OPTSG_/' | sed -e 's/$/,/' -e 's/#OBSOLETE/OBSOLETE_/' | uniq >> "$OUT"

echo 'NB_WMCC_OPTIONS' >> "$OUT"
printf '%b\n' '} wmcc_options_id;\n' >> "$OUT"
printf '%b\n' '\n#if defined(PREFS_C)\n' >> "$OUT"

#generation des chaines de caracteres de noms d'options
echo 'char *wmcc_options_strings[NB_WMCC_OPTIONS+1] = {' >> "$OUT"

#options basiques
sed -e 's/#OBSOLETE//' -e 's/#[^a-z].*//' "$IN" | sed -e 's/#%.*//' | grep ':' | sed -e 's/:.*//' | sed -e 's/^#*/\"/' | sed -e 's/$/\",/' | uniq >> "$OUT"

#options dependantes du site
grep '#\.[a-z]' "$IN" | sed -e 's/#OBSOLETE//' -e 's/#%.*//' | grep ':' | sed -e 's/:.*//' | sed -e 's/#/\"/' | sed -e 's/$/\",/' | uniq >> "$OUT"

#options dependantes du site, mais pouvant etre définies de manière globale
grep '#\[\.\][a-z]' "$IN" | sed -e 's/#OBSOLETE//' -e 's/#%.*//' | grep ':' | sed -e 's/:.*//' | sed -e 's/#\[\.\]/!/' | uniq | awk '{ print "\""$0"\","}' >> "$OUT"


printf '%b\n' 'NULL};\n#else\nextern char *wmcc_options_strings[];\n#endif\n#endif' >> "$OUT"
