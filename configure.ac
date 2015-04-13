AC_PREREQ(2.50)

AC_INIT(couriersrs.h)
AM_INIT_AUTOMAKE(couriersrs,0.1.0)
AM_CONFIG_HEADER(config.h)
AC_LANG(C++)
AC_GNU_SOURCE

sinclude(ac-helpers/ac_define_dir.m4)

dnl Check for programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AM_ICONV

AM_GNU_GETTEXT
AM_GNU_GETTEXT_VERSION(0.14.1)
ALL_LINGUAS=""

AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

AC_SUBST([localedir], ['${datadir}/locale'])

dnl use libtool to compile checks (particularly lib checks), mostly so we don't have to worry about how/if the os supports -R
ac_link="${SHELL} ${srcdir}/libtool --mode=link $ac_link"

dnl headers we need
AC_HEADER_STDC

dnl static builds
AC_MSG_CHECKING(if static builds enabled)
AC_ARG_ENABLE(all-static, AC_HELP_STRING([--enable-all-static], [Build static binaries]), all_static=yes, all_static=no)
if test "x-$all_static" = "x-yes" ; then
    LDFLAGS="$LDFLAGS -Wl,-static -static"
fi
AC_MSG_RESULT($all_static)

AC_MSG_CHECKING(if partial static builds enabled)
AC_ARG_ENABLE(partial-static, AC_HELP_STRING([--enable-partial-static], [Build partially static binaries]), partial_static=yes, partial_static=no)
if test "x-$partial_static" = "x-yes" ; then
    LDFLAGS="$LDFLAGS -Wl,-lc,-static -static"
fi
AC_MSG_RESULT($partial_static)

AC_DEFINE_DIR(LOCALSTATEDIR, localstatedir, [base where spool can be found])

dnl check for libsrs2
AC_ARG_WITH(libsrs2, AC_HELP_STRING([--with-libsrs2=DIR],
	    [Where to find libsrs2 (required)]),
	    libsrs2=$withval, libsrs2=yes)
if test "$libsrs2" != "no"; then
    if test "$libsrs2" != "yes"; then
	LDFLAGS="${LDFLAGS} -L$libsrs2/lib -R$libsrs2/lib"
	CPPFLAGS="${CPPFLAGS} -I$libsrs2/include"
    fi
    AC_CHECK_HEADER(srs2.h,
		    AC_CHECK_LIB(srs2, srs_new,
				 [libsrs2=yes LIBS="${LIBS} -lsrs2"], libsrs2=no),
				 libsrs2=no)
fi
if test "$libsrs2" != "yes"; then
    AC_MSG_ERROR([Couldn't find required libsrs2 installation])
fi

dnl check for libpopt
AC_ARG_WITH(libpopt, AC_HELP_STRING([--with-libpopt=DIR],
	    [Where to find libpopt (required)]),
	    libpopt=$withval, libpopt=yes)
if test "$libpopt" != "no"; then
    if test "$libpopt" != "yes"; then
	LDFLAGS="${LDFLAGS} -L$libpopt/lib -R$libpopt/lib"
	CPPFLAGS="${CPPFLAGS} -I$libpopt/include"
    fi
    AC_CHECK_HEADER(popt.h,
		    AC_CHECK_LIB(popt, poptStrerror,
				 [libpopt=yes LIBS="${LIBS} -lpopt"], libpopt=no),
				 libpopt=no)
fi
if test "$libpopt" != "yes"; then
    AC_MSG_ERROR([Couldn't find required libpopt installation])
fi

dnl define where the configuration file is located
AC_DEFINE_DIR(CONFIG_DIR,sysconfdir,[where the configuration file can be found])

dnl Create the makefiles
AC_OUTPUT(Makefile intl/Makefile po/Makefile.in m4/Makefile )
