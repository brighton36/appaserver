#!/bin/bash

echo Starting: $0 $* 1>&2

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 ignored where" 1>&2
	exit 1
fi

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
