#!/bin/sh
# -------------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/appaserver_purge_temporary_files.sh
# -------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# -------------------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must .set_project first." 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ $# -ne 1 ]
then
	echo "Usage: $0 ignored" 1>&2
	exit 1
fi

document_root=`get_document_root.e`
appaserver_data=`get_appaserver_data_directory.e`

delay_minutes=30
directory=${document_root}/appaserver/${application_name}/data
#directory=${document_root}/appaserver

find	$directory						\
	-name '*_[1-9][0-9][0-9]*'				\
	-mmin +$delay_minutes					\
	-exec rm -f {} \; 2>/dev/null

delay_minutes=180
directory=${appaserver_data}

find	$directory						\
	-name '*_[1-9][0-9][0-9]*'				\
	-mmin +$delay_minutes					\
	-exec rm -f {} \; 2>/dev/null

exit 0
