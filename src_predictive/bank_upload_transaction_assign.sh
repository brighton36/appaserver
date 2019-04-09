#!/bin/bash
# -----------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/bank_upload_transaction_assign.sh
# -----------------------------------------------------------------

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

direct_transaction_assign.sh "$1" "$2" "$3" "$4" "$5" "$6"

exit $?

