#!/bin/sh
# ---------------------------------
# form4folder.sh
# ---------------------------------
#
# Copyright 2000-01 Tim Riley
# ---------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 entity folder" 1>&2
	exit 1
fi

entity=$1
folder=$2

folder_table=`get_table_name $entity folder`

echo "select form							\
      from $folder_table 						\
      where $folder_table.folder = '$folder';"				|
sql.e
