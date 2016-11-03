:
if [ "$#" -ne 1 ]
then
	year=1997
else
	year=$1
fi

weeks_in_year.py $year 4
months_in_year.py
annuals_in_year.py $year
