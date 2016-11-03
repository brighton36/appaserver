:
cat << all_done
<div class=current_vs_historical_chart_class>
<applet code=com.objectplanet.chart.BarChartApplet archive=/hydrology/chart.jar width=800 height=470 VIEWASTEXT id=Applet1>
<param name=chartTitle value="1AAA 1975-01-01 Ending: 2013-05-13">
<param name=chartBackground value="#effdff">
<param name=Background value="#effdff">
<param name=barWidth value="0.8">
<param name=lowerRange value="0">
<param name=seriesCount value=2>
<param name=sampleValues_0 value="0.3,0.2,1.0,5.0,11.5,12.2,15.8,14.3,19.0,3.5,2.5">
<param name=sampleValues_1 value="2.4,0.5,0.1,1.2,3.3,0,0,0,0,0,0,0">
<param name=sampleLabels value="Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec">
<param name=legendLabels value="PeriodOfRecord,Current">
<param name=legendOn value=true>
<param name=legendPosition value=bottom>
<param name=background value=white>
<param name=sampleHighlightOn value="true">
<param name=sampleHighlightStyle value="circle_filled">
<param name=sampleHighlightSize value="0">
<param name=rangeAxisLabel value="Rain (Inches)">
<param name=rangeAxisLabelAngle value=270>
<param name=rangeAxisLabelFont value="Arial,14">
<param name=sampleLabelsOn value=true>
<param name=rangePosition value=left>
<param name=autoLabelSpacingOn value=true>
<param name=barLabelAngle value=0>
<param name=rangeLabelFont value="Arial,14">
<param name=sampleLabelFont value="Arial,14">
<param name=barLabelFont value="Arial,14">
<param name=valueLinesOn value=true>
<param name=valueLabelsOn value=true>
<param name=valueLabelStyle value=inside>
<param name=rangeDecimalCount value=3>
<param name=barLabelsOn value=true>
<param name=multiColorOn value=true>
<param name=sampleColors value="aqua,red,green,gray,blue,olive,maroon,navy,lime,purple,olive,fuchsia,green,yellow,maroon">
</applet>
</div>
all_done

exit 0

