:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 entity" 1>&2
	exit 1
fi

entity=$1

PATH=$PATH:.:/opt/html/utility; export PATH

table_name=`get_table_name $entity station`

echo "	select station						\
	from $table_name 					\
	where station_type = 'marine'				\
	  and telemeter_yn = 'y'				\
	order by station;"					|
sql.e								|
toupper.e
