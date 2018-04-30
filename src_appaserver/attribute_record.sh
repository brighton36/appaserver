#!/bin/sh
# ---------------------------------------------
# attribute_record.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must .set_project first." 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -lt 3 ]
then
	echo \
"Usage: $0 ignored attribute dbms [delimiter]" 1>&2
	exit 1
fi

attribute=$2
dbms=$3

if [ "$#" -eq 4 ]
then
	delimiter=$4
else
	delimiter="|"
fi

attribute_record_list.sh $application "" $dbms "$delimiter"		|
grep "^${delimiter}${attribute}${delimiter}"				|
cat

