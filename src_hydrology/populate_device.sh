:

echo Starting: $0 $* 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application one2m_folder station" 1>&2
	exit 1
fi

application=$1
one2m_folder=$2
station=$3

select="device_type,manufacturer,model,serial_number"

if [ "$one2m_folder" = "site_visit_device_addition" ]
then
	where="current_station is null"
elif [ "$one2m_folder" = "site_visit_device_removal" ]
then
	where="current_station = '$station'"
elif [ "$one2m_folder" = "device_repair" ]
then
	where="current_station is null"
else
	where="1 = 1"
fi

echo "select $select from device where $where order by $select;"	|
sql.e

exit 0
