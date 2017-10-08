#!/bin/sh
# ---------------------------------------------------
# $APPASERVER_HOME/src_appaserver/attributes4table.sh
# ---------------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo \
"Usage: $0 table_name " 1>&2
	exit 1
fi

echo "describe $1;"	|
sql.e '|'		|
piece '|' 0

