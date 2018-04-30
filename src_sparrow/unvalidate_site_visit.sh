:
# src_sparrow/unvalidate_site_visit.sh
# ------------------------------------

# Not used.
# ---------
exit 0

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

echo Starting: $0 $* 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 ignored quad_sheet site_number visit_date state" 1>&2
	exit 1
fi

quad_sheet=$2
site_number=$3
visit_date=$4
state=$5

if [ "$state" != "state" -a "$state" != "update" ]
then
	exit 0
fi

table=$(get_table_name $application site_visit)
set="person_validating = null,date_validated = null, validated_yn = null"
where="quad_sheet='$quad_sheet' and site_number=$site_number and visit_date='$visit_date'"

echo "update $table set $set where $where;" | sql.e

exit 0
