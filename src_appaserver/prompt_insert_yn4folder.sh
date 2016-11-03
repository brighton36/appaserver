#!/bin/sh
# ---------------------------------
# prompt_insert_yn_for_folder.sh
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

echo "select prompt_insert_yn			\
      from $table_name			 	\
      where application ='$application'		\
	and folder = '$folder';" 		|
sql.e
