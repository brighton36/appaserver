:
# src_capitolpops/send_member_list.sh
# -----------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 full_name email_address" 1>&2
	exit 1
fi

full_name=$1
email_address=$2

mailfile="/tmp/mailfile_$$"

echo "To: $email_address" > $mailfile
echo "From: capitolpops@cloudacus.com" >> $mailfile
echo "Subject: Capitol Pops membership list" >> $mailfile
echo "Here is the Capitol Pops membership list:" >> $mailfile
echo "" >> $mailfile

awk 'BEGIN {
	FS = "^"
}
{
	full_name = $1
	primary_instrument = $2
	email_address = $3
	home_phone = $4
	cell_phone = $5

	printf ( "Name: %s\nInstrument: %s\nEmail: %s\nHome: %s\nCell: %s\n",
		full_name,
		primary_instrument,
		email_address,
		home_phone,
		cell_phone )

	printf ( "\n" )

}'					|
cat >> $mailfile

/usr/sbin/sendmail -t < $mailfile
rm -f $mailfile

exit 0
