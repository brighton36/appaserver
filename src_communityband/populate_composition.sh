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

table=$(get_table_name $application composition)
select="composition"
where="location in ('main_folder','christmas_folder')"

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
