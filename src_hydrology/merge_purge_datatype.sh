:
# merge_purge_datatype.sh
# -----------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 old_datatype new_datatype | sql" 1>&2
        exit 1
fi

old_datatype=$1
new_datatype=$2

measurement_column_list="station,datatype,measurement_date,measurement_time"
tmp_file="/tmp/merge_purge_datatype_$$.dat"

echo "	select $measurement_column_list					\
	from measurement                                  		\
        where datatype = '$old_datatype';"                		|
sql '|'                                                 		|
tee_process.e "delete measurement $measurement_column_list > $tmp_file"	|
tr '|', ','								|
sed "s/$/|datatype=$new_datatype/"					|
update.e measurement station,datatype,measurement_date,measurement_time	|
cat
#
cat $tmp_file
/bin/rm -f $tmp_file

exit $?
