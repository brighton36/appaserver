:
if [ "$#" -ne 1 ]
then
        echo "Usage: $0 application" 1>&2
        exit 1
fi

application=$1

table_name=`get_table_name $application enp_station`

echo "  select station 			\
        from $table_name 		\
        where telemeter_yn = 'y' 	\
        order by station;" 		|
sql.e					|
toupper.e
