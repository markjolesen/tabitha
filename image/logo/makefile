# makefile
#
# License CC0 PUBLIC DOMAIN
#
# To the extent possible under law, Mark J. Olesen has waived all copyright
# and related or neighboring rights to tabitha. This work is published from:
# United States.

.PHONY: all
all : tabitha-48x48.png tabitha-64x64.png tabitha-128x128.png 

tabitha-48x48.png : tabitha.svg
	gm convert -scale 48x48 $? $@

tabitha-64x64.png : tabitha.svg
	gm convert -scale 64x64 $? $@

tabitha-128x128.png : tabitha.svg
	gm convert -scale 128x128 $? $@
