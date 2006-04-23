#!/bin/sh

# Package versions
SKYEYE_VER=1.2-RC7-2
SWARM_VER=0.61

SF_MIRROR=ovh
SOURCEFORGE=http://$SF_MIRROR.dl.sourceforge.net/sourceforge
SKYEYE=$SOURCEFORGE/skyeye/skyeye-$SKYEYE_VER.tar.bz2

MWD=http://www.cl.cam.ac.uk/~mwd24
SWARM=$MWD/phd/bin/swarm-$SWARM_VER.tar.gz

FILES="$SKYEYE $SWARM"

mkdir -p orig
cd orig && (
    for i in $FILES ; do
	if [ -e $(basename $i ) ] ; then
	    echo "Skipping $i"
	else
	    echo -n "Getting $i ... "
	    echo "[wget \"$i\"]"
	    if wget "$i" ; then
		echo Done.
	    else
		echo Failed.
	    fi
	fi
    done
)
