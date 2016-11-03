:
# src_everglades/populate_oracle_measurement_date_time.sh
# -------------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 uid_pwd" 1>&2
	exit 1
fi

uid_pwd=$1

. `get_appaserver_mount_point`/src_appaserver/profile_oracle.sh

commit_after=100
select="station,datatype,to_char(measurement_date,'DD-MON-YYYY'),measurement_time,to_char(measurement_date,'DD-MON-YYYY') || ':' || measurement_time"

primary_keys="station,datatype,measurement_date,measurement_time"
where="measurement_date_time is null and ( measurement_time = 'null' or measurement_time between '0000' and '2359' ) and length( measurement_time ) = 4"
sql_filename=/tmp/populate_measurement_date_time.sql

(
echo "set linesize 9999;"						;
echo "set heading off;"							;
echo "select $select from measurement where $where;"			;
echo "exit;"
)									|
cat > $sql_filename

sqlplus -s $uid_pwd @$sql_filename					|
egrep '(:null|:[0-9][0-9][0-9][0-9])'					|
grep -v '^$'								|
grep -v 'rows selected'							|
sed 's/:null/:0000/'							|
column2pipe.e 5								|
tr '|' '^'								|
sed "s/\^/^measurement_date_time^'/4"					|
sed "s/\^/^to_date(/5"							|
sed "s/$/','DD-MON-YYYY:HH24MI')/"					|
update_statement.e measurement $primary_keys '^^'			|
sed "s/'to_date/to_date/"						|
sed "s/' where station/ where station/"					|
sed "s/'[0-9][0-9]-[A-Z][A-Z][A-Z]-[12][0-9][0-9][0-9]'/to_date(&,'DD-MON-YYYY')/"									|
commit.e $commit_after							|
sql.e '^' oracle							|
cat

echo "delete from measurement where measurement_date_time is null;"	|
sql.e '^' oracle

exit 0
