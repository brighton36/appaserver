:
# src_waterquality/delete_parameter_unit_without_result.sh
# -----------------------------------------------------------

# Input
# -----

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database

subquery="
parameter_unit.parameter = results.parameter and
parameter_unit.units = results.units"

statement="
select parameter,units
from parameter_unit
where not exists ( select 1 from results where $subquery );"

table="parameter_unit"
field="parameter,units"

echo "$statement"					|
sql_quick.e '^'						|
delete_statement table=$table field=$field del='^'	|
cat

exit 0
