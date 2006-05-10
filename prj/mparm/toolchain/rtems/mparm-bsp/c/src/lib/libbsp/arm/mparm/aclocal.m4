# generated automatically by aclocal 1.7.2 -*- Autoconf -*-

# Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.

dnl rtems-top.m4,v 1.14.4.4 2004/08/18 07:23:00 ralf Exp

m4_define([RTEMS_API],[4.6])

dnl
dnl RTEMS_TOP($1)
dnl 
dnl $1 .. relative path from this configure.in to the toplevel configure.in
dnl
AC_DEFUN([RTEMS_TOP],
[dnl
AC_REQUIRE([RTEMS_VERSIONING])
AC_CONFIG_AUX_DIR([$1])
AC_CHECK_PROGS(MAKE, gmake make)
AC_BEFORE([$0], [AM_INIT_AUTOMAKE])dnl

AC_PREFIX_DEFAULT([/opt/rtems-][RTEMS_API])

ENDIF=endif
AC_SUBST(ENDIF)

RTEMS_TOPdir="$1";
AC_SUBST(RTEMS_TOPdir)

test -n "$with_target_subdir" || with_target_subdir="."

if test "$with_target_subdir" = "." ; then
# Native
PROJECT_TOPdir=${RTEMS_TOPdir}/'$(top_builddir)'
else
# Cross
dots=`echo $with_target_subdir|\
sed -e 's%^\./%%' -e 's%[[^/]]$%&/%' -e 's%[[^/]]*/%../%g'`
PROJECT_TOPdir=${dots}${RTEMS_TOPdir}/'$(top_builddir)'
fi
AC_SUBST(PROJECT_TOPdir)

PROJECT_ROOT="${RTEMS_TOPdir}/\$(top_builddir)"
AC_SUBST(PROJECT_ROOT)

AC_MSG_CHECKING([for RTEMS Version])
AS_IF([test -r "${srcdir}/${RTEMS_TOPdir}/aclocal/version.m4"],
[],
[AC_MSG_ERROR([Unable to find ${RTEMS_TOPdir}/aclocal/version.m4])])
AC_MSG_RESULT([_RTEMS_VERSION])
])dnl

AC_DEFUN([RTEMS_VERSIONING],
m4_define([_RTEMS_VERSION],[4.6.5]))

# Do all the work for Automake.                            -*- Autoconf -*-

# This macro actually does too much some checks are only needed if
# your package does certain things.  But this isn't really a big deal.

# Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# serial 8

# There are a few dirty hacks below to avoid letting `AC_PROG_CC' be
# written in clear, in which case automake, when reading aclocal.m4,
# will think it sees a *use*, and therefore will trigger all it's
# C support machinery.  Also note that it means that autoscan, seeing
# CC etc. in the Makefile, will ask for an AC_PROG_CC use...


AC_PREREQ([2.54])

# Autoconf 2.50 wants to disallow AM_ names.  We explicitly allow
# the ones we care about.
m4_pattern_allow([^AM_[A-Z]+FLAGS$])dnl

# AM_INIT_AUTOMAKE(PACKAGE, VERSION, [NO-DEFINE])
# AM_INIT_AUTOMAKE([OPTIONS])
# -----------------------------------------------
# The call with PACKAGE and VERSION arguments is the old style
# call (pre autoconf-2.50), which is being phased out.  PACKAGE
# and VERSION should now be passed to AC_INIT and removed from
# the call to AM_INIT_AUTOMAKE.
# We support both call styles for the transition.  After
# the next Automake release, Autoconf can make the AC_INIT
# arguments mandatory, and then we can depend on a new Autoconf
# release and drop the old call support.
AC_DEFUN([AM_INIT_AUTOMAKE],
[AC_REQUIRE([AM_SET_CURRENT_AUTOMAKE_VERSION])dnl
 AC_REQUIRE([AC_PROG_INSTALL])dnl
# test to see if srcdir already configured
if test "`cd $srcdir && pwd`" != "`pwd`" &&
   test -f $srcdir/config.status; then
  AC_MSG_ERROR([source directory already configured; run "make distclean" there first])
fi

# test whether we have cygpath
if test -z "$CYGPATH_W"; then
  if (cygpath --version) >/dev/null 2>/dev/null; then
    CYGPATH_W='cygpath -w'
  else
    CYGPATH_W=echo
  fi
fi
AC_SUBST([CYGPATH_W])

# Define the identity of the package.
dnl Distinguish between old-style and new-style calls.
m4_ifval([$2],
[m4_ifval([$3], [_AM_SET_OPTION([no-define])])dnl
 AC_SUBST([PACKAGE], [$1])dnl
 AC_SUBST([VERSION], [$2])],
[_AM_SET_OPTIONS([$1])dnl
 AC_SUBST([PACKAGE], [AC_PACKAGE_TARNAME])dnl
 AC_SUBST([VERSION], [AC_PACKAGE_VERSION])])dnl

_AM_IF_OPTION([no-define],,
[AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE", [Name of package])
 AC_DEFINE_UNQUOTED(VERSION, "$VERSION", [Version number of package])])dnl

# Some tools Automake needs.
AC_REQUIRE([AM_SANITY_CHECK])dnl
AC_REQUIRE([AC_ARG_PROGRAM])dnl
AM_MISSING_PROG(ACLOCAL, aclocal-${am__api_version})
AM_MISSING_PROG(AUTOCONF, autoconf)
AM_MISSING_PROG(AUTOMAKE, automake-${am__api_version})
AM_MISSING_PROG(AUTOHEADER, autoheader)
AM_MISSING_PROG(MAKEINFO, makeinfo)
AM_MISSING_PROG(AMTAR, tar)
AM_PROG_INSTALL_SH
AM_PROG_INSTALL_STRIP
# We need awk for the "check" target.  The system "awk" is bad on
# some platforms.
AC_REQUIRE([AC_PROG_AWK])dnl
AC_REQUIRE([AC_PROG_MAKE_SET])dnl

_AM_IF_OPTION([no-dependencies],,
[AC_PROVIDE_IFELSE([AC_PROG_CC],
                  [_AM_DEPENDENCIES(CC)],
                  [define([AC_PROG_CC],
                          defn([AC_PROG_CC])[_AM_DEPENDENCIES(CC)])])dnl
AC_PROVIDE_IFELSE([AC_PROG_CXX],
                  [_AM_DEPENDENCIES(CXX)],
                  [define([AC_PROG_CXX],
                          defn([AC_PROG_CXX])[_AM_DEPENDENCIES(CXX)])])dnl
])
])


