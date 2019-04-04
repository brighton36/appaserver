:
# $APPASERVER_HOME/src_appaserver/mysqldump_template.sh
# -----------------------------------------------------

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

if [ "$application" != "template" ]
then
	echo "Error in $0: you must connect to the template database." 1>&2
	exit 1
fi

# Display the password for convenience
# ------------------------------------
grep '^password' /etc/appaserver.config

output_directory=`get_appaserver_home.e`/template
output_filename="${output_directory}/mysqldump_template.sql.gz"
input_filename=`mysqldump.sh template y | column.e 1`

insert_appaserver_user="insert into appaserver_user( login_name, password ) values ( 'nobody', 'changeit' );"
insert_appaserver_user_role_system="insert into role_appaserver_user( role, login_name ) values ( 'system', 'nobody' );"
insert_appaserver_user_role_supervisor="insert into role_appaserver_user( role, login_name ) values ( 'supervisor', 'nobody' );"
insert_appaserver_user_role_user="insert into role_appaserver_user( role, login_name ) values ( 'dataentry', 'nobody' );"

(
zcat $input_filename				|
grep -vi 'insert into `appaserver_user`'	|
grep -vi 'insert into `role_appaserver_user`'	|
grep -vi 'insert into `appaserver_sessions`'	;
echo "$insert_appaserver_user"			;
echo "$insert_appaserver_user_role_system"	;
echo "$insert_appaserver_user_role_supervisor"	;
echo "$insert_appaserver_user_role_user"
)						|
gzip > $output_filename

echo "Created $output_filename"

exit 0
