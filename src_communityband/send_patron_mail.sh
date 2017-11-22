:
# src_capitolpops/send_patron_mail.sh
# --------------------------------------

# Input
# -----

from_address=capitolpops@cloudacus.com

echo "$0" "$*" 1>&2

if [ "$#" -lt 4 ]
then
	echo "Usage: $0 application email_address date time [email_send]" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
fi

email_address=$2
date=$3
time=$4

if [ "$#" -eq 5 ]
then
	parameter_email_send=$5
fi

contact=`get_table_name $application contact`
patron=`get_table_name $application patron`
application_constants=`get_table_name $application application_constants`

record=`echo	"select subject,message,full_name,home_phone
			from $contact,$patron
			where $contact.email_address =
				'$email_address'
		  	and date = '$date'
		  	and time = '$time'
			and $patron.email_address =
				$contact.email_address;"	|
		sql.e`

if [ "$record" = "" ]
then
	echo "Error in $0: empty contact record." 1>&2
	exit 1
fi

subject=`echo $record | piece.e '^' 0`
message=`echo $record | piece.e '^' 1`
full_name=`echo $record | piece.e '^' 2`
home_phone=`echo $record | piece.e '^' 3`

message_email_send_override=`echo "	select application_constant_value
					from $application_constants
					where application_constant =
					   'message_email_send_override';" |
		    	     sql.e`

if [ "$parameter_email_send" != "" ]
then
	message_email_send=$parameter_email_send
elif [ "$message_email_send_override" != "" ]
then
	message_email_send=$message_email_send_override
else
	message_email_send=`select_director_email.sh`
fi

if [ "$message_email_send" = "" ]
then
	exit 0
fi

mailfile="/tmp/mailfile_$$"

echo "To: $message_email_send" > $mailfile
echo "From: $from_address" >> $mailfile

if [ "$email_address" != "" -a "$email_address" != "null@null" ]
then
	echo "Reply-To: $email_address" >> $mailfile
fi

echo "Subject: Capitol Pops: $subject" >> $mailfile

if [ "$full_name" != "" ]
then
	echo "Full name: $full_name" >> $mailfile
fi

if [ "$email_address" != "" -a "$email_address" != "null@null" ]
then
	echo "Email: $email_address" >> $mailfile
fi

if [ "$home_phone" != "" ]
then
	echo "Phone: $home_phone" >> $mailfile
fi

echo "$message" >> $mailfile

/usr/sbin/sendmail -t < $mailfile
rm -f $mailfile

exit 0
