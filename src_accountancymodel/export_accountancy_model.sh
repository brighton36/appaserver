#!/bin/bash
# -----------------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/export_accountancy_model.sh
# -----------------------------------------------------------------
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
else
	export DATABASE=$application
fi

appaserver_config_file="/etc/appaserver.config"

label="appaserver_mount_point="
appaserver_home=`cat $appaserver_config_file	| \
		 grep "^${label}"		| \
		 sed "s/$label//"`

directory=$appaserver_home/src_accountancymodel
input_file=$directory/accountancy_model_folders.dat
output_shell=$directory/import_accountancy_model.sh

function export_processes()
{
	application=$1
	input_file=$2
	output_shell=$3

	echo "" >> $output_shell
	echo "sql.e << all_done3" >> $output_shell

	# populate_drop_down_process
	# --------------------------
	cat $input_file							|
	while read folder
	do
		where="folder = '$folder'"
		select="populate_drop_down_process"

		process=`echo "select $select
				from folder
				where $where;"				|
			sql.e`

		if [ "$process" = "" ]
		then
			continue
		fi

		where="process = '$process'"
		select="process,command_line"
		echo "select $select from process where $where;"	|
		sql.e							|
		insert_statement.e t=process f="$select" del='^'	|
		cat >> $output_shell
	done

	# post_change_process
	# -------------------
	cat $input_file							|
	while read folder
	do
		where="folder = '$folder'"
		select="post_change_process"

		process=`echo "select $select
				from folder
				where $where;"				|
			sql.e`

		if [ "$process" = "" ]
		then
			continue
		fi

		where="process = '$process'"
		select="process,command_line"
		echo "select $select from process where $where;"	|
		sql.e							|
		insert_statement.e t=process f="$select" del='^'	|
		cat >> $output_shell
	done

	echo "all_done3" >> $output_shell
	echo "" >> $output_shell
}
# export_processes()

function create_accountancy_model()
{
	application=$1
	input_file=$2
	output_shell=$3

	command="create_table $application n x x x $application ttable n mysql"

	cat $input_file							|
	while read table
	do
		new_command=`echo $command | sed "s/ttable/$table/"`
		create_file=`
		$new_command						|
		grep Created						|
		column.e 1						|
		cat`

		cat $create_file					|
		egrep "^echo|^table_name="				|
		grep -v 'drop table'					|
		cat >> $output_shell
	done
}
# create_accountancy_model()

function export_accountancy_model()
{
	application=$1
	input_file=$2
	output_shell=$3

	parameter_list=`
	cat $input_file							|
	count.e 1 							|
	sed 's/ //'							|
	sed 's/)/=/'							|
	sed 's/^00*//'							|
	sed 's/^/folder_/'						|
	joinlines.e '&'							|
	cat`

	export_file=`
	export_subschema $application x x x x "${parameter_list}" 2>/dev/null |
	grep Created							      |
	column.e 1							      |
	cat`

	cat $export_file						|
	sed "s/!= ${application}/= ignored/"				|
	grep -v '^exit'							|
	cat >> $output_shell
}
# export_accountancy_model()

function extract_chart_of_accounts()
{
	application=$1
	output_shell=$2

	echo "" >> $output_shell
	echo "sql.e << all_done2" >> $output_shell

	folder=element
	columns=element,accumulate_debit_yn
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e table=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=subclassification
	columns=subclassification,element,display_order
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=account
	columns=account,subclassification,hard_coded_account_key
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=tax_form_line_account
	columns=tax_form,tax_form_line,account
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=tax_form_line
	columns=tax_form,tax_form_line,tax_form_description,itemize_accounts_yn
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	folder=tax_form
	columns=tax_form
	get_folder_data a=$application f=$folder s=$columns		|
	insert_statement.e t=$folder field=$columns del='^'		|
	cat >> $output_shell

	echo "all_done2" >> $output_shell
	echo "" >> $output_shell
}
# extract_chart_of_accounts()

rm $output_shell 2>/dev/null

export_accountancy_model $application $input_file $output_shell
create_accountancy_model $application $input_file $output_shell
extract_chart_of_accounts $application $output_shell
export_processes $application $input_file $output_shell

echo "exit 0" >> $output_shell

chmod +x $output_shell

echo Created $output_shell

exit 0