# When config.status generates a header, we must update the stamp-h file.
# This file resides in the same directory as the config header
# that is generated.  The stamp files are numbered to have different names.

# Autoconf calls _AC_AM_CONFIG_HEADER_HOOK (when defined) in the
# loop where config.status creates the headers, so we can generate
# our stamp files there.
AC_DEFUN([_AC_AM_CONFIG_HEADER_HOOK],
[_am_stamp_count=`expr ${_am_stamp_count-0} + 1`
echo "timestamp for $1" >`AS_DIRNAME([$1])`/stamp-h[]$_am_stamp_count])

# Copyright 2002  Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA

# AM_AUTOMAKE_VERSION(VERSION)
# ----------------------------
# Automake X.Y traces this macro to ensure aclocal.m4 has been
# generated from the m4 files accompanying Automake X.Y.
AC_DEFUN([AM_AUTOMAKE_VERSION],[am__api_version="1.7"])

# AM_SET_CURRENT_AUTOMAKE_VERSION
# -------------------------------
# Call AM_AUTOMAKE_VERSION so it can be traced.
# This function is AC_REQUIREd by AC_INIT_AUTOMAKE.
AC_DEFUN([AM_SET_CURRENT_AUTOMAKE_VERSION],
	 [AM_AUTOMAKE_VERSION([1.7.2])])

# Helper functions for option handling.                    -*- Autoconf -*-

# Copyright 2001, 2002  Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# serial 2

# _AM_MANGLE_OPTION(NAME)
# -----------------------
AC_DEFUN([_AM_MANGLE_OPTION],
[[_AM_OPTION_]m4_bpatsubst($1, [[^a-zA-Z0-9_]], [_])])

# _AM_SET_OPTION(NAME)
# ------------------------------
# Set option NAME.  Presently that only means defining a flag for this option.
AC_DEFUN([_AM_SET_OPTION],
[m4_define(_AM_MANGLE_OPTION([$1]), 1)])

# _AM_SET_OPTIONS(OPTIONS)
# ----------------------------------
# OPTIONS is a space-separated list of Automake options.
AC_DEFUN([_AM_SET_OPTIONS],
[AC_FOREACH([_AM_Option], [$1], [_AM_SET_OPTION(_AM_Option)])])

# _AM_IF_OPTION(OPTION, IF-SET, [IF-NOT-SET])
# -------------------------------------------
# Execute IF-SET if OPTION is set, IF-NOT-SET otherwise.
AC_DEFUN([_AM_IF_OPTION],
[m4_ifset(_AM_MANGLE_OPTION([$1]), [$2], [$3])])

#
# Check to make sure that the build environment is sane.
#

# Copyright 1996, 1997, 2000, 2001 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# serial 3

# AM_SANITY_CHECK
# ---------------
AC_DEFUN([AM_SANITY_CHECK],
[AC_MSG_CHECKING([whether build environment is sane])
# Just in case
sleep 1
echo timestamp > conftest.file
# Do `set' in a subshell so we don't clobber the current shell's
# arguments.  Must try -L first in case configure is actually a
# symlink; some systems play weird games with the mod time of symlinks
# (eg FreeBSD returns the mod time of the symlink's containing
# directory).
if (
   set X `ls -Lt $srcdir/configure conftest.file 2> /dev/null`
   if test "$[*]" = "X"; then
      # -L didn't work.
      set X `ls -t $srcdir/configure conftest.file`
   fi
   rm -f conftest.file
   if test "$[*]" != "X $srcdir/configure conftest.file" \
      && test "$[*]" != "X conftest.file $srcdir/configure"; then

      # If neither matched, then we have a broken ls.  This can happen
      # if, for instance, CONFIG_SHELL is bash and it inherits a
      # broken ls alias from the environment.  This has actually
      # happened.  Such a system could not be considered "sane".
      AC_MSG_ERROR([ls -t appears to fail.  Make sure there is not a broken
alias in your environment])
   fi

   test "$[2]" = conftest.file
   )
then
   # Ok.
   :
else
   AC_MSG_ERROR([newly created file is older than distributed files!
Check your system clock])
fi
AC_MSG_RESULT(yes)])

#  -*- Autoconf -*-


# Copyright 1997, 1999, 2000, 2001 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# serial 3

# AM_MISSING_PROG(NAME, PROGRAM)
# ------------------------------
AC_DEFUN([AM_MISSING_PROG],
[AC_REQUIRE([AM_MISSING_HAS_RUN])
$1=${$1-"${am_missing_run}$2"}
AC_SUBST($1)])


