:
# $APPASERVER_HOME/src_predictive/select_tax_form_line_account.sh
# ---------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must first . set_database" 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 ignored tax_form tax_form_line" 1>&2
	exit 1
fi

tax_form=$2
tax_form_line=$3

key=".account "

select="concat( '$key', account )"

from="tax_form_line_account"

where="tax_form = '$tax_form' and tax_form_line = '$tax_form_line'"

echo "select $select from $from where $where;" | sql.e

exit 0

