#!/usr/bin/ksh
#
#

echo "Starting: $0 $*" 1>&2

 if [ $# -ne 2 ]; then
  print "Usage: $0: station datatype"
  exit -1
  fi

echo "Content-type: text/html"
echo ""
echo "<pre>"

last_date=$(echo "select max(measurement_date) from measurement where	\
		station='$1' and datatype='$2' ;" |sql )

first_date=$(echo "select min(measurement_date) from measurement where	\
		station='$1' and datatype='$2' ;" |sql )

last_time=$(echo "select max(measurement_time) from measurement where	\
		station='$1' and datatype='$2'and measurement_date='$last_date' ;" |sql )


first_time=$(echo "select min(measurement_time) from measurement where	\
		station='$1' and datatype='$2'and measurement_date='$first_date' ;" |sql )
print $1 $2 ${first_date}:${first_time}  ${last_date}:${last_time}

echo "</pre>"

