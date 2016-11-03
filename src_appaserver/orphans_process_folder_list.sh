#!/bin/sh
# ---------------------------------------------
# orphan_process_folder_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1
login_name=$2

folder=`get_table_name $application folder`

#echo "	select folder							\
#	from $folder							\
#	where fix_orphans_process_yn = 'y';"				|
#sql.e									|
#cat

echo "	select folder							\
	from $folder;"							|
sql.e									|
cat

exit 0
