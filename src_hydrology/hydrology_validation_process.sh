#!/bin/ksh
# hydrology_validation_process.sh
# -------------------------------

echo "starting $0 $*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 customer today|yesterday|yyyy-mm-dd" 1>&2
	exit 1
fi

# Skip counts that are zero
# -------------------------
hydrology_validation_process $1 $2 | grep -v "avg:   0/"

