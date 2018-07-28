# vim:noexpandtab:softtabstop=2:tabstop=2:shiftwidth=2:nowrap
#
# License CC0 PUBLIC DOMAIN
#
# To the extent possible under law, Mark J. Olesen has waived all copyright
# and related or neighboring rights to tabitha. This work is published from:
# United States.

DLL=/mingw64/bin
TOP=../..
BIN=bin
LIB=lib
SHARE=share

PIXBUF_TOP=/mingw64/lib/gdk-pixbuf-2.0/2.10.0/
PIXBUF=${LIB}/gdk-pixbuf-2.0/2.10.0

GLADE=\
	${BIN}/tabitha.glade\
	${BIN}/readme.glade

DEPDLL=\
	${BIN}/libatk-1.0-0.dll\
	${BIN}/libbz2-1.dll\
	${BIN}/libcairo-2.dll\
	${BIN}/libcairo-gobject-2.dll\
	${BIN}/libepoxy-0.dll\
	${BIN}/libexpat-1.dll\
	${BIN}/libffi-6.dll\
	${BIN}/libfontconfig-1.dll\
	${BIN}/libfreetype-6.dll\
	${BIN}/libfribidi-0.dll\
	${BIN}/libgcc_s_seh-1.dll\
	${BIN}/libgdk-3-0.dll\
	${BIN}/libgdk_pixbuf-2.0-0.dll\
	${BIN}/libgio-2.0-0.dll\
	${BIN}/libglib-2.0-0.dll\
	${BIN}/libgmodule-2.0-0.dll\
	${BIN}/libgobject-2.0-0.dll\
	${BIN}/libgraphite2.dll\
	${BIN}/libgtk-3-0.dll\
	${BIN}/libharfbuzz-0.dll\
	${BIN}/libiconv-2.dll\
	${BIN}/libintl-8.dll\
	${BIN}/liblzma-5.dll\
	${BIN}/libpango-1.0-0.dll\
	${BIN}/libpangocairo-1.0-0.dll\
	${BIN}/libpangoft2-1.0-0.dll\
	${BIN}/libpangowin32-1.0-0.dll\
	${BIN}/libpcre-1.dll\
	${BIN}/libpixman-1-0.dll\
	${BIN}/libpng16-16.dll\
	${BIN}/libpq.dll\
	${BIN}/libstdc++-6.dll\
	${BIN}/libwinpthread-1.dll\
	${BIN}/libxml2-2.dll\
	${BIN}/zlib1.dll

.PHONY: all
all : createdirs dll dllpixbuf share bin 

.PHONY: createdirs
createdirs :
	@test -d ${BIN} || mkdir -p ${BIN}
	@test -d ${LIB} || mkdir -p ${LIB}
	@test -d ${SHARE}/icons || mkdir -p ${SHARE}/icons
	@test -d ${PIXBUF}/loaders || mkdir -p ${PIXBUF}/loaders

.PHONY: share
share :
	@rsync -r /mingw64/share/icons ${SHARE}

.PHONY: bin
bin : ${BIN}/tabitha.exe ${GLADE}

${BIN}/tabitha.exe : ${TOP}/bin/tabitha.exe
	strip $?
	install -s -m 0655 $? $@

${BIN}/tabitha.glade : ${TOP}/glade/tabitha.glade
	install -m 0644 $? $@

${BIN}/readme.glade : ${TOP}/glade/readme.glade
	install -m 0644 $? $@

