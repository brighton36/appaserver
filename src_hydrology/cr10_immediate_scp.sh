#!/bin/sh
# ----------------------------------------------------
# $APPASERVER_HOME/src_hydrology/cr10_immediate_scp.sh
# ----------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 cr10file" 1>&2
	exit 1
fi

cr10file=$1

cr10_directory="/var/export"

source="$cr10_directory/$cr10file"
destination="bonefish:/$cr10_directory/$cr10file"

scp $source $destination

exit 0
