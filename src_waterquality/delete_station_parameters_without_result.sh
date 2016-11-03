:
# src_waterquality/delete_station_parameters_without_results.sh
# -------------------------------------------------------------

# Input
# -----

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database

subquery="
station_parameter.station = results.station and
station_parameter.parameter = results.parameter and
station_parameter.units = results.units"

statement="
select station,parameter,units
from station_parameter
where not exists ( select 1 from results where $subquery );"

table="station_parameter"
field="station,parameter,units"

echo "$statement"					|
sql_quick.e '^'						|
delete_statement table=$table field=$field del='^'	|
cat

exit 0
