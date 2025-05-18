ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS =									\
	src										\
	po

dist_man_MANS = xfmpc.1

desktopdir = $(datadir)/applications
desktop_in_files = xfmpc.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

xfmpc.desktop: xfmpc.desktop.in
	$(AM_V_GEN)$(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@

.PHONY: ChangeLog

ChangeLog: Makefile
	(GIT_DIR=$(top_srcdir)/.git git log > .changelog.tmp \
	&& mv .changelog.tmp ChangeLog; rm -f .changelog.tmp) \
	|| (touch ChangeLog; echo 'Git directory not found: installing possibly empty changelog.' >&2)

dist-hook: ChangeLog

EXTRA_DIST =								\
	third-party								\
	IDEAS									\
	meson.build								\
	xfce-revision.h.in						\
	$(desktop_in_files)

MAINTAINERCLEANFILES =						\
	aclocal.m4								\
	compile									\
	config.guess							\
	config.h.in								\
	config.sub								\
	configure								\
	configure.ac							\
	depcomp									\
	install-sh								\
	ltmain.sh								\
	missing									\
	mkinstalldirs

DISTCLEANFILES =							\
	$(desktop_DATA)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
