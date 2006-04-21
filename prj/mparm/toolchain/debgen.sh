#!/bin/bash

DIRS=$(echo */debian | sed -e 's|/debian||g')
for i in $DIRS; do
	echo Building debian package of $i
	( cd $i && dpkg-buildpackage -us -uc -D -rfakeroot )
done
