#!/bin/bash
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application one2m_folder state" 1>&2
	exit 1
fi

echo $0 $* 1>&2

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
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
