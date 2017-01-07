:
# $APPASERVER_HOME/src_accountancymodel/select_tax_form_category.sh
# -----------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# -----------------------------------------------------------------
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application tax_form" 1>&2
	exit 1
fi

application=$1
tax_form=$2

export DATABASE=$application

key=".tax_form_category "

select="distinct concat( '$key', tax_form_category ), tax_form_line, itemize_accounts_yn"

from="tax_form_category"

where="tax_form = '$tax_form'"

order="tax_form_line"

echo "select $select from $from where $where order by $order;" | sql.e |
while read record
do
	echo $record
	tax_form_category=`echo "$record" | sed "s/$key//" | piece.e '^' 0`

	tax_form_category_accounts.sh	$application		\
					"$tax_form"		\
					"$tax_form_category"
done

exit 0

