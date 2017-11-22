:
echo "	select distinct email_address
	from musician, member_position
	where position = 'Website'
	  and musician.full_name = member_position.full_name;"	|
sql.e								|
head -1

exit 0
