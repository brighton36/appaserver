#!/bin/bash
# ----------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/folder_attribute_exists.sh
# ----------------------------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application folder attribute" 1>&2
	exit 1
fi

application=$1
folder=$2
attribute=$3

results=$(attributes4folder.sh $application $folder | grep "^${attribute}$")

if [ "$results" = "" ]
then
	exit 1
else
	exit 0
fi
