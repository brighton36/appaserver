:
echo "	select census_date, dayname( census_date )
	from creel_creel_census
	where census_date <> '0000-00-00'
	  and fishing_purpose = 'sport'
	  and interview_location = 'flamingo'
	  and ( dayname( census_date ) = 'Saturday'
	   or   dayname( census_date ) = 'Sunday' )
	order by census_date;"	|
sql.e
