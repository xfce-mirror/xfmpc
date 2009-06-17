bin_PROGRAMS = xfmpc

if MAINTAINER_MODE

xfmpc_VALASOURCES =							\
	preferences-dialog.vala						\
	song-dialog.vala						\
	statusbar.vala							\
	dbbrowser.vala							\
	extended-interface.vala						\
	interface.vala							\
	main-window.vala						\
	playlist.vala							\
	preferences.vala						\
	main.vala

vala_built_SOURCES =							\
	$(xfmpc_VALASOURCES:.vala=.c)

BUILT_SOURCES =	vala.stamp
vala.stamp: $(xfmpc_VALASOURCES)
	$(VALAC) --vapidir=. --pkg=libxfcegui4-1.0 --pkg=config --pkg=mpdclient --pkg=xfce-arrow-button $^ -C -H xfmpc.h
	touch $@

endif

xfmpc_SOURCES = 							\
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
	@LIBMPD_CFLAGS@

xfmpc_LDADD =								\
	@GTK_LIBS@							\
	@LIBXFCEGUI4_LIBS@						\
	@LIBXFCE4UTIL_LIBS@						\
	@LIBMPD_LIBS@

INCLUDES =								\
	-I${top_srcdir}							\
	-DGETTEXT_PACKAGE=\""$(GETTEXT_PACKAGE)"\"			\
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\"				\
	-DLOCALEDIR=\""$(localedir)"\"					\
	-DDATADIR=\""$(datadir)"\"					\
	-DSRCDIR=\""$(top_srcdir)"\"

EXTRA_DIST =								\
	$(xfmpc_VALASOURCES)

CLEANFILES =								\
	$(BUILT_SOURCES)						\
	$(vala_built_SOURCES)						\
	xfmpc.h

