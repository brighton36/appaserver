:
if [ "$#" != 3 ]
then
	echo "Usage: $0 application begin_date end_date" 1>&2
	exit 1
fi

application=$1
begin_date=$2
end_date=$3

time_ticker.e ^ daily $begin_date $end_date ""		|
piece.e '^' 0						|
while read as_of_date
do
	/bin/echo -n $as_of_date"^"

	trial_balance	$application		\
			trial_balance		\
			general_fund		\
			$as_of_date		\
			aggregation		\
			stdout			|
	tail -1
done

exit 0
