:
# appaserver_tz.sh
# ----------------

utc_offset=`echo "	select application_constant_value		\
			from application_constants			\
			where application_constant = 'utc_offset';"	|
			sql.e | sed 's/^-//'`

# Looks like EST5EDT or PST8PDT
# -----------------------------
echo "XXX${utc_offset}XXX"

exit 0
