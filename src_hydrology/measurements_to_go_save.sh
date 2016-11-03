:
if [ "$#" -ne 5 ]
then
	echo "Usage: $0 application station datatype begin_date end_date" 1>&2
	exit 1
fi

application=$1
station=$2
datatype=$3
begin_date=$4
end_date=$5

appaserver_mount_point=`get_appaserver_mount_point`

output_measurement_data user session $application $station $datatype $begin_date $end_date email_address hourly validation_level aggregate_statistic n stdout
