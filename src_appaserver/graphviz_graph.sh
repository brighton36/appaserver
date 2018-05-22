#!/bin/bash
# src_appaserver/graphviz_graph.sh
# --------------------------------

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

appaserver_yn=$1

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

	echo "select $primary_select from $from where $primary_where and $join order by $primary_order;"			|
	sql.e					|
	sed 's/align=left/align="left"/g'	|
	sed 's/,0//'				|
	cat

	echo "select $attribute_select from $from where $attribute_where and $join order by $attribute_order;"			|
	sql.e					|
	sed 's/align=left/align="left"/g'	|
	sed 's/,0//'				|
	cat
}
# select_attributes()

function select_flags ()
{
	folder=$1

	from="folder"

	# lookup_before_drop_down_yn
	# --------------------------
	select="'<tr><td colspan=2>lookup_before_drop_down</td></tr>'"
	where="folder = '$folder' and ifnull(lookup_before_drop_down_yn,'n') = 'y'"

	echo "select $select from $from where $where;"	|
	sql.e						|
	sed 's/colspan=2/colspan="2"/'			|
	cat

	# no_initial_capital_yn
	# --------------------------
	select="'<tr><td colspan=2>no_initial_capital</td></tr>'"
	where="folder = '$folder' and ifnull(no_initial_capital_yn,'n') = 'y'"

	echo "select $select from $from where $where;"	|
	sql.e						|
	sed 's/colspan=2/colspan="2"/'			|
	cat

	# populate_drop_down_process
	# --------------------------
	select="concat('<tr><td colspan=2>', populate_drop_down_process, '</td></tr>')"
	where="folder = '$folder' and populate_drop_down_process is not null"

	echo "select $select from $from where $where;"	|
	sql.e						|
	sed 's/colspan=2/colspan="2"/'			|
	cat

	# post_change_process
	# -------------------
	select="concat('<tr><td colspan=2>', post_change_process, '</td></tr>')"
	where="folder = '$folder' and post_change_process is not null"

	echo "select $select from $from where $where;"	|
	sql.e						|
	sed 's/colspan=2/colspan="2"/'			|
	cat

	# post_change_javascript
	# ----------------------
	select="concat('<tr><td colspan=2>', post_change_javascript, '</td></tr>')"
	where="folder = '$folder' and post_change_javascript is not null"

	echo "select $select from $from where $where;"	|
	sql.e						|
	sed 's/colspan=2/colspan="2"/'			|
	cat

	# html_help_file_anchor
	# ----------------------
	select="concat('<tr><td colspan=2>', html_help_file_anchor, '</td></tr>')"
	where="folder = '$folder' and html_help_file_anchor is not null"

	echo "select $select from $from where $where;"	|
	sql.e						|
	sed 's/colspan=2/colspan="2"/'			|
	cat

}
# select_flags()

function select_edges ()
{
	subschema=$1

join="folder.folder = relation.folder"
subschema_where="folder.subschema = '$subschema'"
select="relation.folder,related_folder,related_attribute"
related_folder_select="relation.related_folder"
from="folder,relation"
where="$subschema_where and $join"

echo "select $select from $from where $where;"		|
sql.e							|
while read record
do
	folder=`echo $record | piece.e '^' 0`
	related_folder=`echo $record | piece.e '^' 1`
	related_attribute=`echo $record | piece.e '^' 2`

	/bin/echo -n "$folder -> $related_folder [label="
	/bin/echo -n '"'

	from="relation"

	# pair_1tom_order
	# ---------------
	select="concat( 'pair-', pair_1tom_order )"
	where="folder='$folder' and related_folder='$related_folder' and related_attribute='$related_attribute' and pair_1tom_order is not null"

	echo "select $select from $from where $where;"		|
	sql.e							|
	cat

	# relation_type_isa_yn
	# --------------------
	select="' isa'"
	where="folder='$folder' and related_folder='$related_folder' and related_attribute='$related_attribute' and ifnull(relation_type_isa_yn,'n') = 'y'"

	echo "select $select from $from where $where;"		|
	sql.e							|
	cat

	# prompt_mto1_recursive_yn
	# ------------------------
	select="' recursive'"
	where="folder='$folder' and related_folder='$related_folder' and related_attribute='$related_attribute' and ifnull(prompt_mto1_recursive_yn,'n') = 'y'"

	echo "select $select from $from where $where;"		|
	sql.e							|
	cat

	# copy_common_attributes_yn
	# -------------------------
	select="' copy'"
	where="folder='$folder' and related_folder='$related_folder' and related_attribute='$related_attribute' and ifnull(copy_common_attributes_yn,'n') = 'y'"

	echo "select $select from $from where $where;"		|
	sql.e							|
	cat

	# automatic_preselection_yn
	# -------------------------
	select="' automatic'"
	where="folder='$folder' and related_folder='$related_folder' and related_attribute='$related_attribute' and ifnull(automatic_preselection_yn,'n') = 'y'"

	echo "select $select from $from where $where;"		|
	sql.e							|
	cat

	# drop_down_multi_select_yn
	# -------------------------
	select="' multi'"
	where="folder='$folder' and related_folder='$related_folder' and related_attribute='$related_attribute' and ifnull(drop_down_multi_select_yn,'n') = 'y'"

	echo "select $select from $from where $where;"		|
	sql.e							|
	cat

	# join_1tom_each_row_yn
	# ---------------------
	select="' join'"
	where="folder='$folder' and related_folder='$related_folder' and related_attribute='$related_attribute' and ifnull(join_1tom_each_row_yn,'n') = 'y'"

	echo "select $select from $from where $where;"		|
	sql.e							|
	cat

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

	echo "<tr><td colspan=2><b>${folder}</b></td></tr>"		|
	sed 's/colspan=2/colspan="2"/'

	select_attributes "$folder"
	select_flags "$folder"

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
select_edges $subschema

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
