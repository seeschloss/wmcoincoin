#!/bin/sh
convert duck.xcf duck.png
montage -geometry 32x32 -tile 50x1 duck.png.* duck_xpm.xpm
mogrify -transparent \#002aff duck_xpm.xpm
mv duck_xpm.xpm duck.xpm && rm -f duck.png.*