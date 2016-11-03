:
# $APPASERVER_HOME/src_appaserver/build/create_application_style_sheets.sh
# ------------------------------------------------------------------------

if [ "$APPASERVER_HOME" == "" ]
then
	echo "Error in $0: APPASERVER_HOME is not set." 1>&2
	exit 1
fi

if [ "$DYNARCH_HOME" == "" ]
then
	echo "Error in $0: DYNARCH_HOME is not set." 1>&2
	exit 1
fi

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application source_application" 1>&2
	exit 1
fi

application=$1
source=$2

cd $APPASERVER_HOME

cp $source/style.css $application
cp $source/diagram_list.css $application
cp $source/style_login.css $application
cp $DYNARCH_HOME/src/skin-$source.css \
		$DYNARCH_HOME/src/skin-$application.css
ln $DYNARCH_HOME/src/skin-$application.css \
	   	$DOCUMENT_ROOT/$application
ln $DOCUMENT_ROOT/$application/style.css \
		$DYNARCH_HOME/src/style-$application.css

exit 0

13) Create the application table in Mysql
	a. Login to the creel application
	b. Become role Appaserver System or Administrator
	c. Run Process Clone Folder
		1) Destination Application = Creel
		2) Folder Attribute = Application-Application
		3) Old Data = creel
		4) New Data = $NewProject
		5) Really yn = Yes <-- Test first with No
	d. Run Process Create Table
		1) Destination Application = $NewProject
		2) Folder = Application
		3) Really yn = No <-- We're creating a file only.
		   Note: this creates $APPASERVER_HOME/data/create_table.sh
	e. . set_project.sh $NewProject
	f. less $APPASERVER_HOME/data/create_table.sh
	g. $APPASERVER_HOME/data/create_table.sh $NewProject
14) Insert the application record. 
	a. Login to the creel application
	b. Become role Appaserver System or Administrator
	c. Run Process Export Folder
		1) Destination Application = $NewProject
		2) Folder = Application
		3) Export Output = Shell Script
		   Note: this creates $APPASERVER_HOME/data/
		   		insert_$NewProject_application.sh
	d. . set_project.sh $NewProject
	e. less $APPASERVER_HOME/data/insert_$NewProject_application.sh
	f. $APPASERVER_HOME/data/insert_$NewProject_application.sh
15) Make sure only_one_primary_yn is set to 'y'.
	a. . set_project.sh $NewProject
	b. echo "select only_one_primary_yn
		 from $NewProject_application;" |
	   sql
	c. If the results is not 'y' then set it to 'y'.
16) If you need to create an empty appaserver,
	see create_new_application.txt, step 6), step 7), ...
17) Delete the residual applications.
	a. . set_project.sh creel
	b. echo "delete from creel_application
		 where application <> 'creel';" |
	   sql
	c. . set_project.sh $NewProject
	d. echo "delete from $NewProject_application
		 where application <> '$NewProject';" |
	   sql
18) Create the Appaserver tables.
	a. cd /tmp
	b. ftp_get.sh timriley.net pub/timriley \
		create_$NewProject_appaserver.sh anonymous whatever
	c. . set_project.sh $NewProject
	d. echo "show tables;" | sql <-- should have $NewProject_application.
	e. less create_$NewProject_appaserver.sh
	f. create_$NewProject_appaserver.sh $NewProject
	g. echo "show tables;" | sql <-- should have Appaserver tables.
19) Create the application tables.
	a. cd /tmp
	b. ftp_get.sh timriley.net pub/timriley \
		create_$NewProject.sh anonymous whatever
	c. . set_project.sh $NewProject
	d. echo "show tables;" | sql <-- should have Appaserver tables.
	e. less create_$NewProject.sh
	f. create_$NewProject.sh $NewProject
	g. echo "show tables;" | sql <-- should have application tables.
20) Insert into the Appaserver tables.
	a. cd /tmp
	b. ftp_get.sh timriley.net pub/timriley \
		insert_$NewProject_appaserver.sh.gz anonymous whatever
	c. . set_project.sh $NewProject
	d. gunzip -f insert_$NewProject_appaserver.sh.gz
	e. less insert_$NewProject_appaserver.sh
	f. insert_$NewProject_appaserver.sh $NewProject
	g. echo "select count(*) from folder;" | sql
21) Insert into the application tables.
	a. cd /tmp
	b. ftp_get.sh timriley.net pub/timriley \
		insert_$NewProject.sh.gz anonymous whatever
	c. . set_project.sh $NewProject
	d. gunzip -f insert_$NewProject.sh.gz
	e. less insert_$NewProject.sh
	f. insert_$NewProject.sh $NewProject
22) Reset the passwords.
	a. . set_project.sh $NewProject
	b. echo "update appaserver_user set password = 'changeit';" | sql
23) Log in.
	a. Point your browser to http://mangrove/$NewProject
	b. Log in as either janice or kevin with password of changeit.
24) Set the application record.
	a. Become role Appaserver System
	b. Lookup: Application
	c. Change Application Title and Relative Source Directory
25) Reset each process' execution count.
	a. . set_project.sh $NewProject
	b. echo "update process set execution_count = 0;" | sql
26) Documentation.
	a. This file should be able to be viewed in the View Documentation List.
