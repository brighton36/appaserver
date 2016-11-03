:
#-------------------------------------
#src_hydrology/backup_model_results.sh
#-------------------------------------

days_to_keep=7

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 destination_directory [days-to-keep]" 1>&2
	echo "This script backs up the model_results table by station." 1>&2
	echo "The default days-to-keep is ${days_to_keep}." 1>&2
	exit 1
fi

destination_directory=$1

if [ "$#" -eq 2 ]
then
	days_to_keep=$2
fi

now_stamp=`now.sh ymd`

select="model,alt,station,datatype,date,value"

backup_model_results()
{
	station=$1

	output_filename="${destination_directory}/model_results_${station}_${now_stamp}.dat"
	gzip_filename="${output_filename}.gz"

	echo "Creating $gzip_filename"
	echo "#${select}" > $output_filename

	echo "	select $select
		from model_results
		where station = '$station'"		|
	sql_quick.e >> $output_filename

	gzip -f $output_filename
}

# Main
# ----
for station in `echo "select distinct station from alt_station;" | sql.e`
do
	backup_model_results $station
done

# Clean out old files
# -------------------
find ${destination_directory} -type f -name 'model_results_*.gz' -mtime +$days_to_keep -exec rm {} \;

exit 0
