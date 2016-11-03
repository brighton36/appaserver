(echo "	select distinct measurement_date,measurement_time,'aaaa',datatype,measurement_value
from measurement,station,creel_fishing_trips,creel_catches_species
where station.location_area = 2 and 				measurement_date >= '2005-12-01' and				measurement_date <= '2006-02-24' and				datatype = 'salinity'	and				measurement.station = station.station and				measurement.measurement_date = creel_fishing_trips.census_date			 and creel_fishing_trips.login_name = creel_catches_species.login_name and			creel_fishing_trips.census_date = creel_catches_species.census_date and			creel_fishing_trips.census_time = creel_catches_species.census_time and			creel_fishing_trips.interview_location = creel_catches_species.interview_location	and	creel_fishing_trips.interview_number = creel_catches_species.interview_number and		creel_fishing_trips.census_date >= '2005-12-01' and				creel_fishing_trips.census_date <= '2006-02-24' and				creel_fishing_trips.location_area = 2 									  		order by measurement_date,measurement_time,'aaaa',datatype,measurement_value;"					 |
tee /dev/tty |
								  sql.e '^'							 |
								  tr '^' ','							 |
								  real_time2aggregate_value.e average 0 1 4 ',' daily n 2006-02-24								 |
								  sed 's/,null,/,/'						 |
								  sort) |
sort |
sed 's/zzzz,//' |
sed 's/aaaa,//'
