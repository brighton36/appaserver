:
entity=everglades

for station in `station_list.sh $entity`
do
	echo $station
	clone_folder	$entity 				\
			primary_shef_datatype 			\
			station 				\
			"$station" 				\
			"station = 'BA'"			\
			y
done
