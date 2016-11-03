:

output_file=select_station_keys_benchmark.txt

do_benchmark()
{
	dbms=$1

	echo "Number of stations:"
	for station in `echo "select station from station;" | sql ^ $dbms`
	do
		echo "select station from station where station = '$station';" |
		sql.e $dbms
	done | wc -l
}

(
for dbms in mysql oracle mysql oracle
do
	echo "Benchmark $dbms"
	echo "--------------------------"
	time do_benchmark $dbms 2>&1
	echo
done
) 2>&1 | tee $output_file

echo "Created $output_file"

exit 0
