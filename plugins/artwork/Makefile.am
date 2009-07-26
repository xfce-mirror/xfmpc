INCLUDES =								\
	-I${top_srcdir}							\
	-DGETTEXT_PACKAGE=\""$(GETTEXT_PACKAGE)"\"			\
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\"				\
	-DLOCALEDIR=\""$(localedir)"\"					\
	-DDATADIR=\""$(datadir)"\"

plugin_LTLIBRARIES = libartwork.la

plugin_VALASOURCES =							\
	artwork.vala

vala_built_SOURCES = 							\
	$(plugin_VALASOURCES:.vala=.c)

if MAINTAINER_MODE

BUILT_SOURCES =	vala.stamp
vala.stamp: $(plugin_VALASOURCES)
	$(VALAC) --pkg=gtk+-2.0 --pkg=gmodule-2.0 $(top_srcdir)/src/plugin-interfaces.vala $^ -C
	touch $@

endif

libartwork_la_SOURCES = $(vala_built_SOURCES)
libartwork_la_SOURCES += plugin-interfaces.c

libartwork_la_CFLAGS =							\
	@GTK_CFLAGS@							\
	@GMODULE_CFLAGS@

libartwork_la_LIBDADD =							\
	@GTK_LIBS@							\
	@GMODULE_LIBS@

EXTRA_DIST = $(plugin_VALASOURCES)
CLEANFILES = $(BUILT_SOURCES)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
