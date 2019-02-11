:
# src_capitolpops/insert_program_from_rehearsal_lineup.sh
# -------------------------------------------------------

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -ne 4 ]
then
	echo "Usage: $0 ignore process_name concert_title date" 1>&2
	exit 1
fi

process_name=$2                       	# Assumed letters_and_underbars

process_title=`echo "$process_name" | format_initial_capital.e`

concert_title="$3"
date="$4"

content_type_cgi.sh

echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
echo "<body><h1>$process_title</h1>"

table=concert_program
field="concert_title,date,composition,sort_order"
select="composition,sort_order"

echo "select $select from rehearsal_lineup order by sort_order;"	|
sql.e									|
sed "s/^/${concert_title}^${date}^/"					|
insert_statement.e table=$table field=$field del='^'			|
sql.e 2>&1								|
html_paragraph_wrapper.e

echo "<h3>Process complete.</h3>"
echo "</body></html>"

exit 0
