:

echo Starting: $0 $* 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 application one2m_folder state station where" 1>&2
	exit 1
fi

application=$1
one2m_folder=$2
state=$3
station=$4
parameter_where=$5

preprompt_where="1 = 1"

if [ "$parameter_where" != "where" ]
then
	preprompt_where="$parameter_where"
fi

select="device_type,manufacturer,model,serial_number"

one2m_where="1 = 1"

if [ "$state" = "insert" ]
then
	if [ "$one2m_folder" = "site_visit_device_addition" ]
	then
		one2m_where="current_station is null"
	elif [ "$one2m_folder" = "site_visit_device_removal" ]
	then
		one2m_where="current_station = '$station'"
	elif [ "$one2m_folder" = "device_repair" ]
	then
		one2m_where="current_station is null"
	fi
fi

where="$parameter_where and $one2m_where"

echo "select $select from device where $where order by $select;"	|
#tee /dev/tty |
sql.e

exit 0
