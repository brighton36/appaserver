#!/bin/bash
# src_appaserver/graphviz_database_schema.sh
# ------------------------------------------

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

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 appaserver_yn" 1>&2
	exit 1
fi

appaserver_yn="$1"

function select_attributes ()
{
	folder=$1

	primary_select="concat( '<tr><td align=left>* ', attribute.attribute, '</td><td align=left>', attribute_datatype, ' ', width, ',', ifnull(float_decimal_places,0), '</td></tr>' )"

	attribute_select="concat( '<tr><td align=left>  ', attribute.attribute, '</td><td align=left>', attribute_datatype, ' ', width, ',', ifnull(float_decimal_places,0), '</td></tr>' )"

	from="folder_attribute,attribute"

	join="folder_attribute.attribute = attribute.attribute"

	primary_where="folder_attribute.folder = '$folder' and ifnull(primary_key_index,0) <> 0"

	attribute_where="folder_attribute.folder = '$folder' and ifnull(primary_key_index,0) = 0"

	primary_order="primary_key_index"

	attribute_order="display_order"

	echo "	select $primary_select
		from $from
		where $primary_where and $join
		order by $primary_order;"		|
	sql.e						|
	sed 's/align=left/align="left"/g'		|
	sed "s/,0//"					|
	cat

	echo "	select $attribute_select
		from $from
		where $attribute_where and $join
		order by $attribute_order;"		|
	sql.e						|
	sed 's/align=left/align="left"/g'		|
	sed "s/,0//"					|
	cat
}
# select_attributes()

function select_folder_flags ()
{
	folder=$1

	from="folder"
	where="folder = '$folder'"

	select="	lookup_before_drop_down_yn,
			no_initial_capital_yn,
			populate_drop_down_process,
			post_change_process,
			post_change_javascript,
			html_help_file_anchor"

	record=`echo "select $select from $from where $where;" | sql.e`

	lookup_before_drop_down_yn=`echo $record | piece.e '^' 0`
	no_initial_capital_yn=`echo $record | piece.e '^' 1`
	populate_drop_down_process=`echo $record | piece.e '^' 2`
	post_change_process=`echo $record | piece.e '^' 3`
	post_change_javascript=`echo $record | piece.e '^' 4`
	html_help_file_anchor=`echo $record | piece.e '^' 5`

	if [ "$lookup_before_drop_down_yn" = "y" ]
	then
		echo '<tr><td colspan="2">lookup_before_drop_down</td></tr>'
	fi

	if [ "$no_initial_capital_yn" = "y" ]
	then
		echo '<tr><td colspan="2">no_initial_capital</td></tr>'
	fi

	if [ "$populate_drop_down_process" != "" ]
	then
		/bin/echo -n '<tr><td colspan="2">'
		echo "${populate_drop_down_process}</td></tr>"
	fi

	if [ "$post_change_process" != "" ]
	then
		/bin/echo -n '<tr><td colspan="2">'
		echo "${post_change_process}</td></tr>"
	fi

	if [ "$post_change_javascript" != "" ]
	then
		/bin/echo -n '<tr><td colspan="2">'
		echo "${post_change_javascript}</td></tr>"
	fi

	if [ "$html_help_file_anchor" != "" ]
	then
		/bin/echo -n '<tr><td colspan="2">'
		echo "${html_help_file_anchor}</td></tr>"
	fi

}
# select_folder_flags()

function select_edges ()
{
	subschema="$1"

join="folder.folder = relation.folder"
subschema_where="folder.subschema = '$subschema'"
related_folder_select="relation.related_folder"
from="folder,relation"
where="$subschema_where and $join"

select="	relation.folder,
		related_folder,
		related_attribute,
		pair_1tom_order,
		relation_type_isa_yn,
		prompt_mto1_recursive_yn,
		copy_common_attributes_yn,
		automatic_preselection_yn,
		drop_down_multi_select_yn,
		join_1tom_each_row_yn"

echo "select $select from $from where $where;"		|
sql.e							|
while read record
do
	folder=`echo $record | piece.e '^' 0`
	related_folder=`echo $record | piece.e '^' 1`
	related_attribute=`echo $record | piece.e '^' 2`
	pair_1tom_order=`echo $record | piece.e '^' 3`
	relation_type_isa_yn=`echo $record | piece.e '^' 4`
	prompt_mto1_recursive_yn=`echo $record | piece.e '^' 5`
	copy_common_attributes_yn=`echo $record | piece.e '^' 6`
	automatic_preselection_yn=`echo $record | piece.e '^' 7`
	drop_down_multi_select_yn=`echo $record | piece.e '^' 8`
	join_1tom_each_row_yn=`echo $record | piece.e '^' 9`

	/bin/echo -n "$folder -> $related_folder [label="
	/bin/echo -n '"'

	if [ "$related_attribute" != "null" ]
	then
		/bin/echo -n " $related_attribute"
	fi

	if [ "$pair_1tom_order" != "" -a "$pair_1tom_order" != "0" ]
	then
		/bin/echo -n " pair-${pair_1tom_order}"
	fi

	if [ "$relation_type_isa_yn" = "y" ]
	then
		/bin/echo -n " isa"
	fi

	if [ "$prompt_mto1_recursive_yn" = "y" ]
	then
		/bin/echo -n " recursive"
	fi

	if [ "$copy_common_attributes_yn" = "y" ]
	then
		/bin/echo -n " copy"
	fi

	if [ "$automatic_preselection_yn" = "y" ]
	then
		/bin/echo -n " automatic"
	fi

	if [ "$drop_down_multi_select_yn" = "y" ]
	then
		/bin/echo -n " multi"
	fi

	if [ "$join_1tom_each_row_yn" = "y" ]
	then
		/bin/echo -n " join"
	fi

	/bin/echo '"];'
done

}
# select_edges()

function select_nodes ()
{
	subschema=$1

join="folder.folder = relation.folder"
subschema_where="folder.subschema = '$subschema'"
folder_select="folder.folder"
related_folder_select="relation.related_folder"
from="folder,relation"
where="$subschema_where and $join"

(
echo "select $folder_select from $from where $where;"			|
sql.e									;
echo "select $related_folder_select from $from where $where;"		|
sql.e
)									|
sort -u									|
while read folder
do
	echo "$folder [label=<"
	echo '<table border="0" cellborder="0" cellspacing="0">'

	/bin/echo -n '<tr><td colspan="2"><b>'
	/bin/echo -n "$folder"
	/bin/echo "</b></td></tr>"

	select_attributes "$folder"
	select_folder_flags "$folder"

	echo "</table>>];"
	echo ""
done

}
# select_nodes()

function select_subschema ()
{
	subschema=$1

echo "digraph $subschema {"
echo "node [shape=box];"
echo "rankdir=BT"
echo 'labelloc="t";'
/bin/echo -n 'label="'
/bin/echo -n "$subschema"
/bin/echo '";'

select_nodes "$subschema"
select_edges "$subschema"

echo "}"

}
# select_subschema()

# main
# ----
where="exists( select subschema from folder where folder.subschema = subschemas.subschema and ifnull( appaserver_yn, 'n' ) = '${appaserver_yn}' )"

echo "select subschema from subschemas where $where;"	|
sql.e							|
while read subschema
do
	select_subschema "$subschema"
done

exit 0
