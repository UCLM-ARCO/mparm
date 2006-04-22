#!/bin/sh

# Package versions
BINUTILS_VER=2.16.1
GCC_VER=4.1.0
NEWLIB_VER=1.14.0
RTEMS_VER=4.6.99

# Note that RTEMS version is always CVS HEAD.  The user specified
# version number only affects the generated compressed archive.

GNU=ftp://ftp.gnu.org/gnu
BU=$GNU/binutils/binutils-$BINUTILS_VER.tar.bz2
GCC=$GNU/gcc/gcc-$GCC_VER
GCC_CORE=$GCC/gcc-core-$GCC_VER.tar.bz2
GCC_CXX=$GCC/gcc-g++-$GCC_VER.tar.bz2

REDHAT=ftp://sources.redhat.com/pub
NEWLIB=$REDHAT/newlib/newlib-$NEWLIB_VER.tar.gz

RTEMS_CVS=:pserver:anoncvs@www.rtems.com:/usr1/CVS
RTEMS=rtems-$RTEMS_VER

FILES="$BU $GCC_CORE $GCC_CXX $NEWLIB"

mkdir -p orig
cd orig && (
    for i in $FILES ; do
	if [ -e $(basename $i ) ] ; then
	    echo "Skipping $i"
	else
	    echo -n "Getting $i ... "
	    if wget "$i" ; then
		echo Done.
	    else
		echo Failed.
	    fi
	fi
    done
    if [ -d $RTEMS ] ; then
	echo "Updating $RTEMS"
	( cd $RTEMS && cvs -Q -z9 update )
    else
	echo "Checking out $RTEMS_CVS/rtems in $RTEMS"
	cvs -Q -z9 -d $RTEMS_CVS co -d $RTEMS rtems
    fi
    echo "Packaging $RTEMS"
    tar cjf $RTEMS.tar.bz2  $RTEMS
)
