:
# src_sparrow/populate_physical_parameter.sh
# ------------------------------------------

echo Starting: $0 $* 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

# See MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER
# ----------------------------------------
select="physical_parameter,'|',physical_parameter,'[',units,']'"

table=$(get_table_name $application physical_parameter)

echo "select $select from $table order by physical_parameter;"		|
sql.e									|
sed 's/\^//g'								|
cat

exit 0
