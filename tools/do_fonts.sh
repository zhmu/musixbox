#!/bin/sh
FONTFILE=/usr/share/fonts/truetype/ttf-dejavu/DejaVuSansMono.ttf
./convert_font $FONTFILE 12 > ../lib/ui/font.cc
./convert_font $FONTFILE 8 >> ../lib/ui/font.cc
