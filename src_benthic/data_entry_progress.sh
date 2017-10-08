:
# src_benthic/data_entry_progress.sh
# ----------------------------------

echo "$0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 project" 1>&2
	exit 1
fi

project=$1

# Constants
# ---------
application=benthic
primary_key_delimiter="^"
data_delimiter="|"

# Preprocess
# ----------
sed="s/\\${primary_key_delimiter}/${data_delimiter}/3"

sampling_point_file=/tmp/${application}_sampling_point_$$.dat
quad_file=/tmp/${application}_quad_$$.dat
sweep_file=/tmp/${application}_sweep_$$.dat
sampling_abiotic_file=/tmp/${application}_sampling_abiotic_$$.dat
throwtrap_abiotic_file=/tmp/${application}_throwtrap_abiotic_$$.dat
validated_file=/tmp/${application}_validated_$$.dat
site_quad_join_file=/tmp/${application}_site_quad_join_$$.dat
site_quad_sweep_join_file=/tmp/${application}_site_quad_sweep_join_$$.dat
site_quad_sweep_sampling_abiotic_join_file=/tmp/${application}_site_quad_sweep_sampling_abiotic_join_$$.dat
site_quad_sweep_throwtrap_abiotic_join_file=/tmp/${application}_site_quad_sweep_throwtrap_abiotic_join_$$.dat
site_quad_sweep_abiotic_validated_join_file=/tmp/${application}_site_quad_sweep_abiotic_validated_join_$$.dat

select="collection_number, collection.collection_name, sampling_point.location, count(*)"
group="collection_number, collection.collection_name, sampling_point.location"
order="collection_number desc, sampling_point.location"

# Process
# =======
content_type_cgi.sh

# Create sampling_point_file
# --------------------------
from="sampling_point, collection"
where="collection.project = '$project' and collection.collection_name = sampling_point.collection_name"

echo "select $select from $from where $where group by $group order by $order;"									|
sql.e "$primary_key_delimiter"					|
sed "$sed"							|
cat > $sampling_point_file

# Create quad_file
# ----------------------
join_where="quad_replicate.anchor_date = sampling_point.anchor_date and quad_replicate.anchor_time = sampling_point.anchor_time and quad_replicate.location = sampling_point.location and quad_replicate.site_number = sampling_point.site_number"

from="sampling_point, collection, quad_replicate"
where="collection.project = '$project' and collection.collection_name = sampling_point.collection_name and $join_where"

echo "select $select from $from where $where group by $group order by $order;"									|
sql.e "$primary_key_delimiter"					|
sed "$sed"							|
cat > $quad_file

# Create sweep_file
# -----------------
join_where="sweep.anchor_date = sampling_point.anchor_date and sweep.anchor_time = sampling_point.anchor_time and sweep.location = sampling_point.location and sweep.site_number = sampling_point.site_number"

from="sampling_point, collection, sweep"
where="collection.project = '$project' and collection.collection_name = sampling_point.collection_name and $join_where"

echo "select $select from $from where $where group by $group order by $order;"									|
sql.e "$primary_key_delimiter"					|
sed "$sed"							|
cat > $sweep_file

# Create sampling_abiotic_file
# ----------------------------
join_where="sampling_point_abiotic_measurement.anchor_date = sampling_point.anchor_date and sampling_point_abiotic_measurement.anchor_time = sampling_point.anchor_time and sampling_point_abiotic_measurement.location = sampling_point.location and sampling_point_abiotic_measurement.site_number = sampling_point.site_number"

from="sampling_point, collection, sampling_point_abiotic_measurement"
where="collection.project = '$project' and collection.collection_name = sampling_point.collection_name and $join_where"

echo "select $select from $from where $where group by $group order by $order;"									|
sql.e "$primary_key_delimiter"					|
sed "$sed"							|
cat > $sampling_abiotic_file

# Create throwtrap_abiotic_file
# -----------------------------
join_where="throwtrap_abiotic_measurement.anchor_date = sampling_point.anchor_date and throwtrap_abiotic_measurement.anchor_time = sampling_point.anchor_time and throwtrap_abiotic_measurement.location = sampling_point.location and throwtrap_abiotic_measurement.site_number = sampling_point.site_number"

from="sampling_point, collection, throwtrap_abiotic_measurement"
where="collection.project = '$project' and collection.collection_name = sampling_point.collection_name and $join_where"

