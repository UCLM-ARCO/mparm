#!/bin/bash

V=4.7
ARCH=-arm-rtems${V}

pkg_extract() {
    if test ! -e $1 ; then return ; fi
    echo "Extracting $1 ... "
    case $1 in
	*.tar.bz2) 
		BASE=$(basename $1 .tar.bz2 )
		EXTRACT="tar xjf"
		;;
	*.tbz) 
		BASE=$(basename $1 .tbz )
		EXTRACT="tar xjf"
		;;
	*.tar.gz)
		BASE=$(basename $1 .tar.gz ) 
		EXTRACT="tar xzf"
		;;
	*.tgz)
		BASE=$(basename $1 .tgz ) 
		EXTRACT="tar xzf"
		;;
    esac

    PKG=$(echo $BASE | sed -e 's/\([A-Z_a-z-]*\)-\([0-9][0-9.]*\)[-A-Z0-9]*/\1/g' )
    DEST=$(echo $BASE | sed -e 's/\([A-Z_a-z-]*\)\([0-9][0-9.]*\)[-A-Z0-9]*/\1\2/g' )
    VER=$(echo $BASE | sed -e 's/\([A-Z_a-z-]*\)\([0-9][0-9.]*\)[-A-Z0-9]*/\2/g' )
    ORIG=$(echo $DEST | sed -e 's/-\([0-9][0-9.]*\)/_\1.orig.tar.gz/g' )
    DIR=$(basename $(tar tjf $1 | head -1 ) )

    cat <<EOF
	PKG  = $PKG
	VER  = $VER
	DEST = $DEST ($DIR -> $DEST)
	ORIG = $ORIG
EOF

    $EXTRACT $1
    if [ $DIR != $DEST ] ; then
        mv $DIR $DEST
    fi
    ln -f $1 $ORIG
    rsync -aq --exclude=.svn $(dirname $0)/$PKG/ $DEST/debian
}

pkg_copy() {
    if test ! -e $1 ; then return ; fi
    echo "Copying $1 ... "
    
    BASE=$(basename "$1" \
	| sed -e 's|\.tar\.bz2||' -e 's|\.tar\.gz||'  -e 's|\.tbz||'  -e 's|\.tgz||' )
    PKG=$(echo $BASE | sed -e 's/\([A-Z_a-z-]*\)-\([0-9][0-9.]*\)[-A-Z0-9]*/\1/g' )
    DEST=$(echo $BASE | sed -e "s/\\([A-Z_a-z-]*\\)-\\([0-9][0-9.]*\\)[-A-Z0-9]*/\\1$ARCH-\\2/g" )
    VER=$(echo $BASE | sed -e 's/\([A-Z_a-z-]*\)\([0-9][0-9.]*\)[-A-Z0-9]*/\2/g' )
    ORIG=$(echo $DEST | sed -e 's/-\([0-9][0-9.]*\)/_\1.orig.tar.gz/g' )

    cat <<EOF
	PKG  = $PKG
	VER  = $VER
	DEST = $DEST
	ORIG = $ORIG
EOF

    mkdir -p $DEST
    ln -f $1 $DEST/
    tar czf $ORIG $DEST
    rsync -aq --exclude=.svn $(dirname $0)/$PKG/ $DEST/debian
}

pkg_copy_gcc() {
    if test ! -e $1 ; then return ; fi
    echo "Copying gcc files from $1 ... "

    COR=$(echo "$1" | sed -e 's/\([a-z]*\)-\([+a-z]*\)-\([0-9][0-9.]*\)/\1-core-\3/g' )
    CPP=$(echo "$1" | sed -e 's/\([a-z]*\)-\([+a-z]*\)-\([0-9][0-9.]*\)/\1-g++-\3/g' )
    NEWLIB=$(ls -1 $(dirname "$1")/newlib* | head -1 )
    DIR=$(basename "$COR" .tar.bz2 )
    PKG=$(echo "$DIR" | sed -e "s/\\([a-z]*\\)-core-\\([0-9][0-9.]*\\)/\\1$ARCH-\\2/g" )
    ORIG=$(echo "$PKG" | sed -e 's/-\([0-9][0-9.]*\)/_\1.orig.tar.gz/g' )

    cat <<EOF
	PKG        = $PKG
	CORE       = $COR
	CPLUSPLUS  = $CPP
	NEWLIB     = $NEWLIB
	ORIG       = $ORIG
EOF

    mkdir -p $PKG
    ln -f $COR $PKG/
    ln -f $CPP $PKG/
    ln -f $NEWLIB $PKG/
    tar czf $ORIG $PKG
    rsync -aq --exclude=.svn $(dirname $0)/gcc/ $PKG/debian
}

extract() {
    for i in "$@" ; do
	case "$i" in
	    -clean) 
		rm -rf $(ls -1 |grep -- "-[0-9][0-9.]*")
		rm -f *.orig.tar.gz *.deb *.dsc *.changes *.build *.diff.gz *~ */*~
		;;
	    *binutils-*) pkg_copy "$i" ;;
	    *rtems-*)    ARCH=${V} pkg_copy "$i" ;;
	    *gcc-*)      pkg_copy_gcc "$i" ;;
	    *.tar.bz2)   pkg_extract "$i" ;;
	    *.tar.gz)    pkg_extract "$i" ;;
	    *) echo "Unknown file format: $i" 1>&2 ;;
	esac
    done
}

if test $# -gt 0 ; then
    extract "$@"
else
    extract orig/binutils*bz2 orig/gcc-core*bz2 orig/rtems*bz2
fi
