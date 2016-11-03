#!/bin/sh

tablespace=`cat tablespace.dat`
if [ "$tablespace" = "" ]
then
	echo "Error in $0: cannot read from tablespace.dat" 1>&2
	exit 1
fi

table_name=measurement

echo "drop table $table_name;" | sql.e '^' oracle
echo "create table $table_name (station char (12) not null,datatype char (25) not null,measurement_date date not null,measurement_time char (4) not null,measurement_date_time date,measurement_value number (12,4),last_validation_date date,last_person_validating char (20),measurement_update_method char (20),last_validation_process char (20),reason_value_missing char (20),delta_time char (14)) tablespace $tablespace;" | sql.e '^' oracle

exit 0


