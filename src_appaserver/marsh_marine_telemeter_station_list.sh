:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 entity" 1>&2
	exit 1
fi

entity=$1

table_name=`get_table_name $entity station`

echo "	select station						\
	from $table_name 					\
	where station_type = 'marsh'				\
	or station_type = 'marine'				\
	  and telemeter_yn = 'y'				\
	order by station;"					|
sql.e								|
toupper.e
