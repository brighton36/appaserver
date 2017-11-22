:
echo "	select distinct email_address
	from musician, band_member
	where primary_instrument = 'Director'
	  and musician.full_name = band_member.full_name;"	|
sql.e								|
head -1

exit 0