# AM_MISSING_HAS_RUN
# ------------------
# Define MISSING if not defined so far and test if it supports --run.
# If it does, set am_missing_run to use it, otherwise, to nothing.
AC_DEFUN([AM_MISSING_HAS_RUN],
[AC_REQUIRE([AM_AUX_DIR_EXPAND])dnl
test x"${MISSING+set}" = xset || MISSING="\${SHELL} $am_aux_dir/missing"
# Use eval to expand $SHELL
if eval "$MISSING --run true"; then
  am_missing_run="$MISSING --run "
else
  am_missing_run=
  AC_MSG_WARN([`missing' script is too old or missing])
fi
])

# AM_AUX_DIR_EXPAND

# Copyright 2001 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# For projects using AC_CONFIG_AUX_DIR([foo]), Autoconf sets
# $ac_aux_dir to `$srcdir/foo'.  In other projects, it is set to
# `$srcdir', `$srcdir/..', or `$srcdir/../..'.
#
# Of course, Automake must honor this variable whenever it calls a
# tool from the auxiliary directory.  The problem is that $srcdir (and
# therefore $ac_aux_dir as well) can be either absolute or relative,
# depending on how configure is run.  This is pretty annoying, since
# it makes $ac_aux_dir quite unusable in subdirectories: in the top
# source directory, any form will work fine, but in subdirectories a
# relative path needs to be adjusted first.
#
# $ac_aux_dir/missing
#    fails when called from a subdirectory if $ac_aux_dir is relative
# $top_srcdir/$ac_aux_dir/missing
#    fails if $ac_aux_dir is absolute,
#    fails when called from a subdirectory in a VPATH build with
#          a relative $ac_aux_dir
#
# The reason of the latter failure is that $top_srcdir and $ac_aux_dir
# are both prefixed by $srcdir.  In an in-source build this is usually
# harmless because $srcdir is `.', but things will broke when you
# start a VPATH build or use an absolute $srcdir.
#
# So we could use something similar to $top_srcdir/$ac_aux_dir/missing,
# iff we strip the leading $srcdir from $ac_aux_dir.  That would be:
#   am_aux_dir='\$(top_srcdir)/'`expr "$ac_aux_dir" : "$srcdir//*\(.*\)"`
# and then we would define $MISSING as
#   MISSING="\${SHELL} $am_aux_dir/missing"
# This will work as long as MISSING is not called from configure, because
# unfortunately $(top_srcdir) has no meaning in configure.
# However there are other variables, like CC, which are often used in
# configure, and could therefore not use this "fixed" $ac_aux_dir.
#
# Another solution, used here, is to always expand $ac_aux_dir to an
# absolute PATH.  The drawback is that using absolute paths prevent a
# configured tree to be moved without reconfiguration.

# Rely on autoconf to set up CDPATH properly.
AC_PREREQ([2.50])

AC_DEFUN([AM_AUX_DIR_EXPAND], [
# expand $ac_aux_dir to an absolute path
am_aux_dir=`cd $ac_aux_dir && pwd`
])

# AM_PROG_INSTALL_SH
# ------------------
# Define $install_sh.

# Copyright 2001 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

AC_DEFUN([AM_PROG_INSTALL_SH],
[AC_REQUIRE([AM_AUX_DIR_EXPAND])dnl
install_sh=${install_sh-"$am_aux_dir/install-sh"}
AC_SUBST(install_sh)])

# AM_PROG_INSTALL_STRIP

# Copyright 2001 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# One issue with vendor `install' (even GNU) is that you can't
# specify the program used to strip binaries.  This is especially
# annoying in cross-compiling environments, where the build's strip
# is unlikely to handle the host's binaries.
# Fortunately install-sh will honor a STRIPPROG variable, so we
# always use install-sh in `make install-strip', and initialize
# STRIPPROG with the value of the STRIP variable (set by the user).
AC_DEFUN([AM_PROG_INSTALL_STRIP],
[AC_REQUIRE([AM_PROG_INSTALL_SH])dnl
# Installed binaries are usually stripped using `strip' when the user
# run `make install-strip'.  However `strip' might not be the right
# tool to use in cross-compilation environments, therefore Automake
# will honor the `STRIP' environment variable to overrule this program.
dnl Don't test for $cross_compiling = yes, because it might be `maybe'.
if test "$cross_compiling" != no; then
  AC_CHECK_TOOL([STRIP], [strip], :)
fi
INSTALL_STRIP_PROGRAM="\${SHELL} \$(install_sh) -c -s"
AC_SUBST([INSTALL_STRIP_PROGRAM])])

# serial 4						-*- Autoconf -*-

# Copyright 1999, 2000, 2001 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.


# There are a few dirty hacks below to avoid letting `AC_PROG_CC' be
# written in clear, in which case automake, when reading aclocal.m4,
# will think it sees a *use*, and therefore will trigger all it's
# C support machinery.  Also note that it means that autoscan, seeing
# CC etc. in the Makefile, will ask for an AC_PROG_CC use...



# _AM_DEPENDENCIES(NAME)
# ----------------------
# See how the compiler implements dependency checking.
# NAME is "CC", "CXX", "GCJ", or "OBJC".
# We try a few techniques and use that to set a single cache variable.
#
# We don't AC_REQUIRE the corresponding AC_PROG_CC since the latter was
# modified to invoke _AM_DEPENDENCIES(CC); we would have a circular
# dependency, and given that the user is not expected to run this macro,
# just rely on AC_PROG_CC.
AC_DEFUN([_AM_DEPENDENCIES],
[AC_REQUIRE([AM_SET_DEPDIR])dnl
AC_REQUIRE([AM_OUTPUT_DEPENDENCY_COMMANDS])dnl
AC_REQUIRE([AM_MAKE_INCLUDE])dnl
AC_REQUIRE([AM_DEP_TRACK])dnl

ifelse([$1], CC,   [depcc="$CC"   am_compiler_list=],
       [$1], CXX,  [depcc="$CXX"  am_compiler_list=],
       [$1], OBJC, [depcc="$OBJC" am_compiler_list='gcc3 gcc'],
       [$1], GCJ,  [depcc="$GCJ"  am_compiler_list='gcc3 gcc'],
                   [depcc="$$1"   am_compiler_list=])

AC_CACHE_CHECK([dependency style of $depcc],
               [am_cv_$1_dependencies_compiler_type],
[if test -z "$AMDEP_TRUE" && test -f "$am_depcomp"; then
  # We make a subdir and do the tests there.  Otherwise we can end up
  # making bogus files that we don't know about and never remove.  For
  # instance it was reported that on HP-UX the gcc test will end up
  # making a dummy file named `D' -- because `-MD' means `put the output
  # in D'.
  mkdir conftest.dir
  # Copy depcomp to subdir because otherwise we won't find it if we're
  # using a relative directory.
  cp "$am_depcomp" conftest.dir
  cd conftest.dir

  am_cv_$1_dependencies_compiler_type=none
  if test "$am_compiler_list" = ""; then
     am_compiler_list=`sed -n ['s/^#*\([a-zA-Z0-9]*\))$/\1/p'] < ./depcomp`
  fi
  for depmode in $am_compiler_list; do
    # We need to recreate these files for each test, as the compiler may
    # overwrite some of them when testing with obscure command lines.
    # This happens at least with the AIX C compiler.
    echo '#include "conftest.h"' > conftest.c
    echo 'int i;' > conftest.h
    echo "${am__include} ${am__quote}conftest.Po${am__quote}" > confmf

    case $depmode in
    nosideeffect)
      # after this tag, mechanisms are not by side-effect, so they'll
      # only be used when explicitly requested
      if test "x$enable_dependency_tracking" = xyes; then
	continue
      else
	break
      fi
      ;;
    none) break ;;
    esac
    # We check with `-c' and `-o' for the sake of the "dashmstdout"
    # mode.  It turns out that the SunPro C++ compiler does not properly
    # handle `-M -o', and we need to detect this.
    if depmode=$depmode \
       source=conftest.c object=conftest.o \
       depfile=conftest.Po tmpdepfile=conftest.TPo \
       $SHELL ./depcomp $depcc -c -o conftest.o conftest.c >/dev/null 2>&1 &&
       grep conftest.h conftest.Po > /dev/null 2>&1 &&
       ${MAKE-make} -s -f confmf > /dev/null 2>&1; then
      am_cv_$1_dependencies_compiler_type=$depmode
      break
    fi
  done

  cd ..
  rm -rf conftest.dir
else
  am_cv_$1_dependencies_compiler_type=none
fi
])
AC_SUBST([$1DEPMODE], [depmode=$am_cv_$1_dependencies_compiler_type])
AM_CONDITIONAL([am__fastdep$1], [
  test "x$enable_dependency_tracking" != xno \
  && test "$am_cv_$1_dependencies_compiler_type" = gcc3])
])


# AM_SET_DEPDIR
# -------------
# Choose a directory name for dependency files.
# This macro is AC_REQUIREd in _AM_DEPENDENCIES
AC_DEFUN([AM_SET_DEPDIR],
[rm -f .deps 2>/dev/null
mkdir .deps 2>/dev/null
if test -d .deps; then
  DEPDIR=.deps
else
  # MS-DOS does not allow filenames that begin with a dot.
  DEPDIR=_deps
fi
rmdir .deps 2>/dev/null
AC_SUBST([DEPDIR])
])


# AM_DEP_TRACK
# ------------
AC_DEFUN([AM_DEP_TRACK],
[AC_ARG_ENABLE(dependency-tracking,
[  --disable-dependency-tracking Speeds up one-time builds
  --enable-dependency-tracking  Do not reject slow dependency extractors])
if test "x$enable_dependency_tracking" != xno; then
  am_depcomp="$ac_aux_dir/depcomp"
  AMDEPBACKSLASH='\'
fi
AM_CONDITIONAL([AMDEP], [test "x$enable_dependency_tracking" != xno])
AC_SUBST([AMDEPBACKSLASH])
])

# Generate code to set up dependency tracking.   -*- Autoconf -*-

# Copyright 1999, 2000, 2001, 2002 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

#serial 2

# _AM_OUTPUT_DEPENDENCY_COMMANDS
# ------------------------------
AC_DEFUN([_AM_OUTPUT_DEPENDENCY_COMMANDS],
[for mf in $CONFIG_FILES; do
  # Strip MF so we end up with the name of the file.
  mf=`echo "$mf" | sed -e 's/:.*$//'`
  # Check whether this is an Automake generated Makefile or not.
  # We used to match only the files named `Makefile.in', but
  # some people rename them; so instead we look at the file content.
  # Grep'ing the first line is not enough: some people post-process
  # each Makefile.in and add a new line on top of each file to say so.
  # So let's grep whole file.
  if grep '^#.*generated by automake' $mf > /dev/null 2>&1; then
    dirpart=`AS_DIRNAME("$mf")`
  else
    continue
  fi
  grep '^DEP_FILES *= *[[^ @%:@]]' < "$mf" > /dev/null || continue
  # Extract the definition of DEP_FILES from the Makefile without
  # running `make'.
  DEPDIR=`sed -n -e '/^DEPDIR = / s///p' < "$mf"`
  test -z "$DEPDIR" && continue
  # When using ansi2knr, U may be empty or an underscore; expand it
  U=`sed -n -e '/^U = / s///p' < "$mf"`
  test -d "$dirpart/$DEPDIR" || mkdir "$dirpart/$DEPDIR"
  # We invoke sed twice because it is the simplest approach to
  # changing $(DEPDIR) to its actual value in the expansion.
  for file in `sed -n -e '
    /^DEP_FILES = .*\\\\$/ {
      s/^DEP_FILES = //
      :loop
	s/\\\\$//
	p
	n
	/\\\\$/ b loop
      p
    }
    /^DEP_FILES = / s/^DEP_FILES = //p' < "$mf" | \
       sed -e 's/\$(DEPDIR)/'"$DEPDIR"'/g' -e 's/\$U/'"$U"'/g'`; do
    # Make sure the directory exists.
    test -f "$dirpart/$file" && continue
    fdir=`AS_DIRNAME(["$file"])`
    AS_MKDIR_P([$dirpart/$fdir])
    # echo "creating $dirpart/$file"
    echo '# dummy' > "$dirpart/$file"
  done
done
])# _AM_OUTPUT_DEPENDENCY_COMMANDS


# AM_OUTPUT_DEPENDENCY_COMMANDS
# -----------------------------
# This macro should only be invoked once -- use via AC_REQUIRE.
#
# This code is only required when automatic dependency tracking
# is enabled.  FIXME.  This creates each `.P' file that we will
# need in order to bootstrap the dependency handling code.
AC_DEFUN([AM_OUTPUT_DEPENDENCY_COMMANDS],
[AC_CONFIG_COMMANDS([depfiles],
     [test x"$AMDEP_TRUE" != x"" || _AM_OUTPUT_DEPENDENCY_COMMANDS],
     [AMDEP_TRUE="$AMDEP_TRUE" ac_aux_dir="$ac_aux_dir"])
])

# Check to see how 'make' treats includes.	-*- Autoconf -*-

# Copyright (C) 2001, 2002 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# serial 2

# AM_MAKE_INCLUDE()
# -----------------
# Check to see how make treats includes.
AC_DEFUN([AM_MAKE_INCLUDE],
[am_make=${MAKE-make}
cat > confinc << 'END'
doit:
	@echo done
END
# If we don't find an include directive, just comment out the code.
AC_MSG_CHECKING([for style of include used by $am_make])
am__include="#"
am__quote=
_am_result=none
# First try GNU make style include.
echo "include confinc" > confmf
# We grep out `Entering directory' and `Leaving directory'
# messages which can occur if `w' ends up in MAKEFLAGS.
# In particular we don't look at `^make:' because GNU make might
# be invoked under some other name (usually "gmake"), in which
# case it prints its new name instead of `make'.
if test "`$am_make -s -f confmf 2> /dev/null | grep -v 'ing directory'`" = "done"; then
   am__include=include
   am__quote=
   _am_result=GNU
fi
# Now try BSD make style include.
if test "$am__include" = "#"; then
   echo '.include "confinc"' > confmf
   if test "`$am_make -s -f confmf 2> /dev/null`" = "done"; then
      am__include=.include
      am__quote="\""
      _am_result=BSD
   fi
fi
AC_SUBST(am__include)
AC_SUBST(am__quote)
AC_MSG_RESULT($_am_result)
rm -f confinc confmf
])

# AM_CONDITIONAL                                              -*- Autoconf -*-

# Copyright 1997, 2000, 2001 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# serial 5

AC_PREREQ(2.52)

# AM_CONDITIONAL(NAME, SHELL-CONDITION)
# -------------------------------------
# Define a conditional.
AC_DEFUN([AM_CONDITIONAL],
[ifelse([$1], [TRUE],  [AC_FATAL([$0: invalid condition: $1])],
        [$1], [FALSE], [AC_FATAL([$0: invalid condition: $1])])dnl
AC_SUBST([$1_TRUE])
AC_SUBST([$1_FALSE])
if $2; then
  $1_TRUE=
  $1_FALSE='#'
else
  $1_TRUE='#'
  $1_FALSE=
fi
AC_CONFIG_COMMANDS_PRE(
[if test -z "${$1_TRUE}" && test -z "${$1_FALSE}"; then
  AC_MSG_ERROR([conditional "$1" was never defined.
Usually this means the macro was only invoked conditionally.])
fi])])

dnl
dnl canonical-target-name.m4,v 1.12.4.2 2004/08/18 07:23:00 ralf Exp
dnl

dnl canonicalize target cpu
dnl NOTE: Most rtems targets do not fullfil autoconf's
dnl target naming conventions "processor-vendor-os"
dnl Therefore autoconf's AC_CANONICAL_TARGET will fail for them
dnl and we have to fix it for rtems ourselves 

AC_DEFUN([RTEMS_CANONICAL_TARGET_CPU],
[
AC_CANONICAL_TARGET
AC_MSG_CHECKING(rtems target cpu)
case "${target}" in
  # hpux unix port should go here
  i[[34567]]86-*linux*)		# unix "simulator" port
	RTEMS_CPU=unix
	;;
  i[[34567]]86-*freebsd*) 	# unix "simulator" port
	RTEMS_CPU=unix
	;;
  i[[34567]]86-pc-cygwin*) 	# Cygwin is just enough unix like :)
	RTEMS_CPU=unix
	;;
  no_cpu-*rtems*)
        RTEMS_CPU=no_cpu
	;;
  sparc-sun-solaris*)           # unix "simulator" port
	RTEMS_CPU=unix
	;;
  tic4x-*rtems*)          	# gcc changed the name
	RTEMS_CPU=c4x
	;;
  *) 
	RTEMS_CPU=`echo $target | sed 's%^\([[^-]]*\)-\(.*\)$%\1%'`
	;;
