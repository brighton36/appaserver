#You must dot '.' this file.
# -------------------------------------------
# $APPASERVER_HOME/src_predictive/set_self.sh
# -------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

SELF_FULL_NAME="changeme"
SELF_STREET_ADDRESS="1234 Main St."

