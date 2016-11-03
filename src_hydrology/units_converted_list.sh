#!/bin/sh
# ---------------------------------------------
# units_converted_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 application [datatype]" 1>&2
	exit 1
fi

application=$1

if [ "$#" -eq 1 ]
then
	datatype="datatype"
else
	datatype=$2
fi

units_converted_table=`get_table_name $application units_converted`

if [ "$datatype" = "" -o "$datatype" = "datatype" ]
then
	# If not called with a preprompt drop down
	# ----------------------------------------
	echo "	select units,units_converted				\
		from $units_converted_table				\
		order by units,units_converted;"			|
	sql.e '^'
else
	# If called with a preprompt drop down
	# ------------------------------------
	datatype_table=`get_table_name $application datatype`
	datatype_and_clause="and $datatype_table.datatype in (`single_quotes_around.e $datatype`)"

	echo "	select	$units_converted_table.units,			\
			$units_converted_table.units_converted		\
		from $units_converted_table,$datatype_table		\
		where	$units_converted_table.units =			\
			$datatype_table.units				\
			$datatype_and_clause				\
		order by	$units_converted_table.units,		\
				$units_converted_table.units_converted;"|
	sql.e '^'							|
	sort -u
fi

exit 0
