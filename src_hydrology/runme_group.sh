:
# runme.sh
# --------------------------------------------------------
# This loads CR10 data for stations in the "stations" file
# --------------------------------------------------------

host_name=`uname -n`
application=everglades

if [ "$host_name" = "snook" ]
then
	execute_directory=/opt/html/cgi
	data_directory=/opt/export/enpdata
else
	execute_directory=`get_appaserver_mount_point`/src_$application
	data_directory=`get_appaserver_mount_point`/$application
fi

cd $execute_directory

for file in $(cat $data_directory/stations); do

./load_cr10	$application 				\
		$data_directory 			\
		${file}.DAT 			       	\
		station					\
 	        date_offset_comma_list                	\
		y 					\
		email_address 				\
		y 2>&1					|
sed "s/<p>//g"						|
grep -v "Duplicate"

done
