#!/bin/sh
# ---------------------------------------------
# get_role_folder_update_lookup_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo \
	"Usage: $0 application session role" 1>&2
	exit 1
fi

application=$1
session=$2
role=$3

(
get_role_folder_lookup_list.sh $application $session $role
get_role_folder_update_list.sh $application $session $role
) | sort -u

exit 0
