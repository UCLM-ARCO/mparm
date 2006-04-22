#!/bin/bash

canonicalize_pkg() {
    echo "$1" | sed \
	-e 's/\.v\([0-9]\)$/.\1/g' \
	-e "s/systemc/systemc$2/g"
}

pkg_extract() {
    if test ! -e $1 ; then return ; fi
    echo "Extracting $1 ... "
    DIR=$(basename $1 .tgz )
    VER=$(echo $DIR | sed -e 's/\([A-Z_a-z-]*\)\([0-9]*\).\([0-9]*\).[0-9v]*/\2\3/g' )
    DEST=$(canonicalize_pkg $DIR $VER)
    DEB=$(echo $DEST | sed -e 's/-[0-9][0-9.]*//g' )
    ORIG=$(echo $DEST | sed -e 's/-\([0-9][0-9.]*\)/_\1.orig.tar.gz/g' )
    tar xzf $1
    ln $1 $ORIG
    if [ $DIR != $DEST ] ; then
        mv $DIR $DEST
    fi
    rsync -aq --exclude=.svn $(dirname $0)/$DEB/ $DEST/debian
}

extract() {
    for i in "$@" ; do
	case "$i" in
	    -clean) 
		rm -rf $(ls -1 |grep -- "-[0-9][0-9.]*")
		rm -f *.orig.tar.gz *.deb *.dsc *.changes *.build *.diff.gz *~ */*~
		;;
	    *.tgz) pkg_extract "$i" ;;
	    *) echo "Unknown file format: $i" 1>&2 ;;
	esac
    done
}

if test $# -gt 0 ; then
    extract "$@"
else
    extract orig/*.tgz
fi
