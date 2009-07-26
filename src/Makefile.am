INCLUDES =								\
	-I${top_srcdir}							\
	-DGETTEXT_PACKAGE=\""$(GETTEXT_PACKAGE)"\"			\
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\"				\
	-DLOCALEDIR=\""$(localedir)"\"					\
	-DDATADIR=\""$(datadir)"\"					\
	-DSRCDIR=\""$(top_srcdir)"\"					\
	-DPLUGINDIR=\""$(plugindir)"\"

bin_PROGRAMS = xfmpc

xfmpc_VALASOURCES =							\
	main-window.vala						\
	statusbar.vala							\
	interface.vala							\
	extended-interface.vala						\
	paned-interface.vala						\
	playlist.vala							\
	dbbrowser.vala							\
	song-dialog.vala						\
	preferences-dialog.vala						\
	preferences.vala						\
	main.vala							\
	plugin-loader.vala						\
	plugin-interfaces.vala

vala_built_SOURCES = $(xfmpc_VALASOURCES:.vala=.c) xfmpc.h

if MAINTAINER_MODE

BUILT_SOURCES =	vala.stamp
vala.stamp: $(xfmpc_VALASOURCES)
	$(VALAC) --vapidir=. --pkg=gmodule-2.0 --pkg=libxfcegui4-1.0 --pkg=config --pkg=mpdclient --pkg=xfce-arrow-button $^ -C -H xfmpc.h
	touch $@

endif

xfmpc_SOURCES = 							\
	mpdclient.c							\
	mpdclient.h							\
	xfce-arrow-button.c						\
	xfce-arrow-button.h						\
	$(vala_built_SOURCES)

xfmpc_CFLAGS =								\
	@GTK_CFLAGS@							\
	@GMODULE_CFLAGS@						\
	@LIBXFCEGUI4_CFLAGS@						\
	@LIBXFCE4UTIL_CFLAGS@						\
	@LIBMPD_CFLAGS@							\
	@GTHREAD_CFLAGS@

xfmpc_LDADD =								\
	@GTK_LIBS@							\
	@GMODULE_LIBS@ 							\
	@LIBXFCEGUI4_LIBS@						\
	@LIBXFCE4UTIL_LIBS@						\
	@LIBMPD_LIBS@							\
	@GTHREAD_LIBS@

EXTRA_DIST =								\
	$(xfmpc_VALASOURCES)

CLEANFILES =								\
	$(BUILT_SOURCES)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
