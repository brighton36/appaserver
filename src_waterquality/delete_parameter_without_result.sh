:
# src_waterquality/delete_parameter_without_result.sh
# ---------------------------------------------------

# Input
# -----

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)	# May have appended database

results_subquery="parameter.parameter = results.parameter"
alias_subquery="parameter.parameter = parameter_alias.parameter"

statement="
select parameter
from parameter
where not exists ( select 1 from results where $results_subquery )
  and not exists ( select 1 from parameter_alias where $alias_subquery );"

table="parameter"
field="parameter"

echo "$statement"					|
sql_quick.e '^'						|
delete_statement table=$table field=$field del='^'	|
cat

exit 0
