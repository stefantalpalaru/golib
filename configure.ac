AC_INIT([golib], [0.0.1], [stefantalpalaru@yahoo.com], [golib], [https://github.com/stefantalpalaru/golib])
AM_INIT_AUTOMAKE([foreign subdir-objects])
LT_INIT

CFLAGS+=" -g -std=gnu99 -Wall -Wextra "
dnl we need to avoid stripping because gccgo binaries need their debug info at runtime
dnl so either filter LDFLAGS or warn the user not to have '-s' in them
LDFLAGS=`echo $LDFLAGS | sed 's/-s\>//g'`
AC_PROG_CC
AC_PROG_GO([gccgo])
AC_CHECK_LIB([pthread], [pthread_mutex_init], [], [AC_MSG_ERROR([pthread not found])])
AC_CHECK_LIB([m], [sqrt], [], [AC_MSG_ERROR([libm not found])])
AC_ARG_VAR(HAVE_GO, [go is available])
AC_CHECK_PROG(HAVE_GO, go, true, false)
AM_CONDITIONAL(HAVE_GO,$HAVE_GO)

AC_OUTPUT([Makefile])

