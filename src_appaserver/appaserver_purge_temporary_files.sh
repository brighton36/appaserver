#!/bin/sh
# -------------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/appaserver_purge_temporary_files.sh
# -------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# -------------------------------------------------------------------

if [ $# -ne 1 ]
then
	echo "Usage: `basename.e $0 n` ignored" 1>&2
	exit 1
fi

# First, purge $document_root/appaserver/*/data/
# ----------------------------------------------
delay_minutes=30
document_root=`get_document_root.e`
appaserver_directory=${document_root}/appaserver

cd $appaserver_directory

find . -type d -name data					|
while read directory
do
	find	$directory					\
		-type f						\
		-name '*_[1-9][0-9][0-9]*'			\
		-mmin +$delay_minutes				\
		-exec rm -f {} \; 2>/dev/null
done

# Second, purge appaserver data temporary files
# ---------------------------------------------
appaserver_data_directory=`get_appaserver_data_directory.e`
delay_minutes=180

find	$appaserver_data_directory				\
	-type f							\
	-name '*_[1-9][0-9][0-9]*'				\
	-mmin +$delay_minutes					\
	-exec rm -f {} \; 2>/dev/null

exit 0
