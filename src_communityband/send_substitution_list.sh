:
# src_communityband/send_substitution_list.sh
# -------------------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

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
echo "Subject: Capitol Pops substitution list" >> $mailfile
echo "Here is you instrument's substitution list:" >> $mailfile
echo "" >> $mailfile

awk 'BEGIN {
	FS = "^"
}
{
	email_address = $1
	full_name = $2
	home_phone = $3
	cell_phone = $4
	work_phone = $5
	substitute_concert_only_yn = $6

	printf ( "Name: %s\nEmail: %s\nHome: %s\nCell: %s\nWork: %s\n",
		full_name,
		email_address,
		home_phone,
		cell_phone,
		work_phone )

	if ( substitute_concert_only_yn == "y" )
	{
		printf ( "*** Substitute Concerts Only ***\n" )
	}

	printf ( "\n" )

}'					|
cat >> $mailfile

echo "" >> $mailfile
echo "If you deem anyone should be removed" >> $mailfile
echo "from our substitution list, please contact" >> $mailfile
echo "capitolpops@cloudacus.com ." >> $mailfile

/usr/sbin/sendmail -t < $mailfile
rm -f $mailfile

exit 0