esac
AC_SUBST(RTEMS_CPU)
AC_MSG_RESULT($RTEMS_CPU)
])

dnl bsp-configure.m4,v 1.5 2002/12/07 13:06:23 ralf Exp

dnl RTEMS_BSP_CONFIGURE
dnl Common macro to be included by all bsp-configure.acs
AC_DEFUN([RTEMS_BSP_CONFIGURE],
[
  AC_PREREQ([2.52])
  AM_MAINTAINER_MODE
  RTEMS_ENV_RTEMSBSP

  RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

  RTEMS_CANONICAL_HOST
  AM_CONFIG_HEADER([include/bspopts.tmp:include/bspopts.h.in],[
echo "/* BSP dependent options file */"         >$tmp/config.h
echo "/* automatically generated -- DO NOT EDIT!! */" >>$tmp/config.h
echo                                                  >>$tmp/config.h
echo "#ifndef __BSP_OPTIONS_h"                        >>$tmp/config.h
echo "#define __BSP_OPTIONS_h"                        >>$tmp/config.h
echo                                                  >>$tmp/config.h
sed -e '/.*PACKAGE.*/d' include/bspopts.tmp >> $tmp/config.h
echo                                                  >>$tmp/config.h
echo "#endif"                                         >>$tmp/config.h
  AS_IF([cmp -s include/bspopts.h $tmp/config.h 2>/dev/null],[
    AC_MSG_NOTICE([include/bspopts.h is unchanged])
    rm -f $tmp/config.h],[
    AC_MSG_NOTICE([creating include/bspopts.h])
    rm -f include/bspopts.h
    mv $tmp/config.h include/bspopts.h
  ])
])
  RTEMS_PROJECT_ROOT

  RTEMS_CHECK_MULTIPROCESSING
])

