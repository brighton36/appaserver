:
# $APPASERVER_HOME/src_predictive/select_tax_form_line_account.sh
# ---------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application tax_form tax_form_line" 1>&2
	exit 1
fi

application=$1
tax_form=$2
tax_form_line=$3

export DATABASE=$application

key=".account "

select="concat( '$key', account )"

from="tax_form_line_account"

where="tax_form = '$tax_form' and tax_form_line = '$tax_form_line'"

echo "select $select from $from where $where;" | sql.e

exit 0

