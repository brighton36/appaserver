:
# /opt/html/src/hydrology/interpolate_stage_spike.sh
# --------------------------------------------------

if [ "$#" -lt 4 ]
then
	echo "Usage: $0 station start_date end_date [tee_update_filename]" 1>&2
	exit 1
fi

# change directory
# ----------------
cd /opt/html/src/hydrology

station=$1
start_date=$2
end_date=$3

if [ "$#" -eq 4 ]
then
	tee_update_filename=$4
else
	tee_update_filename=/dev/tty
fi

interpolate_stage_spike_extract.sh	$station 		\
					$start_date 		\
					$end_date 		|
interpolate_stage_spike_update.sh	$station 		\
					$start_date 		\
					$end_date 		\
					$tee_update_filename
