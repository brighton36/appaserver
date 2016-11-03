:
# update_datatype_ignore_duplicate.sh
# -----------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 old_datatype new_datatype | sql" 1>&2
        exit 1
fi

old_datatype=$1
new_datatype=$2

echo "	select  station,                                		\
	        datatype,                               		\
	        measurement_date,                       		\
	        measurement_time                        		\
	from measurement                                  		\
        where datatype = '$old_datatype';"                		|
sql ','                                                 		|
sed "s/$/|datatype=$new_datatype/"					|
update.e measurement station,datatype,measurement_date,measurement_time	|
cat
