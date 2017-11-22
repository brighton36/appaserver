:
# src_capitolpops/send_patron_calendar_message.sh
# -----------------------------------------------

# Input
# -----

application=capitolpops

echo "$0" "$*" 1>&2

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 concert_title date process_name execute_yn" 1>&2
	exit 1
fi

concert_title=$1
date=$2
process_name=$3
execute_yn=$4

process_title=$(echo $process_name | format_initial_capital.e)

mailfile="/tmp/mailfile_$$"
export DATABASE=capitolpops

calendar_message=`echo "	select calendar_message
				from concert
				where concert_title = '$concert_title'
				  and date = '$date';" | sql.e`

if [ "$calendar_message" = "" ]
then
	echo "Error: empty calendar_message." 1>&2
	exit 1
fi

execute ()
{
	for email_address in `echo "	select email_address
					from patron
					where email_address not in (
						select email_address
						from opt_out );" | sql.e`
	do
		echo "To: $email_address" > $mailfile
		echo "From: timriley@cloudacus.com" >> $mailfile
		echo "Subject: Upcoming Capitol Pops concert" >> $mailfile
		echo "Dear Capitol Pops patron," >> $mailfile
		echo "" >> $mailfile
		echo "$calendar_message" >> $mailfile
		echo "" >> $mailfile
		echo "--" >> $mailfile
		echo "Tim Riley, Public Relations" >> $mailfile
		echo "" >> $mailfile
		echo "P.S. I hope to see you there." >> $mailfile
	
		/usr/sbin/sendmail -t < $mailfile
	done
	
	rm -f $mailfile
}

test_only ()
{
	echo "<table>"
	echo "<tr><td>From: timriley@cloudacus.com"
	echo "<tr><td>Subject: Upcoming Capitol Pops concert"
	echo "<tr><td>Dear Capitol Pops patron,"
	echo "<tr>"
	echo "<tr><td>$calendar_message"
	echo "<tr>"
	echo "<tr><td>--"
	echo "<tr><td>Tim Riley, Public Relations"
	echo "<tr>"
	echo "<tr><td>P.S. I hope to see you there."
	echo "</table>"

	echo "<table>"
	echo "<tr>"

	for email_address in `echo "	select email_address
					from patron
					where email_address not in (
						select email_address
						from opt_out );" | sql.e`
	do
		echo "<tr><td>To: $email_address"
	done
	echo "</table>"
}

content_type_cgi.sh

echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
echo "<body><h1>$process_title</h1>"

if [ "$execute_yn" = "y" ]
then
	execute
	echo "<h3>Process complete.</h3>"
else
	test_only
	echo "<h3>Testing complete.</h>"
fi

echo "</body>"
echo "</html>"

exit 0
