:
# src_sparrow/populate_site_visit.sh
# ----------------------------------

echo Starting: $0 $* 1>&2

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 ignored from_process_yn where" 1>&2
	exit 1
fi

from_process_yn=$2

if [ "$3" != "" -a "$3" != "where" -a "$3" != "\$where" ]
then
	parameter_where=" and $3"
else
	parameter_where=" and 1 = 1"
fi

if [ "$from_process_yn" = "y" ]
then
	validated_where=" and date_validated is null" 
else
	validated_where=" and 1 = 1"
fi

site_visit=`get_table_name $application site_visit`
observation_site=`get_table_name $application observation_site`

join_where="${site_visit}.quad_sheet = ${observation_site}.quad_sheet and ${site_visit}.site_number = ${observation_site}.site_number"

where="$join_where $validated_where $parameter_where"

select="concat(${site_visit}.quad_sheet,'^',${site_visit}.site_number,'^',visit_date,'|',site_id,'^',survey_year,'^',survey_number)"

from="${site_visit},${observation_site}"

order="${site_visit}.quad_sheet,${site_visit}.site_number,visit_date"

echo "select $select from $from where $where order by $order;"		|
sql.e

exit 0
