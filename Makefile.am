ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign -Wall -Werror

SUBDIRS = src

EXTRA_ORG			= \
				TODO.org \
				OpenHeNB.org \
				cwmp/cwmp.org \
				include/fapi/fapi.org

EXTRA_DIST			=
EXTRA_DIST			+= $(EXTRA_ORG)

cctags: cscope
	@CTAGS@ -e -f TAGS -L cscope.files

cscope:
	@CSCOPE_INDEXER@ -f cscope.out -i cscope.files -r -v

# W/O in fake Makefile.in that build Linux Kernel Modules 
lkm:
	./config.status src/testing/Makefile src/modules/Makefile src/drivers/Makefile