# Add --enable-maintainer-mode option to configure.
# From Jim Meyering

# Copyright 1996, 1998, 2000, 2001, 2002  Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

# serial 2

AC_DEFUN([AM_MAINTAINER_MODE],
[AC_MSG_CHECKING([whether to enable maintainer-specific portions of Makefiles])
  dnl maintainer-mode is disabled by default
  AC_ARG_ENABLE(maintainer-mode,
[  --enable-maintainer-mode enable make rules and dependencies not useful
                          (and sometimes confusing) to the casual installer],
      USE_MAINTAINER_MODE=$enableval,
      USE_MAINTAINER_MODE=no)
  AC_MSG_RESULT([$USE_MAINTAINER_MODE])
  AM_CONDITIONAL(MAINTAINER_MODE, [test $USE_MAINTAINER_MODE = yes])
  MAINT=$MAINTAINER_MODE_TRUE
  AC_SUBST(MAINT)dnl
]
)

AU_DEFUN([jm_MAINTAINER_MODE], [AM_MAINTAINER_MODE])

dnl env-rtemsbsp.m4,v 1.10.2.1 2004/08/18 07:23:00 ralf Exp

dnl Pass a single BSP via an environment variable
dnl used by per BSP configure scripts
AC_DEFUN([RTEMS_ENV_RTEMSBSP],
[dnl
AC_BEFORE([$0], [RTEMS_ENABLE_RTEMSBSP])dnl
AC_BEFORE([$0], [RTEMS_PROJECT_ROOT])dnl
AC_BEFORE([$0], [RTEMS_CHECK_CUSTOM_BSP])dnl

AC_ARG_VAR([CPU_CFLAGS],[CFLAGS specifying CPU-dependent features])
AC_ARG_VAR([CFLAGS_OPTIMIZE_V],[CFLAGS for building the OPTIMIZE variant])
AC_ARG_VAR([CFLAGS_DEBUG_V],[CFLAGS for building the DEBUG variant])
AC_ARG_VAR([CFLAGS_PROFILE_V],[CFLAGS for building the PROFILE variant])
AC_ARG_VAR([RTEMS_BSP_FAMILY],[RTEMS's BSP directory])
AC_ARG_VAR([RTEMS_CPU_MODEL],[RTEMS's cpu model])

AC_ARG_VAR([RTEMS_BSP],[RTEMS_BSP to build])
AC_MSG_CHECKING([for RTEMS_BSP])
AC_CACHE_VAL(rtems_cv_RTEMS_BSP,
[dnl
  test -n "${RTEMS_BSP}" && rtems_cv_RTEMS_BSP="$RTEMS_BSP";
])dnl
if test -z "$rtems_cv_RTEMS_BSP"; then
  AC_MSG_ERROR([Missing RTEMS_BSP])
fi
RTEMS_BSP="$rtems_cv_RTEMS_BSP"
AC_MSG_RESULT(${RTEMS_BSP})
AC_SUBST(RTEMS_BSP)

RTEMS_BSP_SPECS="-specs bsp_specs -qrtems"
AC_SUBST(RTEMS_BSP_SPECS)

GCC_SPECS="-B\$(PROJECT_ROOT)/lib/ -B\$(PROJECT_ROOT)/$RTEMS_BSP/lib/"
AC_SUBST(GCC_SPECS)

PROJECT_INCLUDE="\$(PROJECT_ROOT)/$RTEMS_BSP/lib/include"
AC_SUBST(PROJECT_INCLUDE)

PROJECT_RELEASE="\$(PROJECT_ROOT)/$RTEMS_BSP"   
AC_SUBST(PROJECT_RELEASE)

RTEMS_ROOT="\$(PROJECT_ROOT)/c/$RTEMS_BSP"
AC_SUBST(RTEMS_ROOT)

RTEMS_ENABLE_BARE
AC_SUBST(BARE_CPU_MODEL)
AC_SUBST(BARE_CPU_CFLAGS)

AM_CONDITIONAL([MULTILIB],[false])

includedir="\${exec_prefix}/${RTEMS_BSP}/lib/include"
libdir="\${exec_prefix}/${RTEMS_BSP}/lib"

bsplibdir="\${exec_prefix}/${RTEMS_BSP}/lib"
AC_SUBST(bsplibdir)
])

dnl enable-rtemsbsp.m4,v 1.12.2.1 2004/08/18 07:23:00 ralf Exp

dnl Override the set of BSPs to be built.
dnl used by the toplevel configure script
dnl RTEMS_ENABLE_RTEMSBSP(rtems_bsp_list)
AC_DEFUN([RTEMS_ENABLE_RTEMSBSP],
[
AC_BEFORE([$0], [RTEMS_ENV_RTEMSBSP])
AC_ARG_ENABLE(rtemsbsp,
[AC_HELP_STRING([--enable-rtemsbsp="bsp1 bsp2 .."],
[BSPs to include in build])],
[case "${enableval}" in
  yes|no) AC_MSG_ERROR([missing argument to --enable-rtemsbsp="bsp1 bsp2"]);;
  *) $1=$enableval;;
esac],[$1=""])
])

dnl
dnl project-root.m4,v 1.6.4.1 2004/08/18 07:23:00 ralf Exp
dnl 

dnl
dnl PROJECT_TOPdir     .. relative path to the top of the build-tree
dnl PROJECT_ROOT       .. relative path to the top of the temporary
dnl                       installation directory inside the build-tree
dnl RTEMS_TOPdir       .. relative path of a subpackage's configure.in to the
dnl                       toplevel configure.in of the source-tree
dnl RTEMS_ROOT         .. path to the top of a bsp's build directory
dnl                       [Applied by custom/*.cfg, depredicated otherwise]
dnl

AC_DEFUN([RTEMS_PROJECT_ROOT],
[dnl
AC_REQUIRE([RTEMS_TOP])

PACKHEX="\$(PROJECT_TOPdir)/tools/build/packhex"
AC_SUBST(PACKHEX)
])


dnl check-custom-bsp.m4,v 1.1.2.1 2004/08/18 07:23:00 ralf Exp

AC_DEFUN([RTEMS_CHECK_CUSTOM_BSP],[
AC_REQUIRE([RTEMS_TOP])

AC_MSG_CHECKING([for make/custom/[$]$1.cfg])
AS_IF([test -r "$srcdir/$RTEMS_TOPdir/make/custom/[$]$1.cfg"],
  [AC_MSG_RESULT([yes])],
  [AC_MSG_ERROR([no])])
])

AC_DEFUN([RTEMS_ENABLE_BARE],
[
AC_ARG_ENABLE(bare-cpu-cflags,
[AC_HELP_STRING([--enable-bare-cpu-cflags],
[specify a particular cpu cflag (bare bsp specific)])],
[case "${enableval}" in
  no) BARE_CPU_CFLAGS="" ;;
  *)    BARE_CPU_CFLAGS="${enableval}" ;;
esac],
[BARE_CPU_CFLAGS=""])

AC_ARG_ENABLE(bare-cpu-model,
[AC_HELP_STRING([--enable-bare-cpu-model],
[specify a particular cpu model (bare bsp specific)])],
[case "${enableval}" in
  no)   BARE_CPU_MODEL="" ;;
  *)    BARE_CPU_MODEL="${enableval}" ;;
esac],
[BARE_CPU_MODEL=""])
])


dnl canonical-host.m4,v 1.6.6.1 2004/08/18 07:23:00 ralf Exp

AC_DEFUN([RTEMS_CANONICAL_HOST],
[dnl
AC_REQUIRE([AC_CANONICAL_HOST])
RTEMS_HOST=$host_os
case "${target}" in
  # hpux unix port should go here
  i[[34567]]86-*linux*)          # unix "simulator" port
        RTEMS_HOST=Linux
	;;
  i[[34567]]86-*freebsd*)         # unix "simulator" port
        RTEMS_HOST=FreeBSD
	;;
  i[[34567]]86-pc-cygwin*) 	# Cygwin is just enough unix like :)
        RTEMS_HOST=Cygwin
	;;
  sparc-sun-solaris*)           # unix "simulator" port
        RTEMS_HOST=Solaris
	;;
  *) 
	;;
esac
AC_SUBST(RTEMS_HOST)
])dnl

# Like AC_CONFIG_HEADER, but automatically create stamp file. -*- Autoconf -*-

# Copyright 1996, 1997, 2000, 2001 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.

AC_PREREQ([2.52])

# serial 6

# AM_CONFIG_HEADER is obsolete.  It has been replaced by AC_CONFIG_HEADERS.
AU_DEFUN([AM_CONFIG_HEADER], [AC_CONFIG_HEADERS($@)])

dnl
dnl check-multiprocessing.m4,v 1.11.4.1 2004/08/18 07:23:00 ralf Exp
dnl

AC_DEFUN([RTEMS_CHECK_MULTIPROCESSING],
[dnl
AC_REQUIRE([RTEMS_ENABLE_MULTILIB])dnl
AC_REQUIRE([RTEMS_ENV_RTEMSBSP])dnl
AC_REQUIRE([RTEMS_TOP])dnl
AC_REQUIRE([RTEMS_CHECK_CPU])dnl
AC_REQUIRE([RTEMS_ENABLE_MULTIPROCESSING])dnl
AC_REQUIRE([RTEMS_BSP_ALIAS])dnl

AC_CACHE_CHECK([if wanting multiprocessing],
  [rtems_cv_want_multiprocessing],
  [
    AS_IF([test x"$multilib" = x"no"],
      [# no cpukit
       rtems_cv_want_multiprocessing="$enable_multiprocessing"
      ],[
#HACK: Should check for RTEMS_MULTIPROCESSING in cpuopts.h, instead
       rtems_cv_want_multiprocessing="$enable_multiprocessing"
      ])
  ])

AS_IF([test "$rtems_cv_want_multiprocessing" = "yes"],
[
  AC_CACHE_CHECK([whether BSP supports multiprocessing],
  [rtems_cv_HAS_MP],[
    if test -d "$srcdir/${RTEMS_TOPdir}/c/src/lib/libbsp/${RTEMS_CPU}/${RTEMS_BSP_FAMILY}/shmsupp"; then
        rtems_cv_HAS_MP="yes" ;
    else
        rtems_cv_HAS_MP="no";
    fi
  ])
if test $rtems_cv_HAS_MP = "no"; then
AC_MSG_ERROR([multiprocessing requested but not supported])
fi
],[rtems_cv_HAS_MP="no";])

AC_SUBST(HAS_MP,[$rtems_cv_HAS_MP])
])

dnl This provides configure definitions used for multilib support

dnl parts of these macros are derived from newlib-1.8.2's multilib support

AC_DEFUN([RTEMS_ENABLE_MULTILIB],
[
AC_ARG_ENABLE(multilib,
AC_HELP_STRING([--enable-multilib],
[build many library versions (default=no)]),
[case "${enableval}" in
  yes) multilib=yes ;;
  no)  multilib=no ;;
  *)   AC_MSG_ERROR(bad value ${enableval} for multilib option) ;;
 esac], [multilib=no])dnl

AM_CONDITIONAL(MULTILIB,test x"${multilib}" = x"yes")
])

dnl check-cpu.m4,v 1.6.2.1 2004/08/18 07:23:00 ralf Exp

dnl check if RTEMS support a cpu
AC_DEFUN([RTEMS_CHECK_CPU],
[dnl
AC_REQUIRE([RTEMS_TOP])
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])

# Is this a supported CPU?
AC_MSG_CHECKING([if cpu $RTEMS_CPU is supported])
# FIXME: Temporary hack
if test -d "$srcdir/$RTEMS_TOPdir/cpukit/score/cpu/$RTEMS_CPU"; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_ERROR(no)
fi
])dnl


dnl enable-multiprocessing.m4,v 1.6.2.1 2004/08/18 07:23:00 ralf Exp

AC_DEFUN([RTEMS_ENABLE_MULTIPROCESSING],
[
AC_ARG_ENABLE(multiprocessing,
[AC_HELP_STRING([--enable-multiprocessing],
[enable multiprocessing interface])],
[case "${enable_multiprocessing}" in 
  yes) ;;
  no) ;;
  *)  AC_MSG_ERROR(bad value ${enableval} for enable-multiprocessing option) ;;
esac],[enable_multiprocessing=no])
])

dnl
dnl  bsp-alias.m4,v 1.16.2.3 2005/10/05 19:31:20 joel Exp
dnl 

dnl _RTEMS_BSP_ALIAS(BSP_ALIAS,RTEMS_BSP_FAMILY)
dnl Internal subroutine to RTEMS_BSP_ALIAS
AC_DEFUN([_RTEMS_BSP_ALIAS],
[# account for "aliased" bsps which share source code
  case $1 in
    simcpu32)     $2=sim68000         ;; # BSVC CPU32 variant
    c3xsim)       $2=c4xsim           ;; # TI C3x Simulator in gdb
    mcp750)       $2=motorola_powerpc ;; # Motorola PPC board variant
    mvme2100)     $2=motorola_powerpc ;; # Motorola PPC board variant
    mvme2307)     $2=motorola_powerpc ;; # Motorola PPC board variant
    mtx603e)      $2=motorola_powerpc ;; # Motorola PPC board variant
    mvme162lx)    $2=mvme162          ;; # m68k - mvme162 board variant
    gen68360_040) $2=gen68360         ;; # m68k - 68360 in companion mode
    p4600)        $2=p4000            ;; # mips64orion - p4000 board w/IDT 4600
    p4650)        $2=p4000            ;; # mips64orion - p4000 board w/IDT 4650
    mbx8*)        $2=mbx8xx           ;; # MBX821/MBX860 board
    pc386dx)      $2=pc386            ;; # i386 - PC w/o FPU
    pc486)        $2=pc386            ;; # i386 - PC with i486DX
    pc586)        $2=pc386            ;; # i386 - PC with Pentium
    pc686)        $2=pc386            ;; # i386 - PC with PentiumPro
    pck6)         $2=pc386            ;; # i386 - PC with K6
    bare*)        $2=bare             ;; # EXP: bare-aliases
    sis)     	  $2=erc32            ;; # ERC32 SIS simulator
    leon2)     	  $2=leon2            ;; # leon with fpu
    leon3)     	  $2=leon3            ;; # another leon variant
    simsh7032)    $2=shsim            ;; # SH7032 simulator
    simsh7045)    $2=shsim            ;; # SH7045 simulator
    *)            $2=$1;;
  esac]
)

dnl RTEMS_BSP_ALIAS(BSP_ALIAS,RTEMS_BSP_FAMILY)
dnl convert a bsp alias $1 into its bsp directory RTEMS_BSP_FAMILY
AC_DEFUN([RTEMS_BSP_ALIAS],
[_RTEMS_BSP_ALIAS(m4_if([$1],,[$RTEMS_BSP],[$1]),
  m4_if([$2],,[RTEMS_BSP_FAMILY],[$2]))]
)

dnl
dnl prog-cc.m4,v 1.18.2.2 2004/08/18 07:23:00 ralf Exp
dnl 
dnl Check for target gcc
dnl

AC_DEFUN([RTEMS_PROG_CC],
[
AC_BEFORE([$0], [AC_PROG_CPP])dnl
AC_BEFORE([$0], [AC_PROG_CC])dnl
AC_BEFORE([$0], [RTEMS_CANONICALIZE_TOOLS])dnl

_RTEMS_FLAGS([CFLAGS],
  ["\$(CPU_CFLAGS) \$(RTEMS_CFLAGS_\$(VARIANT_V)_V) \$(CFLAGS_\$(VARIANT_V)_V) -g"])

RTEMS_CHECK_TOOL(CC,gcc)
test -z "$CC" && \
  AC_MSG_ERROR([no acceptable cc found in \$PATH])
AC_PROG_CC
AC_PROG_CPP

AM_CONDITIONAL(RTEMS_USE_GCC,test x"$GCC" = x"yes")
])

AC_DEFUN([RTEMS_PROG_CC_FOR_TARGET],
[
dnl check target cc
RTEMS_PROG_CC
dnl check if the compiler supports -isystem
RTEMS_GCC_ISYSTEM
dnl check if the target compiler may use --pipe
RTEMS_GCC_PIPE
test "$rtems_cv_gcc_pipe" = "yes" && CC="$CC --pipe"

dnl check if the compiler supports --specs
RTEMS_GCC_SPECS

if test "$GCC" = yes; then
RTEMS_CFLAGS="$RTEMS_CFLAGS -Wall"
m4_if([$1],,[],[RTEMS_CFLAGS="$RTEMS_CFLAGS $1"])
fi

AS_IF([test x"$rtems_cv_gcc_isystem" = x"yes"],[
  RTEMS_CPPFLAGS="-isystem \$(PROJECT_INCLUDE)"],[
  RTEMS_CPPFLAGS="-I\$(PROJECT_INCLUDE)"
])
AC_SUBST(RTEMS_CPPFLAGS)
])

dnl
dnl canonicalize-tools.m4,v 1.16.2.1 2004/08/18 07:23:00 ralf Exp
dnl
dnl Set target tools
dnl

AC_DEFUN([RTEMS_CANONICALIZE_TOOLS],
[AC_REQUIRE([RTEMS_PROG_CC])dnl

dnl FIXME: What shall be done if these tools are not available?
  RTEMS_CHECK_TOOL(AR,ar,no)
  RTEMS_CHECK_TOOL(AS,as,no)
  RTEMS_CHECK_TOOL(LD,ld,no)
  RTEMS_CHECK_TOOL(NM,nm,no)

dnl special treatment of ranlib
  RTEMS_CHECK_TOOL(RANLIB,ranlib,:)

dnl NOTE: These may not be available if not using gnutools
  RTEMS_CHECK_TOOL(OBJCOPY,objcopy,no)
  RTEMS_CHECK_TOOL(SIZE,size,no)
  RTEMS_CHECK_TOOL(STRIP,strip,:)
])

dnl check-tool.m4,v 1.2.4.1 2004/08/18 07:23:00 ralf Exp

dnl RTEMS_CHECK_TOOL(VARIABLE, PROG-TO-CHECK-FOR[, VALUE-IF-NOT-FOUND [, PATH]])
AC_DEFUN([RTEMS_CHECK_TOOL],
[
  AS_IF([test "x$build_alias" != "x$host_alias"],
    [rtems_tool_prefix=${ac_tool_prefix}])
  AC_CHECK_PROG($1, ${rtems_tool_prefix}$2, ${rtems_tool_prefix}$2, $3, $4)
])


AC_DEFUN([_RTEMS_FLAGS],[
AS_IF([test -n "[$]{$1}"],
  [RTEMS_$1=[$]{$1}],
  [RTEMS_$1=$2])
AC_SUBST([RTEMS_$1])
])

dnl
dnl gcc-isystem.m4,v 1.1.2.2 2004/08/18 07:23:00 ralf Exp
dnl
dnl Check whether the gcc accepts -isystem
dnl

AC_DEFUN([RTEMS_GCC_ISYSTEM],
[AC_REQUIRE([RTEMS_PROG_CC])
AC_CACHE_CHECK(whether $CC accepts -isystem,rtems_cv_gcc_isystem,
[
rtems_cv_gcc_isystem=no
if test x"$GCC" = x"yes"; then
cat << EOF > conftest.h
int conftest123();
EOF
cat << EOF > conftest.c
#include <conftest.h>
int conftest123() {}
EOF
  if test -z "`${CC} -isystem./ -c conftest.c 2>&1`";then
    rtems_cv_gcc_isystem=yes
  fi
fi
rm -f conftest*
])])

dnl
dnl gcc-pipe.m4,v 1.8.6.1 2004/08/18 07:23:00 ralf Exp
dnl
dnl Check whether the target compiler accepts -pipe
dnl

AC_DEFUN([RTEMS_GCC_PIPE],
[AC_REQUIRE([RTEMS_PROG_CC]) 
AC_REQUIRE([AC_CANONICAL_HOST])
AC_CACHE_CHECK(whether $CC accepts --pipe,rtems_cv_gcc_pipe,
[
rtems_cv_gcc_pipe=no
if test x"$GCC" = x"yes"; then
    echo 'void f(){}' >conftest.c
    if test -z "`${CC} --pipe -c conftest.c 2>&1`";then
      rtems_cv_gcc_pipe=yes
    fi
    rm -f conftest*
fi
])
])

dnl
dnl gcc-specs.m4,v 1.5.6.1 2004/08/18 07:23:00 ralf Exp
dnl
dnl Check whether the target compiler accepts -specs
dnl

AC_DEFUN([RTEMS_GCC_SPECS],
[AC_REQUIRE([RTEMS_PROG_CC])
AC_CACHE_CHECK(whether $CC accepts -specs,rtems_cv_gcc_specs,
[
rtems_cv_gcc_specs=no
if test x"$GCC" = x"yes"; then
  touch confspec
  echo 'void f(){}' >conftest.c
  if test -z "`${CC} -specs confspec -c conftest.c 2>&1`";then
    rtems_cv_gcc_specs=yes
  fi
fi
rm -f confspec conftest*
])])

