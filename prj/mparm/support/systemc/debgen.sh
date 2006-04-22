#!/bin/bash

if test $# -gt 0 ; then
    DIRS="$@"
else
    DIRS=$(echo */debian | sed -e 's|/debian||g')
fi

for i in $DIRS; do
	echo Building debian package of $i
	( cd $i && dpkg-buildpackage -us -uc -D -rfakeroot )
done
