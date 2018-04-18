#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_vegetation/populate_cell_50x50.sh
# ---------------------------------------------------------------
if [ "$#" -eq 0 ]
then
	echo "Usage: $0 application [where]" 1>&2
	exit 1
fi

echo $0 $* 1>&2

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
else
	export DATABASE=$application
fi

if [ $# -eq 2 -a "$2" != "where" ]
then
	where=$2
else
	where="1 = 1"
fi

table=$(get_table_name $application cell_50x50)
select="UTMX,concat( UTMY, ' [', cell_id, ']' )"

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
