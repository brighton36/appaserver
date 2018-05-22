:
# src_appaserver/graphviz_database_schema_process.sh
# --------------------------------------------------

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

# Constants
# ---------
document_root=$(get_document_root.e)
output_directory="$document_root/appaserver/$application/data"
prompt_directory="/appaserver/$application/data"

# Input
# -----

echo "$0" "$*" 1>&2

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 process appaserver_yn" 1>&2
	exit 1
fi

process_name=$1                       	# Assumed letters_and_underbars
appaserver_yn=$2

process_title=`echo "$process_name" | format_initial_capital.e`

content_type_cgi.sh

echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
echo "<body><h1>$process_title</h1>"

# Output
# ------
cd $output_directory
graphviz_file="${process_name}_$$.gv"
graphviz_database_schema.sh "$appaserver_yn" > $graphviz_file
postscript_file=`graphviz_render.sh $graphviz_file`

graphviz_prompt_file=${prompt_directory}/${graphviz_file}
postscript_prompt_file=${prompt_directory}/${postscript_file}

echo "<a href=$postscript_prompt_file>Press to transmit postscript file.</a>"
echo "<br>"
echo "<a href=$graphviz_prompt_file>Press to transmit graphviz file.</a>"
echo "</body></html>"

exit 0
