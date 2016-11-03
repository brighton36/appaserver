#!/bin/bash
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
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
