#!/bin/sh
# ---------------------------------
# prompt_table_yn4folder.sh
# ---------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 entity application folder" 1>&2
	exit 1
fi

entity=$1
application=$2
folder=$3

table_name=`get_table_name $entity "" folder`

echo "select prompt_table_yn			\
      from $table_name			 	\
      where application ='$application'		\
	and folder = '$folder';" 		|
sql.e
