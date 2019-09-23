:
# $APPASERVER_HOME/src_predictive/post_change_bank_upload_transaction.sh
# ----------------------------------------------------------------------
# Freely available software. See appaserver.org
# ----------------------------------------------------------------------

echo "$0 $*" 1>&2

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

if [ "$#" -ne 6 ]
then
	echo "Usage: $0 state bank_date bank_description full_name street_address transaction_date_time" 1>&2
	exit 1
fi

state=$1
bank_date=$2
bank_description=$3
full_name=$4
transaction_date_time=$5

bank_upload_sequence_propagate.sh $bank_date | sql.e
bank_upload_balance_propagate.sh $bank_date | sql.e

exit 0
