:
# $APPASERVER_HOME/src_hydrology/date_time_between_where.sh
# ---------------------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------------------

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 begin_date begin_time end_date end_time" 1>&2
	exit 1
fi

begin_date=$1
begin_time=$2
end_date=$3
end_time=$4

echo "( ( measurement_date = '$begin_date' and measurement_time >= '$begin_time' and measurement_time <= '2359' ) or ( measurement_date = '$begin_date' and measurement_time = 'null' ) or ( measurement_date = '$end_date' and measurement_time = 'null' ) or ( measurement_date > '$begin_date' and measurement_date < '$end_date' ) or ( measurement_date = '$end_date' and measurement_time >= '0000' and measurement_time <= '$end_time' ) )"

exit 0
