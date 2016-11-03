:
# shef_alias_check.sh
# -------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 entity" 1>&2
	exit 1
fi

entity=$1
offset=2

shef_alias_to_shef $entity $offset '/'				|
piece.e '/' 2							|
sort -u
