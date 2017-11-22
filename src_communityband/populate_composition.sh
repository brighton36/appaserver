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

table=$(get_table_name $application composition)
select="composition"
where="location in ('main_folder','christmas_folder')"

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
