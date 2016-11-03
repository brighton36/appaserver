#!/bin/sh
# ---------------------------------
# attributes4table.sh
# ---------------------------------
#
# Copyright 1999 By Tim Riley
#
# This is not in the public domain.
# ---------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo \
"Usage: $0 table_name " 1>&2
	exit 1
fi

echo "describe $1;"	|
./mysql.sh		|
./piece '|' 0

