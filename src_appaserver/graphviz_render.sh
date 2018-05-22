:
# src_appaserver/graphviz_render.sh
# ---------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 filename_label" 1>&2
	exit 1
fi

filename_label=$1

dot -Tps ${filename_label}.gv -o ${filename_label}.ps

exit 0
