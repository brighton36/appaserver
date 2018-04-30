:
# $APPASERVER_HOME/src_predictive/populate_investment_account.sh
# --------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_project" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

echo $0 $* 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 ignored where" 1>&2
	exit 1
fi

parameter_where=$2

table="investment_account"

select="concat( full_name, '^', street_address, '^', account_number, ' [', classification, ']' )"

if [ "$parameter_where" = "where" -o "$parameter_where" = "" ]
then
	where="1 = 1"
else
	where=$parameter_where
fi

echo "select $select from $table where $where;" | sql.e

exit 0
