:
if [ "$#" -ne 1 ]
then
        echo "Usage: $0 application" 1>&2
        exit 1
fi

application=$1

table_name=`get_table_name $application enp_station`

echo "  select station                                          \
        from $table_name                                        \
        where run in ( 'north_taylor_slough', 'south_taylor_slough',
'shark_slough','road' )         \
          and telemeter_yn = 'y'                                \
        order by station;"                                      |
sql.e                                                           |
toupper.e
