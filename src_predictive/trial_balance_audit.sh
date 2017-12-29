:
if [ "$#" != 4 ]
then
	echo "Usage: $0 application begin_date end_date fund" 1>&2
	exit 1
fi

application=$1
begin_date=$2
end_date=$3
fund=$4

time_ticker.e ^ daily $begin_date $end_date "" 2>/dev/null	|
piece.e '^' 0							|
while read as_of_date
do
	input_record=`trial_balance	$application		\
					trial_balance		\
					"$fund"			\
					$as_of_date		\
					aggregation		\
					stdout			|
			tail -1`

	debit=`echo $input_record | piece.e '^' 4`
	credit=`echo $input_record | piece.e '^' 5`

	if [ "$debit" != "$credit" ]
	then
		echo "${as_of_date}^$input_record"
		break
	fi
done

exit 0
