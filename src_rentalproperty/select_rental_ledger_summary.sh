:
# src_accountancymodel/select_rental_ledger_summary.sh
# ----------------------------------------------------

echo "$0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 tax_year" 1>&2
	exit 1
fi

tax_year=$1

debit_select="sum(debit_amount)"
credit_select="sum(credit_amount)"

join_transaction="journal_ledger.full_name = transaction.full_name and journal_ledger.street_address = transaction.street_address and journal_ledger.transaction_date_time = transaction.transaction_date_time" 

join_account="journal_ledger.account = account.account"

join_subclassification="account.subclassification = subclassification.subclassification"

join_element="subclassification.element = element.element"

debit_where="accumulate_debit_yn = 'y'"
credit_where="accumulate_debit_yn <> 'y'"

transaction_where="journal_ledger.transaction_date_time like '${tax_year}-%'"

street_address_where="transaction.rental_property_street_address is not null"

group_by="transaction.rental_property_street_address,journal_ledger.account"

from="journal_ledger,transaction,account,subclassification,element"

# Debit accounts (expense)
# ------------------------
select="${group_by},${debit_select}"

element_where="element.element = 'expense'"

where="${transaction_where} and ${join_transaction} and ${join_account} and ${join_subclassification} and ${join_element} and ${debit_where} and ${element_where} and ${street_address_where}"

echo "select ${select} from ${from} where ${where} group by ${group_by}" |
sql.e

# Credit accounts (revenue)
# -------------------------
select="${group_by},${credit_select}"

element_where="element.element = 'revenue'"

where="${transaction_where} and ${join_transaction} and ${join_account} and ${join_subclassification} and ${join_element} and ${credit_where} and ${element_where} and ${street_address_where}"

echo "select ${select} from ${from} where ${where} group by ${group_by}" |
sql.e

exit 0
