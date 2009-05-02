bin_PROGRAMS = xfmpc

xfmpc_VALASOURCES =							\
	preferences-dialog.vala						\
	song-dialog.vala						\
	statusbar.vala							\
	dbbrowser.vala							\
	extended-interface.vala						\
	interface.vala							\
	main-window.vala						\
	playlist.vala

vala_built_SOURCES =							\
	$(xfmpc_VALASOURCES:.vala=.c)

if MAINTAINER_MODE

BUILT_SOURCES =								\
	main-ui.h							\
	vala.stamp

vala.stamp: $(xfmpc_VALASOURCES)
	$(VALAC) --vapidir=$(top_srcdir)/vapi --pkg=gtk+-2.0 --pkg=libxfce4util-1.0 --pkg=libxfcegui4-1.0 --pkg=config --pkg=mpdclient --pkg=preferences --pkg=xfce-arrow-button $^ -C -H xfmpc.h
	touch $@

main-ui.h: Makefile $(srcdir)/main-ui.xml
	exo-csource --strip-comments --strip-content --static --name=main_ui $(srcdir)/main-ui.xml > main-ui.h

endif

xfmpc_SOURCES = 							\
	main.c								\
	main-ui.h							\
	preferences.c							\
	preferences.h							\
	mpdclient.c							\
	mpdclient.h							\
	xfce-arrow-button.c						\
	xfce-arrow-button.h						\
	xfmpc.h
xfmpc_SOURCES += $(vala_built_SOURCES)

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

EXTRA_DIST =								\
	main-ui.xml							\
	$(xfmpc_VALASOURCES)

DISTCLEANFILES =							\
	main-ui.h							\
	$(BUILT_SOURCES)						\
	$(vala_built_SOURCES)

