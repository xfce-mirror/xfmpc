dnl
dnl xfmpc - A graphical GTK+ MPD client focusing on low footprint
dnl
dnl 2008-2009 Mike Massonnet <mmassonnet@xfce.org>
dnl

dnl ***************************
dnl *** Version information ***
dnl ***************************
XDT_VERSION_INIT([0.3.2], [git])
m4_define([copyright_year], [2025])

dnl ***************************
dnl *** Initialize autoconf ***
dnl ***************************
AC_COPYRIGHT([Copyright (c) 2008-copyright_year() The Xfce development team. All rights reserved.])
AC_INIT([Xfmpc], [xdt_version], [https://gitlab.xfce.org/apps/xfmpc], [xfmpc],
        [https://docs.xfce.org/apps/xfmpc/start])
AC_PREREQ([2.69])
AC_CONFIG_MACRO_DIRS([m4])
AC_CANONICAL_TARGET()
AC_REVISION([xdt_version_build])
AC_DEFINE([VERSION_FULL], [PACKAGE_VERSION], [Alias for VERSION and PACKAGE_VERSION for meson compatibility])
COPYRIGHT_YEAR=copyright_year()
AC_DEFINE_UNQUOTED([COPYRIGHT_YEAR], ["$COPYRIGHT_YEAR"], [Copyright year])
AC_SUBST([COPYRIGHT_YEAR])

dnl ***************************
dnl *** Initialize automake ***
dnl ***************************
AM_INIT_AUTOMAKE([1.11 no-dist-gzip dist-bzip2 foreign])
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE()
AM_SILENT_RULES([yes])

dnl ********************************
dnl *** Check for basic programs ***
dnl ********************************
AC_PROG_CC()
AM_PROG_CC_C_O()
LT_PATH_LD()
AC_PROG_INSTALL()

dnl **************************
dnl *** Initialize libtool ***
dnl **************************
LT_PREREQ([2.2.6])
LT_INIT([disable-static])

dnl ***********************************
dnl *** Check for required packages ***
dnl ***********************************
XDT_CHECK_PACKAGE([GTK], [gtk+-3.0], [3.24.0])
XDT_CHECK_PACKAGE([GLIB], [glib-2.0], [2.66.0])
XDT_CHECK_PACKAGE([GTHREAD], [gthread-2.0], [2.66.0])
XDT_CHECK_PACKAGE([LIBXFCE4UI], [libxfce4ui-2], [4.18.0])
XDT_CHECK_PACKAGE([LIBXFCE4UTIL], [libxfce4util-1.0], [4.18.0])
XDT_CHECK_PACKAGE([LIBMPD], [libmpd], [0.15.0])

dnl ***********************
dnl *** Check for valac ***
dnl ***********************
if test "x$USE_MAINTAINER_MODE" = "xyes" ; then
	AM_PROG_VALAC([0.14.0])
	if test "x$VALAC" = "x" ; then
		AC_MSG_ERROR([Cannot find the "valac" compiler in your PATH])
	else
		VALAC_CFLAGS="-w"
		AC_SUBST([VALAC_CFLAGS])
	fi
fi

dnl ******************************
dnl *** Check for i18n support ***
dnl ******************************
GETTEXT_PACKAGE="$PACKAGE"
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Name of default gettext domain])
AC_SUBST([GETTEXT_PACKAGE])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])

dnl ***********************************
dnl *** Check for debugging support ***
dnl ***********************************
XDT_FEATURE_DEBUG([xdt_debug_default])

AC_CONFIG_FILES([
Makefile
src/Makefile
src/libc/Makefile
po/Makefile.in
])
AC_OUTPUT

dnl ***************************
dnl *** Print configuration ***
dnl ***************************
echo
echo "Build Configuration:"
echo
echo "* Maintainer Mode:        $USE_MAINTAINER_MODE"
if test "x$USE_MAINTAINER_MODE" = "xyes" ; then
echo
echo "    * Vala:               $VALAC $am__vala_version"
echo
fi
echo "* Debug Support:          $enable_debug"
echo