echo "select $select from $from where $where group by $group order by $order;"									|
sql.e "$primary_key_delimiter"					|
sed "$sed"							|
cat > $throwtrap_abiotic_file

# Create validated_file
# ---------------------
from="sampling_point, collection"
where="collection.project = '$project' and collection.collection_name = sampling_point.collection_name and validated_date is not null"

echo "select $select from $from where $where group by $group order by $order;"									|
sql.e "$primary_key_delimiter"					|
sed "$sed"							|
cat > $validated_file

# Create site_quad_join_file
# ---------------------------
join.e	$sampling_point_file				\
	$quad_file					\
	"|"						|
awk 'BEGIN {
	FS = "|"
}
{
	key = $1
	site_count = $2
	quad_count = $3
	results = quad_count / site_count
	printf ( "%s|%d|%.1f\n",
		key,
		site_count,
		results )
}'| cat > $site_quad_join_file

# Create site_quad_sweep_join_file
# --------------------------------
join.e	$site_quad_join_file				\
	$sweep_file					\
	"|"						|
awk 'BEGIN {
	FS = "|"
}
{
	key = $1
	site_count = $2
	quad_average = $3
	sweep_count = $4
	results = sweep_count / site_count
	printf ( "%s|%d|%s|%.1f\n",
		key,
		site_count,
		quad_average,
		results )
}'| cat > $site_quad_sweep_join_file

# Create site_quad_sweep_sampling_abiotic_join_file
# -------------------------------------------------
join.e	$site_quad_sweep_join_file		\
	$sampling_abiotic_file			\
	'|' |
awk 'BEGIN {
	FS = "|"
}
{
	key = $1
	site_count = $2
	quad_average = $3
	sweep_average = $4
	abiotic_count = $5
	results = abiotic_count / site_count
	printf ( "%s|%d|%s|%s|%.1f\n",
		key,
		site_count,
		quad_average,
		sweep_average,
		results )
}'| cat > $site_quad_sweep_sampling_abiotic_join_file

# Create site_quad_sweep_throwtrap_abiotic_join_file
# --------------------------------------------------
join.e	$site_quad_sweep_sampling_abiotic_join_file	\
	$throwtrap_abiotic_file				\
	'|'						|
awk 'BEGIN {
	FS = "|"
}
{
	key = $1
	site_count = $2
	quad_average = $3
	sweep_average = $4
	sampling_abiotic_average = $5
	throwtrap_abiotic_count = $6
	results = throwtrap_abiotic_count / site_count
	printf ( "%s|%d|%s|%s|%s|%.1f\n",
		key,
		site_count,
		quad_average,
		sweep_average,
		sampling_abiotic_average,
		results )
}'| cat > $site_quad_sweep_throwtrap_abiotic_join_file

# Create site_quad_sweep_abiotic_validated_join_file
# --------------------------------------------------
join.e	$site_quad_sweep_throwtrap_abiotic_join_file	\
	$validated_file					\
	"|"						|
cat > $site_quad_sweep_abiotic_validated_join_file

# Output
# ------

cat << all_done
<html>
<head>
<link rel=stylesheet type="text/css" href="/appaserver/$application/style.css">
<title>Data Entry Progress</title>
</head>
all_done
 
title="Data Entry Progress"
heading="Collection Number, Collection Name, Location, Site Count, Avg. Quad, Avg. Sweep, Avg. Sampling Abiotic, Avg. Throwtrap Abiotic, Validated"
justification="right,left,left,right,right,right,right,right"

cat $site_quad_sweep_abiotic_validated_join_file		|
sed "s/$data_delimiter/$primary_key_delimiter/g"		|
sed "s/\^$/^0/"							|
format_initial_capital.e					|
html_table.e "$title" "$heading" "$primary_key_delimiter" "$justification" 

cat << all_done
</html>
all_done

rm -f $sampling_point_file
rm -f $quad_file
rm -f $sweep_file
rm -f $abiotic_file
rm -f $validated_file
rm -f $site_quad_join_file
rm -f $site_quad_sweep_join_file
rm -f $site_quad_sweep_abiotic_join_file
rm -f $site_quad_sweep_abiotic_validated_join_file
rm -f $throwtrap_abiotic_file
rm -f $site_quad_sweep_throwtrap_abiotic_join_file
rm -f $site_quad_sweep_sampling_abiotic_join_file
rm -f $sampling_abiotic_file

exit 0
