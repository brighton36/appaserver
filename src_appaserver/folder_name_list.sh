:
# -------------------
# folder_name_list.sh
# -------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 entity [appaserver_yn]" 1>&2
	exit 1
fi

entity=$1

if [ "$#" -eq 2 ]
then
	appaserver_yn="$2"
else
	appaserver_yn="n"
fi

folder_attribute=$(get_table_name $entity folder_attribute)
folder_table=$(get_table_name $entity folder)

if [ "$appaserver_yn" = "y" ]
then
	appaserver_folder_list $entity
else
	echo "	select $folder_table.folder				\
		from $folder_attribute,					\
		     $folder_table					\
		where $folder_table.folder = $folder_attribute.folder	\
		  and ifnull( appaserver_yn, 'n' ) <> 'y';"		|
	sql.e								|
	sort -u
fi