.PHONY: dllpixbuf
dllpixbuf :
	@rsync -r ${PIXBUF_TOP}/loaders.cache ${PIXBUF}/
	@rsync -r ${PIXBUF_TOP}/loaders/*.dll ${PIXBUF}/loaders/

.PHONY: dll
dll : ${DEPDLL}

${BIN}/libatk-1.0-0.dll : ${DLL}/libatk-1.0-0.dll
	install -s -m 0655 $? $@

${BIN}/libbz2-1.dll : ${DLL}/libbz2-1.dll
	install -s -m 0655 $? $@

${BIN}/libcairo-2.dll : ${DLL}/libcairo-2.dll 
	install -s -m 0655 $? $@

${BIN}/libcairo-gobject-2.dll: ${DLL}/libcairo-gobject-2.dll
	install -s -m 0655 $? $@

${BIN}/libexpat-1.dll : ${DLL}/libexpat-1.dll
	install -s -m 0655 $? $@

${BIN}/libgcc_s_seh-1.dll : ${DLL}/libgcc_s_seh-1.dll
	install -s -m 0655 $? $@

${BIN}/libgdk_pixbuf-2.0-0.dll : ${DLL}/libgdk_pixbuf-2.0-0.dll
	install -s -m 0655 $? $@

${BIN}/libgdk-3-0.dll : ${DLL}/libgdk-3-0.dll
	install -s -m 0655 $? $@

${BIN}/libgio-2.0-0.dll : ${DLL}/libgio-2.0-0.dll
	install -s -m 0655 $? $@

${BIN}/libglib-2.0-0.dll : ${DLL}/libglib-2.0-0.dll
	install -s -m 0655 $? $@

${BIN}/libgmodule-2.0-0.dll : ${DLL}/libgmodule-2.0-0.dll
	install -s -m 0655 $? $@

${BIN}/libgobject-2.0-0.dll : ${DLL}/libgobject-2.0-0.dll
	install -s -m 0655 $? $@

${BIN}/libgraphite2.dll : ${DLL}/libgraphite2.dll
	install -s -m 0655 $? $@

${BIN}/libgtk-3-0.dll : ${DLL}/libgtk-3-0.dll
	install -s -m 0655 $? $@

${BIN}/libffi-6.dll : ${DLL}/libffi-6.dll
	install -s -m 0655 $? $@

${BIN}/libfontconfig-1.dll : ${DLL}/libfontconfig-1.dll
	install -s -m 0655 $? $@

${BIN}/libfreetype-6.dll : ${DLL}/libfreetype-6.dll
	install -s -m 0655 $? $@

${BIN}/libharfbuzz-0.dll : ${DLL}/libharfbuzz-0.dll
	install -s -m 0655 $? $@

${BIN}/libfribidi-0.dll : ${DLL}/libfribidi-0.dll
	install -s -m 0655 $? $@

${BIN}/libiconv-2.dll : ${DLL}/libiconv-2.dll
	install -s -m 0655 $? $@

${BIN}/libintl-8.dll : ${DLL}/libintl-8.dll
	install -s -m 0655 $? $@

${BIN}/libpango-1.0-0.dll : ${DLL}/libpango-1.0-0.dll
	install -s -m 0655 $? $@

${BIN}/libpangocairo-1.0-0.dll : ${DLL}/libpangocairo-1.0-0.dll
	install -s -m 0655 $? $@

${BIN}/libpangoft2-1.0-0.dll : ${DLL}/libpangoft2-1.0-0.dll
	install -s -m 0655 $? $@

${BIN}/libpangowin32-1.0-0.dll : ${DLL}/libpangowin32-1.0-0.dll
	install -s -m 0655 $? $@

${BIN}/libpcre-1.dll : ${DLL}/libpcre-1.dll
	install -s -m 0655 $? $@

${BIN}/libpixman-1-0.dll : ${DLL}/libpixman-1-0.dll
	install -s -m 0655 $? $@

${BIN}/libpng16-16.dll : ${DLL}/libpng16-16.dll
	install -s -m 0655 $? $@

${BIN}/libepoxy-0.dll : ${DLL}/libepoxy-0.dll
	install -s -m 0655 $? $@

${BIN}/libpq.dll : ${DLL}/libpq.dll
	install -s -m 0655 $? $@

${BIN}/libwinpthread-1.dll : ${DLL}/libwinpthread-1.dll
	install -s -m 0655 $? $@

${BIN}/libstdc++-6.dll : ${DLL}/libstdc++-6.dll
	install -s -m 0655 $? $@

${BIN}/libxml2-2.dll : ${DLL}/libxml2-2.dll
	install -s -m 0655 $? $@

${BIN}/liblzma-5.dll : ${DLL}/liblzma-5.dll
	install -s -m 0655 $? $@

${BIN}/zlib1.dll : ${DLL}/zlib1.dll
	install -s -m 0655 $? $@
