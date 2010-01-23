bin_PROGRAMS = xfmpc

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

xfmpc_VALAFLAGS =							\
	--vapidir=.							\
	--pkg=libxfcegui4-1.0						\
	--pkg=config							\
	--pkg=mpdclient							\
	--pkg=xfce-arrow-button

vala_built_SOURCES =							\
	$(xfmpc_VALASOURCES:.vala=.c) xfmpc.h

if MAINTAINER_MODE

BUILT_SOURCES =	vala.stamp
vala.stamp: $(xfmpc_VALASOURCES)
	$(VALAC) $(xfmpc_VALAFLAGS) $^ -C -h xfmpc.h
	touch $@

endif

xfmpc_SOURCES = 							\
	mpdclient.c							\
	mpdclient.h							\
	xfce-arrow-button.c						\
	xfce-arrow-button.h
xfmpc_SOURCES += $(vala_built_SOURCES)

xfmpc_CFLAGS =								\
	@GTK_CFLAGS@							\
	@LIBXFCEGUI4_CFLAGS@						\
	@LIBXFCE4UTIL_CFLAGS@						\
	@LIBMPD_CFLAGS@							\
	@GTHREAD_CFLAGS@

xfmpc_LDADD =								\
	@GTK_LIBS@							\
	@LIBXFCEGUI4_LIBS@						\
	@LIBXFCE4UTIL_LIBS@						\
	@LIBMPD_LIBS@							\
	@GTHREAD_LIBS@

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
	$(BUILT_SOURCES)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
