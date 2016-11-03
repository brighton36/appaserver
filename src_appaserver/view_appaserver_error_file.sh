:

echo "$0 $1 $2 $3" 1>&2

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application directory line_count" 1>&2
	exit 1
fi

application=$1
application=`echo $application | sed 's/:.*$//'`

directory=$2
line_count=$3

if [ "$line_count" = "" -o "$line_count" = "line_count" ]
then
	line_count=50
fi


filename=$directory/appaserver_${application}.err

#if [ ! -r $filename ]
#then
#	filename=$directory/appaserver.err
#fi

echo "Content-type: text/html"
echo ""
echo "<html>"
echo "<head>"
echo "<link rel=stylesheet type='text/css' href=/$application/style.css>"
echo "</head>"
echo "<body>"
echo "<h1>Appaserver Log File</h1>"

echo  "<table>"
tail -$line_count $filename				|
grep -v password					|
sed 's/^/<tr><td>/'

echo "</table>"
echo "</body>"
echo "</html>"

exit 0
