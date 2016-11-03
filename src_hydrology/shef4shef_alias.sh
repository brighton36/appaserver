:
# src_everglades/shef4shef_alias.sh
# -----------------------------------------
#
# -----------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: echo shef_alias | $0 entity" 1>&2
	exit 1
fi

entity=$1
cd /opt/html/cgi

PATH=$PATH:.:/opt/html/utility; export PATH

table_name=`get_table_name $entity shef_alias`

while read alias
do
	echo_no_cr.sh "$alias|"

	shef_code=`						\
		echo "	select shef_code			\
			from $table_name 			\
			where shef_alias = '$alias';" 		|
		sql`

	if [ "$shef_code" != "" ]
	then
		echo "$shef_code"
	else
		echo
	fi
done
