:
# echo "Content-type: text/html"
# echo ""

filename="/hydrology/enp_station.kml"

echo "<html>"
echo "<head>"
echo '<link rel=stylesheet type="text/css" href="/hydrology/style.css">'
echo "</head>"
echo "<body>"
echo "<h1>Google Earth Everglades</h1>"
echo "<br>"
echo "<a href=$filename>Press to load file.</a>"
echo "</body>"
echo "</html>"

exit 0
