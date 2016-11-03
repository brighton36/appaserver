#!/bin/sh
# ---------------------------------------------
# folders4role.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

# echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo \
	"Usage: $0 application role insert,update,lookup" 1>&2
	exit 1
fi

application=$1
role=$2
permission_list=`single_quotes_around $3`

role_folder=`get_table_name $application role_folder`

echo "select folder						\
      from $role_folder						\
      where role = '$role'					\
	and permission in ( "$permission_list" )		\
	and folder != 'null'					\
      order by folder;"						|
sql.e								|
sort -u

