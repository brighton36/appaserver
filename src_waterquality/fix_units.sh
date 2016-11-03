:

where="units = 'mg/M^3'"

echo "update parameter_unit set units = 'mg/M**3' where $where;"	|
cat

echo "update station_parameter set units = 'mg/M**3' where $where;"	|
cat

echo "update results set units = 'mg/M**3' where $where;"		|
cat

echo "update results_exception set units = 'mg/M**3' where $where;"	|
cat

exit 0
