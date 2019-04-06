#!/bin/sh

FONTCONVERT=fontconverter/fontconvert

${FONTCONVERT} -s 8 -b 1 fonts/tiny5x5.ttf tiny5x5
${FONTCONVERT} -s 8 -b 1 -f 16  fonts/ati8x8.ttf ati8x8
