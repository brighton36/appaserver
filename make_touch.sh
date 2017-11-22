:
if [ "$APPASERVER_HOME" = "" ]
then
	echo "Error in $0: APPASERVER_HOME not defined" 1>&2
	exit 1
fi

cd $APPASERVER_HOME/library; make touch
cd $APPASERVER_HOME/utility; make touch

cd $APPASERVER_HOME
for directory in `ls -1 -d $APPASERVER_HOME/src_*`
do
	if [ -f $directory/makefile ]
	then
		cd $directory; make touch
	fi
done

if [ -d $APPASERVER_HOME/src_hydrology/reg_sched ]
then
	cd $APPASERVER_HOME/src_hydrology/reg_sched && make touch
fi

exit 0
