#!/bin/bash

echo $0 $* 1>&2

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

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 ignored one2m_folder state" 1>&2
	exit 1
fi

one2m_folder=$2
state=$3

table=music_folder
select="concat(folder_number, '|', folder_number, '^', instrument, '^', part_number )"

if [ "$state" = "lookup" ]
then
	where="1 = 1"
else
	where="1 = 1"
	#where="not exists ( select 1 from band_member where band_member.folder_number = music_folder.folder_number ) or exists ( select 1 from band_member where full_name = '$full_name' )"
fi

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
