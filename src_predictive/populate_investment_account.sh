:
# $APPASERVER_HOME/src_predictive/populate_investment_account.sh
# --------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------------------------

echo $0 $* 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application where" 1>&2
	exit 1
fi

application=$1
parameter_where=$2

export DATABASE=$application

table="investment_account"

# Javascript won't receive [$classification] with this select.
# ------------------------------------------------------------
#select="full_name, street_address, concat( account_number, '|', full_name, '---', street_address, '---', account_number, ' [', classification, ']' )"

select="concat( full_name, '^', street_address, '^', account_number, ' [', classification, ']' )"

if [ "$parameter_where" = "where" -o "$parameter_where" = "" ]
then
	where="1 = 1"
else
	where=$parameter_where
fi

echo "select $select from $table where $where;" | sql.e

exit 0
