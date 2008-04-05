bin_PROGRAMS = xfmpc

xfmpc_SOURCES =								\
	main.c								\
	main-ui.h							\
	main-ui.xml							\
	interface.c							\
	interface.h							\
	interface-ui.h							\
	interface-ui.xml						\
	extended-interface.c						\
	extended-interface.h						\
	playlist.c							\
	playlist.h							\
	dbbrowser.c							\
	dbbrowser.h							\
	preferences.c							\
	preferences.h							\
	mpdclient.c							\
	mpdclient.h

xfmpc_CFLAGS =								\
	@GTK_CFLAGS@							\
	@LIBXFCEGUI4_CFLAGS@						\
	@LIBXFCE4UTIL_CFLAGS@						\
	@LIBMPD_CFLAGS@							\
	-DDATADIR=\"$(datadir)\"					\
	-DSRCDIR=\"$(top_srcdir)\"

xfmpc_LDADD =								\
	@GTK_LIBS@							\
	@LIBXFCEGUI4_LIBS@						\
	@LIBXFCE4UTIL_LIBS@						\
	@LIBMPD_LIBS@

INCLUDES =								\
	-I${top_srcdir}							\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"				\
	-DLOCALEDIR=\"$(localedir)\"



main-ui.h: Makefile $(srcdir)/main-ui.xml
	exo-csource --strip-comments --strip-content --static --name=main_ui $(srcdir)/main-ui.xml > main-ui.h

interface-ui.h: Makefile $(srcdir)/interface-ui.xml
	exo-csource --strip-comments --strip-content --static --name=xfmpc_interface_ui $(srcdir)/interface-ui.xml > interface-ui.h

