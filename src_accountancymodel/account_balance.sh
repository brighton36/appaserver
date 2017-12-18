:
# src_accountancymodel/account_balance.sh
# ---------------------------------------------
# Freely available software. See appaserver.org
# ---------------------------------------------

content_type_cgi.sh

echo "$0 $*" 1>&2

if [ "$#" -ne 6 ]
then
	echo "Usage: $0 application process_name as_of_date duration_term institution_full_name institution_street_address" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)

process_name=$(echo $2 | format_initial_capital.e)

as_of_date=$3
if [ "$as_of_date" = "" -o "$as_of_date" = "as_of_date" ]
then
	as_of_date=`date.e 0 | piece.e ':' 0`
fi

duration_term=$4
if [ "$duration_term" = "" -o "$duration_term" = "duration_term" ]
then
	duration_where="1 = 1"
else
	duration_where="duration_term = '$duration_term'"
fi

institution_full_name=$5
street_address=$6
if [ "$institution_full_name" = "" -o "$institution_full_name" = "full_name" ]
then
	institution_where="1 = 1"
else
	institution_where="full_name = '$institution_full_name' and street_address = '$street_address'"
fi

heading="institution_full_name, account_number, As Of Date, balance, Certificate Maturity, Duration"
justification="left, left, left, right, left"

account_process='echo "select full_name, street_address, account_number, certificate_maturity_date, duration_term from investment_account where $duration_where and $institution_where order by full_name, street_address, certificate_maturity_date;" | sql "^"'

(
eval $account_process						|
while read record
do
	institution_full_name=`echo $record | piece.e '^' 0`
	street_address=`echo $record | piece.e '^' 1`
	account_number=`echo $record | piece.e '^' 2`
	certificate_maturity_date=`echo $record | piece.e '^' 3`
	duration_term=`echo $record | piece.e '^' 4`

	select="full_name, account_number, date, balance, '$certificate_maturity_date', '$duration_term'"
	from="account_balance"
	where="full_name = '$institution_full_name' and street_address = '$street_address' and account_number = '$account_number' and date <= '$as_of_date'"
	order="date desc"

	echo "select $select from $from where $where order by $order;"	|
	sql '^'								|
	head -1

done
)									|
piece_sum.e '^' 3							|
piece_commas_in_double.e 3						|
format_initial_capital.e						|
html_table.e "$process_name" "$heading" '^' "$justification"		|
html_wrapper "/appaserver/template/style.css"				|
cat

exit 0
