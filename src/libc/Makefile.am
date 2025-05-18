noinst_LTLIBRARIES = libcxfmpc.la

libcxfmpc_la_SOURCES = \
	mpdclient.c \
	mpdclient.h

libcxfmpc_la_CFLAGS = \
	-I$(top_srcdir) \
	-DGETTEXT_PACKAGE=\""$(GETTEXT_PACKAGE)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	$(GTHREAD_CFLAGS) \
	$(GTK_CFLAGS) \
	$(LIBXFCE4UI_CFLAGS) \
	$(LIBXFCE4UTIL_CFLAGS) \
	$(LIBMPD_CFLAGS)

libcxfmpc_la_LIBADD = \
	$(GTHREAD_LIBS) \
	$(GTK_LIBS) \
	$(LIBXFCE4UI_LIBS) \
	$(LIBXFCE4UTIL_LIBS) \
	$(LIBMPD_LIBS)

EXTRA_DIST = \
	mpdclient.h \
	mpdclient.vapi \
	meson.build
