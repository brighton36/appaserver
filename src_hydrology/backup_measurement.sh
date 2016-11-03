:
#-----------------------------------
#src_hydrology/backup_measurement.sh
#-----------------------------------

days_to_keep=7

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 destination_directory [days-to-keep]" 1>&2
	echo "This script backs up the measurement table by agency." 1>&2
	echo "The default days-to-keep is ${days_to_keep}." 1>&2
	exit 1
fi

destination_directory=$1

if [ "$#" -eq 2 ]
then
	days_to_keep=$2
fi

application=hydrology

now_stamp=`now.sh ymd`

station_table=`get_table_name $application station`
agency_table=`get_table_name $application agency`
measurement_table=`get_table_name $application measurement`

select="station,datatype,measurement_date,measurement_time,measurement_value,last_validation_date,last_person_validating,measurement_update_method,last_validation_process,reason_value_missing,delta_time"

backup_measurement()
{
	in_clause=$1
	agency=$2

	output_filename="${destination_directory}/measurement_${agency}_${now_stamp}.dat"
	gzip_filename="${output_filename}.gz"

	echo "#${select}" > $output_filename

	echo "	select $select
		from $measurement_table
		where station in $in_clause"	|
	sql_quick.e >> $output_filename

	gzip -f $output_filename
	echo "Created $gzip_filename"
}

backup_agency()
{
	agency=$1

	station_list=`
	echo "select station from $station_table where agency = '$agency';"  |
		sql.e							     |
		joinlines.e ','						     |
		single_quotes_around.e`
	in_clause="(${station_list})"
	backup_measurement "$in_clause" "$agency"
}

# Main
# ----
for agency in `echo "select agency from $agency_table;" | sql.e`
do
	backup_agency $agency
done

# Clean out old files
# -------------------
find ${destination_directory}/ -type f -name 'measurement_*.gz' -mtime +$days_to_keep -exec rm {} \;

exit 0
