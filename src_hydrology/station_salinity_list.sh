#!/bin/bash
# ---------------------------------
# station_salanity_list.sh
# ---------------------------------

select="distinct station"
where="datatype = 'salinity'"
from="station_datatype"
order="station"

echo "select $select from $from where $where order by $order;"	|
sql.e

exit 0
