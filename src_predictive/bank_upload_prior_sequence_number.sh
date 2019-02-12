#!/bin/bash
# --------------------------------------------------------------------
# $APPASERVER_HOME/src_predictive/bank_upload_prior_sequence_number.sh
# --------------------------------------------------------------------

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

echo "$0 $*" 1>&2

if [ "$#" -eq 0 ]
then
	echo "Usage: $0 bank_date" 1>&2
	exit 1
fi

# May have time appended
# ----------------------
bank_date=`echo $1 | column.e 0`

select="max(sequence_number)"
table=bank_upload
where="bank_date < '$bank_date'"

echo "select $select from $table where $where;" | sql.e

exit 0
