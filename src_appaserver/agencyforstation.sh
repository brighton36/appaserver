:
# src_everglades/agencyforstation.sh
# --------------------------------------
#
# Tim Riley
# --------------------------------------

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 station [station...]" 1>&2
	exit 1
fi

application=hydrology
cd /opt/html/cgi

while [ "$#" -gt 0 ]
do
	station=$1
	echo_no_cr.sh "$station|"
	found_it=0

	for entity in everglades other
	do
		table_name=`./get_table_name $entity $application station`

		count=`							 \
			echo "	select count(*)				 \
				from $table_name 			 \
				where station = '$station';"		|\
			mysql.sh`

		if [ "$count" -eq 1 ]
		then
			found_it=1
			break
		fi
	done
	if [ "$found_it" -eq 1 ]
	then
		echo "$entity"
	else
		echo ""
	fi
	shift
done
