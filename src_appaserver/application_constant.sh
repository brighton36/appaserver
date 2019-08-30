:
# $APPASERVER_HOME/src_appaserver/application_constant.sh
# -------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application_constant" 1>&2
	exit 1
fi

application_constant=$1

select="application_constant_value"
from="application_constants"
where="application_constant = '$application_constant'"

echo "select $select from $from where $where;" | sql.e

exit 0
