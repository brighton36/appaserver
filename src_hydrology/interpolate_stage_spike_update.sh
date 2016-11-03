:
# /opt/html/src/hydrology/interpolate_stage_spike_update.sh
# ---------------------------------------------------------

if [ "$#" -lt 4 ]
then
	echo "Usage: $0 station start_date end_date [tee_update_filename]" 1>&2
	exit 1
fi

station=$1
start_date=$2
end_date=$3

if [ "$#" -eq 4 ]
then
	tee_update_filename=$4
else
	tee_update_filename=/dev/tty
fi

interpolate_spike_update						\
     primary_key_delimiter=','						\
     value_delimiter='|'						\
     maximum_out_of_range=7						\
     subsequent_minus_previous_compare=0.03				\
     primary_key_name_comma_list=station,datatype,measurement_date,measurement_time \
     table_name=measurement						\
     value_column_name=measurement_value				|
tee $tee_update_filename						|
mysql.sh
