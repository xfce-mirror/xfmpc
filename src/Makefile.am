INCLUDES =								\
	-I${top_srcdir}							\
	-DGETTEXT_PACKAGE=\""$(GETTEXT_PACKAGE)"\"			\
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\"				\
	-DLOCALEDIR=\""$(localedir)"\"					\
	-DDATADIR=\""$(datadir)"\"					\
	-DSRCDIR=\""$(top_srcdir)"\"

bin_PROGRAMS = xfmpc

xfmpc_SOURCES =								\
	shortcuts-dialog.vala						\
	preferences-dialog.vala						\
	song-dialog.vala						\
	statusbar.vala							\
	dbbrowser.vala							\
	extended-interface.vala						\
	interface.vala							\
	main-window.vala						\
	playlist.vala							\
	preferences.vala						\
	main.vala							\
	mpdclient.c							\
	xfce-arrow-button.c

xfmpc_VALAFLAGS =							\
	--header=xfmpc.h						\
	--vapidir=.							\
	--vapidir=$(top_srcdir)/third-party				\
	--pkg=libxfce4ui-1						\
	--pkg=config							\
	--pkg=mpdclient							\
	--pkg=xfce-arrow-button

xfmpc_CFLAGS =								\
	@GTHREAD_CFLAGS@						\
	@LIBX11_CFLAGS@							\
	@GTK_CFLAGS@							\
	@LIBXFCE4UI_CFLAGS@						\
	@LIBXFCE4UTIL_CFLAGS@						\
	@LIBMPD_CFLAGS@

xfmpc_LDADD =								\
	@GTHREAD_LIBS@							\
	@LIBX11_LIBS@							\
	@GTK_LIBS@							\
	@LIBXFCE4UI_LIBS@						\
	@LIBXFCE4UTIL_LIBS@						\
	@LIBMPD_LIBS@

EXTRA_DIST =								\
	mpdclient.h							\
	mpdclient.vapi							\
	xfce-arrow-button.h						\
	xfce-arrow-button.vapi						\
	xfmpc.h

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
