#!/bin/sh
# ---------------------------------
# insert_rows4folder.sh
# ---------------------------------
#
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 entity application folder" 1>&2
	exit 1
fi

entity=$1
application=$2
folder_name=$3

folder_table=`get_table_name $entity "" folder`
application_folder_attribute=`get_table_name				\
				$entity 				\
				"" 					\
				application_folder_attribute`

echo "select insert_rows_number						\
      from $folder_table, $application_folder_attribute			\
      where $application_folder_attribute.application = '$application'	\
	and $application_folder_attribute.folder = $folder_table.folder	\
        and $folder_table.folder = '$folder_name';"			|
sql.e									|
sort -u

