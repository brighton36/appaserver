:
# --------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_database_summarize.sh
# --------------------------------------------------------

spoolfile="/tmp/mysqldump_database_summarize_$$.dat"

# Save the output
# ---------------
cat > $spoolfile

# Prepend summary
# ---------------

echo "-------------------------"
echo "Begin display of warnings"
echo "-------------------------"

cat $spoolfile				|
grep -i warning

echo
echo "-----------------------"
echo "End display of warnings"
echo "-----------------------"

echo
echo "--------------------"
echo "Begin display of log"
echo "--------------------"

cat $spoolfile
rm -f $spoolfile

exit 0
