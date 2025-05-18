SUBDIRS = \
	libc

AM_CPPFLAGS =								\
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
	progress-bar.vala						\
	main.vala

xfmpc_VALAFLAGS =							\
	--header=xfmpc.h						\
	--vapidir=.				\
	--vapidir=$(top_srcdir)/src/libc				\
	--vapidir=$(top_srcdir)/third-party				\
	--pkg=libxfce4ui-2						\
	--pkg=config							\
	--pkg=mpdclient

xfmpc_CFLAGS =								\
	@GTHREAD_CFLAGS@						\
	@GTK_CFLAGS@							\
	@LIBXFCE4UI_CFLAGS@						\
	@LIBXFCE4UTIL_CFLAGS@						\
	@LIBMPD_CFLAGS@						\
	@VALAC_CFLAGS@

xfmpc_LDADD =								\
	$(top_builddir)/src/libc/libcxfmpc.la							\
	@GTHREAD_LIBS@							\
	@GTK_LIBS@							\
	@LIBXFCE4UI_LIBS@						\
	@LIBXFCE4UTIL_LIBS@						\
	@LIBMPD_LIBS@

BUILT_SOURCES = xfmpc_vala.stamp

EXTRA_DIST =								\
	config.vapi								\
	xfmpc.h									\
	meson.build

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
