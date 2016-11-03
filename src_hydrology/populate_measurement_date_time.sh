:
# src_everglades/populate_measurement_date_time.sh
# ------------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 uid_pwd" 1>&2
	exit 1
fi

uid_pwd=$1

fetch_at_a_time=10000
commit_after=100

select="station,datatype,to_char(measurement_date,'DD-MON-YYYY'),measurement_time,to_char(measurement_date,'DD-MON-YYYY') || ':' || measurement_time"
primary_keys="station,datatype,measurement_date,measurement_time"
where="measurement_date_time is null and rownum < $fetch_at_a_time"

sql_filename=/tmp/populate_measurement_date_time.sql

(
echo "set linesize 9999;"						;
echo "set heading off;"							;
echo "select $select from measurement where $where;"			;
echo "exit;"
)									|
cat > $sql_filename

sqlplus -s $uid_pwd @$sql_filename					|
grep -v '^$'								|
gre -v 'rows selected'							|
sed 's/:null/:0000/'							|
column2pipe.e 5								|
tr '|' '^'								|
sed "s/\^/^measurement_date_time^'/4"					|
sed "s/\^/^to_date(/5"							|
sed "s/$/','DD-MON-YYYY:HH24MI')/"					|
update_statement.e measurement $primary_keys '^^'			|
sed "s/'to_date/to_date/"						|
sed "s/' where station/ where station/"					|
commit.e $commit_after							|
sql.e '^' oracle							|
sed 's/ORA-01858.*/No empty measurement_date_time records/'		|
cat

exit 0
