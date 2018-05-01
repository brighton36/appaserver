#!/bin/sh
# ---------------------------------------------
# orphan_process_folder_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
fi

if [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 ignored" 1>&2
	exit 1
fi

login_name=$2

folder=`get_table_name $application folder`

echo "	select folder							\
	from $folder;"							|
sql.e									|
cat

exit 0
