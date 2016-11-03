#!/bin/sh
# ------------------------------
# permission4role_folder.sh
# ------------------------------

# echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 4 ]
then
	echo \
	"Usage: $0 application session role folder" 1>&2
	exit 1
fi

application=$1
session=$2
role=$3
folder=$4

role_folder=`get_table_name $application role_folder`

echo "select permission							\
      	from $role_folder						\
      	where role = '$role'						\
	  and folder = '$folder';"					|
sql.e 

exit 0
