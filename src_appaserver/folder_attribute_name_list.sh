#!/bin/sh
# -------------------------------------------------
# src_appaserver/folder_attribute_name_list.sh
# -------------------------------------------------
#
# Freely available software: see Appaserver.org
# -------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 folder" 1>&2
	exit 1
fi

folder=$1

folder_attribute_record_list.sh $folder '^'			|
piece.e '^' 0							|
joinlines.e ','							|
cat

exit 0
