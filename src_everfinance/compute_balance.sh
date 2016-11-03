:

echo "$0 $1" 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 account" 1>&2
	exit 1
fi

account=$1

echo "Content-type: text/html"
echo ""
echo "<html>"
echo "<head>"
echo "<link rel=stylesheet type='text/css' href=/hydrology/style.css>"
echo "</head>"
echo "<body>"
echo "<h1>Balance Report</h1>"

if [ "$account" = "" -o "$account" = "account_name" ]
then
	echo "<h3>Please select an account.</h3>"
	echo "</body>"
	exit 0
fi

appropriation_amount=`echo "select appropriation_amount from account where account_name = '${account}';" | sql.e`
if [ "$appropriation_amount" = "" ]
then
	appropriation_amount="0"
fi

total_expenditures=`echo "select sum( purchase_total ) from purchase where account_name = '${account}';" | sql.e`
if [ "$total_expenditures" = "" ]
then
	total_expenditures="0"
fi

balance=`echo "$appropriation_amount" - "$total_expenditures" | bc`
echo  "<table>"
/usr/bin/printf "<tr><td>The appropriation is:<td align=right>%10.2lf\n" $appropriation_amount
/usr/bin/printf "<tr><td>The expenditures are:<td align=right>%10.2lf\n" $total_expenditures
/usr/bin/printf "<tr><td>The balance is:<td align=right>%10.2lf\n" $balance
echo "</table>"
echo "</body>"
echo "</html>"

exit 0
