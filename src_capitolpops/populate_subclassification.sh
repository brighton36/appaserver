#!/bin/bash
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application where state" 1>&2
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

parameter_where=$2
state=$3

table=subclassification
select="subclassification"

if [ "$state" = "insert" ]
then
	where="$parameter_where and subclassification != 'donation'"
else
	where="$parameter_where"
fi

echo "select $select from $table where $where order by $select;"	|
sql.e

exit 0
