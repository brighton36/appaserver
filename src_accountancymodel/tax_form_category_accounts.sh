:
# $APPASERVER_HOME/src_accountancymodel/select_tax_form_category_accounts.sh
# --------------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# --------------------------------------------------------------------------
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application tax_form tax_form_category" 1>&2
	exit 1
fi

application=$1
tax_form=$2
tax_form_category=$3

export DATABASE=$application

key=".account "

select="concat( '$key', account )"

from="tax_form_category"

where="tax_form = '$tax_form' and tax_form_category = '$tax_form_category'"

echo "select $select from $from where $where;" | sql.e

exit 0

