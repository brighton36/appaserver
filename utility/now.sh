:
# now.sh
# ---------
# Tim Riley

if [ "$#" -eq 0 ]
then
	exit_usage=1
fi

if [ "$1" = "colon" ]
then
	date_format="+%Y-%m-%d %H:%M:%S"
elif [ "$1" = "hhmmss" ]
then
	date_format="+%H%M%S"
elif [ "$1" = "ymdhm" ]
then
	date_format="+%Y%m%d%H%M"
elif [ "$1" = "ymd" ]
then
	date_format="+%Y-%m-%d"
elif [ "$1" = "pretty" ]
then
	date_format="+%m/%d/%Y %H:%M"
elif [ "$1" = "full" ]
then
	date_format="+%B %d, %Y"
else
	exit_usage=1
fi

if [ "$exit_usage" = 1 ]
then
	echo "Usage: $0 ymdhm|ymd|hhmmss|pretty|full|colon [days_from_now]" 1>&2
	exit 1
fi

if [ "$#" -eq 2 ]
then
	from_now=$2
else
	from_now=0
fi

date "$date_format" --date "$from_now days"

exit 0
