:
# src_timriley/account_balance.sh
# ---------------------------------------------
# Freely available software. See appaserver.org
# ---------------------------------------------

content_type_cgi.sh

echo "$0 $*" 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 application process_name as_of_date account_duration institution" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)

process_name=$(echo $2 | format_initial_capital.e)

as_of_date=$3
if [ "$as_of_date" = "" -o "$as_of_date" = "as_of_date" ]
then
	as_of_date=`date.e 0 | piece.e ':' 0`
fi

account_duration=$4
if [ "$account_duration" = "" -o "$account_duration" = "account_duration" ]
then
	account_duration_where="1 = 1"
else
	account_duration_where="account_duration = '$account_duration'"
fi

institution=$5
if [ "$institution" = "" -o "$institution" = "institution" ]
then
	institution_where="1 = 1"
else
	institution_where="institution = '$institution'"
fi

heading="institution, account_number, As Of Date, balance, Certificate Maturity, Duration"
justification="left, left, left, right, left"

account_process='echo "select institution, account_number, certificate_maturity_date, account_duration from account where $account_duration_where and $institution_where order by institution, certificate_maturity_date;" | sql "^"'

(
eval $account_process						|
while read record
do
	institution=`echo $record | piece.e '^' 0`
	account_number=`echo $record | piece.e '^' 1`
	certificate_maturity_date=`echo $record | piece.e '^' 2`
	account_duration=`echo $record | piece.e '^' 3`

	select="institution, account_number, date, balance, '$certificate_maturity_date', '$account_duration'"
	from="account_balance"
	where="institution = '$institution' and account_number = '$account_number' and date <= '$as_of_date'"
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
html_wrapper "/${application}/style.css"				|
cat

exit 0
