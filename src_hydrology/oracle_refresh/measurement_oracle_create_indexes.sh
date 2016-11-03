#!/bin/sh

tablespace=`cat tablespace.dat`
if [ "$tablespace" = "" ]
then
	echo "Error in $0: cannot read from tablespace.dat" 1>&2
	exit 1
fi

table_name=measurement

echo "create unique index $table_name on $table_name (station,datatype,measurement_date,measurement_time) tablespace $tablespace;" | sql.e '^' oracle
echo "create index measurement_date_time on $table_name (measurement_date_time) tablespace $tablespace;" | sql.e '^' oracle

exit 0


