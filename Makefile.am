ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS =									\
	src										\
	po

dist_man_MANS = xfmpc.1

desktopdir = $(datadir)/applications
desktop_in_files = xfmpc.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

.PHONY: ChangeLog

ChangeLog: Makefile
	(GIT_DIR=$(top_srcdir)/.git git log > .changelog.tmp \
	&& mv .changelog.tmp ChangeLog; rm -f .changelog.tmp) \
	|| (touch ChangeLog; echo 'Git directory not found: installing possibly empty changelog.' >&2)

dist-hook: ChangeLog

EXTRA_DIST =								\
	third-party								\
	IDEAS									\
	README.md                               \
	intltool-extract.in						\
	intltool-merge.in						\
	intltool-update.in						\
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
	mkinstalldirs							\
	intltool-extract						\
	intltool-merge							\
	intltool-update

DISTCLEANFILES =							\
	$(desktop_DATA)							\
	po/.intltool-merge-cache.lock

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
