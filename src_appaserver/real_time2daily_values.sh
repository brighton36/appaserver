:
# real_time2daily_values.sh
#
# Appahost
# -------------------------

if [ $# -ne 3 ]
then
	echo "Usage: $0 yyyy_mm_dd station datatype" 1>&2
	exit 1
fi

if [ `uname -n` = "snook" ]
then
	cd /opt/html/cgi
fi

yyyy_mm_dd=$1
station=$2
datatype=$3

echo "	select	station, 						\
		datatype, 						\
		measurement_date, 					\
		measurement_time, 					\
		measurement_value					\
	from measurement_real_time					\
	where station = '$station'					\
	  and datatype = '$datatype'					\
	  and measurement_date = '$yyyy_mm_dd'				\
	order by station, datatype, measurement_date;"			|
sql.e ','								|
real_time2daily_values $datatype 2					|
sed 's/,/|/g'								|
insert	measurement_daily_values 					\
		station,datatype,measurement_date,daily_value		|
sql.e
