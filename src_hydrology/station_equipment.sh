#!/usr/bin/ksh
#
# Script to generate a report of equipment at a station
#
#

if [ $# -ne 1 ]; then

	echo ""
	echo "Usage $0: station"
	echo ""
	exit 1

fi

req_station=$1

#Directories
#temp_dir=/opt/physical/data_man/hydrology/equipment/restructure/site_visit_device # Test directory
temp_dir=$2

echo "Content-type: text/html"
echo ""
echo "<pre>"


#echo "SELECT current_station,device_type,manufacturer,model,serial_number FROM device WHERE current_station='$req_station';" | sql

printf "%-15s%-30s%-25s%-25s%-20s%-20s\n" Station Device_Type Manufacturer Model Serial_Number Delta_time

#diff ${temp_dir}/in_equip_${req_station}_${user} ${temp_dir}/out_equip_${req_station}_${user} | grep "<" | tr -d "<" | 

#while read eq_type serial datatype; do

#	echo "SELECT station,visit_date,visit_time,datatype,equipment_type,serial_number		\
#		FROM site_visit_device WHERE serial_number='$serial' AND				\
#		equipment_type='$eq_type' AND datatype='$datatype';" | sql | 				\

echo "SELECT current_station,device_type,manufacturer,model,serial_number,delta_time FROM device	\
	WHERE current_station='$req_station';" | sql |							\

awk 'BEGIN {FS = "^"} {printf("%-15s%-30s%-25s%-25s%-20s%-20s\n", $1,$2,$3,$4,$5,$6)}'

#done

#rm ${temp_dir}/in_equip_${req_station}_${user}
#rm ${temp_dir}/out_equip_${req_station}_${user}

exit 0
