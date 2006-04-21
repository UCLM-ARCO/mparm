#!/bin/bash

pkg_extract() {
    if test ! -e $1 ; then return ; fi
    echo "Extracting $1 ... "
    case $1 in
	*.tar.bz2) 
		BASE=$(basename $1 .tar.bz2 )
		EXTRACT="tar xjf"
		INSPECT="tar tjf"
		;;
	*.tbz) 
		BASE=$(basename $1 .tbz )
		EXTRACT="tar xjf"
		INSPECT="tar tjf"
		;;
	*.tar.gz)
		BASE=$(basename $1 .tar.gz ) 
		EXTRACT="tar xzf"
		INSPECT="tar tzf"
		;;
	*.tgz)
		BASE=$(basename $1 .tgz ) 
		EXTRACT="tar xzf"
		INSPECT="tar tzf"
		;;
    esac

    PKG=$(echo $BASE | sed -e 's/\([A-Z_a-z-]*\)-\([0-9][0-9.]*\)[-A-Z0-9]*/\1/g' )
    DEST=$(echo $BASE | sed -e 's/\([A-Z_a-z-]*\)\([0-9][0-9.]*\)[-A-Z0-9]*/\1\2.0/g' )
    VER=$(echo $BASE | sed -e 's/\([A-Z_a-z-]*\)\([0-9][0-9.]*\)[-A-Z0-9]*/\2/g' )
    ORIG=$(echo $DEST | sed -e 's/-\([0-9][0-9.]*\)/_\1.orig.tar.gz/g' )
    DIR=$(basename $($INSPECT $1 | head -1 ) )

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
    tar czf $ORIG $DEST
    rsync -aq --exclude=.svn $(dirname $0)/$PKG/ $DEST/debian
}

pkg_copy() {
    if test ! -e $1 ; then return ; fi
    echo "Copying $1 ... "
    
    BASE=$(basename "$1" \
	| sed -e 's|\.tar\.bz2||' -e 's|\.tar\.gz||'  -e 's|\.tbz||'  -e 's|\.tgz||' )
    PKG=$(echo $BASE | sed -e 's/\([A-Z_a-z-]*\)-\([0-9][0-9.]*\)[-A-Z0-9]*/\1/g' )
    DEST=$(echo $BASE | sed -e "s/\\([A-Z_a-z-]*\\)-\\([0-9][0-9.]*\\)[-A-Z0-9]*/\\1-\\2/g" )
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

extract() {
    for i in "$@" ; do
	case "$i" in
	    -clean) 
		rm -rf $(ls -1 |grep -- "-[0-9][0-9.]*")
		rm -f *.orig.tar.gz *.deb *.dsc *.changes *.build *.diff.gz *~ */*~
		;;
	    *.tar.bz2)   pkg_extract "$i" ;;
	    *.tar.gz)    pkg_extract "$i" ;;
	    *) echo "Unknown file format: $i" 1>&2 ;;
	esac
    done
}

if test $# -gt 0 ; then
    extract "$@"
else
    extract orig/*
fi
