#!/bin/sh
# ---------------------------------------------
# units_converted_flow_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

datatype=`get_table_name $application datatype`
units_converted=`get_table_name $application units_converted`

datatype_and_clause="and $datatype.datatype = 'flow'"

echo "	select	distinct $units_converted.units_converted		\
	from $units_converted, $datatype				\
	where $units_converted.units = $datatype.units			\
	  $datatype_and_clause						\
	order by $units_converted.units_converted;"			|
sql 									|
cat

exit 0

