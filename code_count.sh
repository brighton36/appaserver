:
# --------------------------------------
# $APPASERVER_HOME/utility/code_count.sh
# --------------------------------------
# This file represents 15 lines of code.
# --------------------------------------

/bin/echo -n "File count: "
(
for file in `find . -name '*.c' -o -name '*.h' -o -name makefile -o -name '*.js'`
do
	echo $file
done
) | wc -l | commas_in_long.e

/bin/echo -n "Line count: "
(
for file in `find . -name '*.c' -o -name '*.h' -o -name makefile -o -name '*.js'`
do
	cat $file
done
# ------------------------------
# Skip empty lines and comments.
# ------------------------------
) | grep -v '^$' | grep -v '^\/\*' | wc -l | commas_in_long.e

exit 0
