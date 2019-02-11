#!/bin/bash

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

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 ignored" 1>&2
	exit 1
fi

current_date=$(date.e 0 | piece.e ':' 0)
table=$(get_table_name $application concert)
select="concert_title, date"
order="date"

where="date >= '$current_date'"
echo "select $select from $table where $where order by $order;" | sql.e

where="date < '$current_date'"
echo "select $select from $table where $where order by $order;" | sql.e

exit 0
