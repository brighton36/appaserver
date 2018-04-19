:
# src_vegetation/peruse_photos.sh
# -------------------------------

# Input
# -----
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application process_name" 1>&2
	exit 1
fi

echo $0 $* 1>&2

application=$(echo $1 | piece.e ':' 0)	# May have appended database
process_name=$2                       	# Assumed letters_and_underbars

process_title=$(echo $process_name | format_initial_capital.e)
peruse_link="/appaserver/$application/photos"

echo "<html><head><link rel=stylesheet type=text/css href=/appaserver/$application/style.css></head>"
echo "<body><h1>$process_title</h1>"

echo "<a href=$peruse_link target=_new>Press to peruse photos.</a>"
echo "</body></html>"

exit 0

