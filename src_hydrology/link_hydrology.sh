:
destination_directory=../cgi

for file in 							\
	agencyforstation.sh					\
	interpolate_spike_update.sh				\
	interpolate_spike_extract.sh				\
	datatype4shef.sh					\
	datatype_list.sh					\
	datatype_name_list.sh					\
	expected_count_list.sh					\
	marine_telemeter_station_list.sh			\
	marsh_marine_telemeter_station_list.sh			\
	marsh_telemeter_station_list.sh				\
	realdata_load_station.sh				\
	shef_for_drew.sh					\
	shef_list.sh						\
	shef_upload_list.sh					\
	station4alias.sh					\
	station_alias_list.sh					\
	station_datatype_list.sh				\
	station_list.sh						\
	station_list_all.sh					\
	telemeter_station_list.sh				\
	telemetry_station_list.sh
do
	rm -f $destination_directory/$file 2>/dev/null
	ln $file $destination_directory/$file
done
