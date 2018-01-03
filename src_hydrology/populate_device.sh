:

echo Starting: $0 $* 1>&2

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 application one2m_folder station where" 1>&2
	exit 1
fi

application=$1
one2m_folder=$2
station=$3
parameter_where=$4

if [ "$parameter_where" = "where" ]
then
	preprompt_where="1 = 1"
else
	preprompt_where="$parameter_where"
fi

select="device_type,manufacturer,model,serial_number"

if [ "$one2m_folder" = "site_visit_device_addition" ]
then
	one2m_where="current_station is null"
elif [ "$one2m_folder" = "site_visit_device_removal" ]
then
	one2m_where="current_station = '$station'"
elif [ "$one2m_folder" = "device_repair" ]
then
	one2m_where="current_station is null"
else
	one2m_where="1 = 1"
fi

where="$parameter_where and $one2m_where"

echo "select $select from device where $where order by $select;"	|
sql.e

exit 0
