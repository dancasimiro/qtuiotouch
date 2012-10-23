#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([qtuiotouch],
        m4_esyscmd([build-aux/git-version-gen .tarball-version]),
        [daniel.casimiro@navy.mil])
AC_CONFIG_SRCDIR([src/qtuiotouch.cpp])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADER([build-aux/config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

dnl Automake 1.12 issues a warning about missing AM_PROG_AR
dnl
dnl The following line adds AM_PROG_AR for new versions of automake, while
dnl allowing its use with older versions. The following paragraphs were
dnl copied from the automake mailing list. It offers a little background
dnl on the warning. Maybe libtool will address the issue, but this line
dnl will still be needed for developer boxes with automake-1.12 and an
dnl older version of libtool.
dnl
dnl From <Nick Bowler, Elliptic Technologies (http://www.elliptictech.com/)>
dnl
dnl That being said, the new warning is enabled by the -Wextra-portability
dnl option that was added to Automake-1.12.  I'm by no means an expert on
dnl this issue, but I think it relates to new support for building with the
dnl Microsoft compiler: in other words, I think that ignoring this warning
dnl will not cause regressions in your package but, instead, your package
dnl will simply not be able to make use of the new feature.
dnl 
dnl Regardless, I believe that for most packages the new warning is actually
dnl caused by Libtool, and therefore the correct place to add the call to
dnl AM_PROG_AR is in libtool itself, rather than ad-hoc patching of each
dnl package individually.  You can silence the warnings (but not fix them)
dnl by adding -Wno-extra-portability to AM_INIT_AUTOMAKE.
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

dnl Automake 1.11 added a feature to support silent builds
dnl
dnl This line uses that feature if it is available. The build system output
dnl will resemble the output from the Linux kernel build system. You can restore
dnl the verbose output at configure time by passing the "--disable-silent-rules"
dnl option or at Make time by specifying an envirenment variable V (make V=0).
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O

# Initializes libtool
LT_PREREQ([2.2])
LT_INIT

# Checks for libraries.
# Qt with AutoTroll
AT_WITH_QT([gui-private], [plugin])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
