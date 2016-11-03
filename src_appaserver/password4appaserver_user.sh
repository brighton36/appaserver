#!/bin/sh
# ---------------------------------------------
# password4appaserver_user.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 login_name application" 1>&2
	exit 1
fi

login_name=$1
application=$2

table_name=`get_table_name $application appaserver_user`

echo "select password				\
      from $table_name		 		\
      where login_name = '$login_name';" 	|
sql.e

exit 0
