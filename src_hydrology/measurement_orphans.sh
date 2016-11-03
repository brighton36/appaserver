:
# src_hydrology/measurement_orphans.sh
# ------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 output_filename" 1>&2
	exit 1
fi

station_list=`	cd /opt/html/cgi; station_list.sh everglades hydrology	|\
		tr '\012' ','						|\
		remove_last_character`

echo "	select station, datatype, measurement_date, measurement_time	 \
	from measurement						 \
	where station not in ($station_list)				 \
	order by station, datatype, measurement_date, measurement_time;" |
mysql.sh								 |
tee $1
