:
# build_inventory_price.sh
# ------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

appaserver_mount_pointer=`get_appaserver_mount_point`
output_file=${appaserver_mount_pointer}/javascript/${application}_inventory_price.js

table_name=`get_table_name $application inventory`

(
echo "var price_array = new Array();"

get_folder_data		application=$application	\
			select=inventory,price		\
			folder=inventory		|
tr '^' '"'						|
sed 's/^.*\"/price_array["&]=/'				|
sed 's/$/;/'						|
sed 's/=;$/=0;/'
) | cat > $output_file

cat << finished
Content-type: text/html

<html>
<head>
	<link rel=stylesheet type="text/css" href=/${application}/style.css>
</head>

<body bgcolor=white>
<h3>Process complete.
</body>
</html>
finished
