:
# src_everglades/shef_for_drew.sh
# ------------------------------------------------------------
#
# This script creates a shef file containing all datatypes
# for the marsh stations for all measurements for the previous
# five days.
#
# Tim Riley
# ------------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 entity" 1>&2
	exit 1
fi

entity=$1

destination_directory=/opt/export/enpdata/code/shef

destination_file=$destination_directory/enp_`date '+%Y%m%d_%H%M'`
file_of_destination_file=$destination_directory/shefilename

cd /opt/html/src_everglades
PATH=$PATH:/opt/html/cgi:/opt/html/utility:/opt/html/bin; export PATH

begin_date=`date.e -10`
end_date=`date.e 0`

(
for station in `marsh_telemeter_station_list.sh $entity`
do
	echo "Exporting station = $station" 1>&2
	get_folder_data							\
		application=$entity 					\
		folder=measurement 				        \
		select="station,datatype,measurement_date,		\
			measurement_time,measurement_value" 		\
		where="station = '$station' and				\
		 datatype <> 'voltage' and				\
		 measurement_date between '$begin_date' and '$end_date'"
done
) 									|

sed 's/\^/|/g' 								|
tee /dev/tty 								|
measurement2shef $entity 24 > $destination_file


echo "$destination_file" > $file_of_destination_file
echo "Created $destination_file" 1>&2
