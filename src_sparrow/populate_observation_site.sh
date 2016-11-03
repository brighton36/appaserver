#!/bin/bash

echo Starting: $0 $* 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application where" 1>&2
	exit 1
fi

application=$1

if [ "$2" != "" -a "$2" != "where" -a "$2" != "\$where" ]
then
	where="$2"
else
	where="1 = 1"
fi

table=`get_table_name $application observation_site`
select="concat(quad_sheet,'^',site_number,'|',site_id)"
order="quad_sheet,site_number"

echo "select $select from $table where $where order by $order;"	|
sql.e

exit 0
