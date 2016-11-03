:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 title" 1>&2
	exit 1
fi

title=$1

cat << all_done
    <script type="text/javascript" src="https://www.google.com/jsapi"></script>
    <script type="text/javascript">
      google.load('visualization', '1', {packages: ['corechart']});
      function drawVisualization()
	{
        var data = google.visualization.arrayToDataTable([
          ['Month', 'PeriodOfRecord', 'Current'],
          ['Jan',  3.5,      2.4],
          ['Feb',  0.3,      0.5],
          ['Mar',  0.2,      0.1],
          ['Apr',  1.0,      1.2],
          ['May',  5.0,      3.3],
          ['Jun',  11.5,     10.2],
          ['Jul',  12.2,     8.3],
          ['Aug',  15.8,     11.0],
          ['Sep',  14.3,     12.8],
          ['Oct',  19.0,     20.2],
          ['Nov',  3.5,      5.4],
          ['Dec',  2.5,      1.8]
        ]);

        var options = {
          title: '$title',
	  width: 800,
	  height: 500,
          vAxis: {title: "Rain/Inches (Sum)"},
          seriesType: "bars",
	  backgroundColor: '#effdff',
	  legend: { position: 'bottom' }
	};


        var chart = new google.visualization.ColumnChart(
		document.getElementById('chart_div'));
        chart.draw(data, options);
      }
      google.setOnLoadCallback(drawVisualization);
    </script>
    <div style="position: absolute;
		left: 265px;
		top: 50px;
		width: 800px;
		border-width: thin;
		border-style: solid;
		border-color: teal" >
    <div id="chart_div"></div>
    </div>
all_done

exit 0
