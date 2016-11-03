:
# $APPASERVER_HOME/src_appaserver/build/create_application_links.sh
# -----------------------------------------------------------------

if [ "$APPASERVER_HOME" == "" ]
then
	echo "Error in $0: APPASERVER_HOME is not set." 1>&2
	exit 1
fi

if [ "$DOCUMENT_ROOT" == "" ]
then
	echo "Error in $0: DOCUMENT_ROOT is not set." 1>&2
	exit 1
fi

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

cd $APPASERVER_HOME
ln -s $APPASERVER_HOME/$application $DOCUMENT_ROOT
ln -s $APPASERVER_HOME/src_$application $DOCUMENT_ROOT

exit 0

