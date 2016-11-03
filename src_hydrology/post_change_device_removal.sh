#!/bin/bash
# ---------------------------------------------
# src_hydrology/post_change_device_removal.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 5 ]
then
       echo "Usage: $0 state device_type manufacturer model serial_number" 1>&2
	exit 1
fi

state=$1
device_type=$2
manufacturer=$3
model=$4
serial_number=$5

if [ "$state" != "insert" ]
then
	exit 0
fi

key="device_type,manufacturer,model,serial_number"
row="$device_type^$manufacturer^$model^$serial_number^current_station^"

echo "$row"							|
update_statement.e table=device key=$key carrot=y		|
sql.e 2>&1							|
cat


exit 0
