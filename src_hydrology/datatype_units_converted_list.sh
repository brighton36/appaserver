#!/bin/sh
# ---------------------------------------------
# datatype_units_converted_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 application datatype" 1>&2
	exit 1
fi

application=$1

if [ "$#" -eq 1 ]
then
	datatype="datatype"
else
	datatype=$2
fi

datatype=$2

datatype_table=`get_table_name $application datatype`
units_converted_table=`get_table_name $application units_converted`

if [ "$datatype" = "" -o "$datatype" = "datatype" ]
then
	# If not called with a preprompt drop down
	# ----------------------------------------
	datatype_and_clause="and 1 = 1"
else
	# If called with a preprompt drop down
	# ------------------------------------
	datatype_and_clause="and $datatype_table.datatype in (`single_quotes_around.e $datatype`)"

fi

(
echo "	select	datatype,units_converted			\
	from $units_converted_table,$datatype_table		\
	where	$units_converted_table.units =			\
		$datatype_table.units				\
		$datatype_and_clause				\
	order by datatype,units_converted;"			|
sql.e '^'							;
echo "	select	datatype,units					\
	from $datatype_table					\
	where	1 = 1						\
		$datatype_and_clause				\
	order by datatype;"					|
sql.e '^'
) | sort -u

exit 0
