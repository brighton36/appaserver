:
# application_folder_list.sh
# --------------------------

echo "Starting: $0 $*" 1>&2

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must .set_project first." 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 ignored system_folders_yn" 1>&2
	exit 1
fi

system_folders_yn=$2

if [ "$system_folders_yn" = "y" ]
then
	appaserver_folder_list $application
else
	temp_file1=/tmp/folder_list1_$$.tmp
	temp_file2=/tmp/folder_list2_$$.tmp
	appaserver_folder_list $application | sort > $temp_file1

	get_folder_data	application=$application			\
			folder=folder					\
			select=folder	 				\
			where="folder <> 'null'"			|
	sort -u								|
	cat > $temp_file2
	join -v 2 $temp_file1 $temp_file2
	rm $temp_file1 $temp_file2
fi

