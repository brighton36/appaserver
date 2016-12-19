#!/bin/sh
# -------------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/appaserver_purge_temporary_files.sh
# -------------------------------------------------------------------
#
# Freely available software: see Appaserver.org
# -------------------------------------------------------------------

if [ $# -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application_name=$1

delay_minutes=30
document_root=`get_document_root.e`
appaserver_data=`get_document_root.e`

directory=${document_root}/${application_name}/appaserver

find	$directory						\
	-xdev							\
	-maxdepth 2						\
	-name '*_[1-9][0-9][0-9][0-9]*'				\
	-cmin +$delay_minutes					\
	-exec rm -f {} \; 2>/dev/null

directory=${appaserver_data}

find	$directory						\
	-xdev							\
	-maxdepth 2						\
	-name '*_[1-9][0-9][0-9][0-9]*'				\
	-cmin +$delay_minutes					\
	-exec rm -f {} \; 2>/dev/null

#if [ -f ./appaserver.config ]
#then
#	appaserver_config="./appaserver.config"
#else
#	appaserver_config="/etc/appaserver.config"
#fi
#
#appaserver_mount_point=`cat $appaserver_config			|
#			grep '^appaserver_mount_point='		|
#			piece.e '=' 1`
#
#find	$appaserver_mount_point					\
#	-xdev							\
#	-maxdepth 2						\
#	-name '*_[1-9][0-9][0-9][0-9]*'				\
#	-cmin +$delay_minutes					\
#	-exec rm -f {} \; 2>/dev/null

exit 0
