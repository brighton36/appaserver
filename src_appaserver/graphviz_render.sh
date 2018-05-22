:
# src_appaserver/graphviz_render.sh
# ---------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 graphviz_file" 1>&2
	exit 1
fi

graphviz_file=$1
output_filename=graphviz_render_$$.ps

dot -Tps $graphviz_file -o $output_filename

echo "$output_filename"

exit 0
