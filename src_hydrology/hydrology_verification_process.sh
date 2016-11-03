:
# /opt/html/src_hydrology/hydrology_verification_process.sh
# ---------------------------------------------------------

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 entity run_date [output_filename]" 1>&2
	exit 1
fi

entity=$1
run_date=$2

if [ "$#" -eq 2 -o "$3" = "output_filename" ]
then
	tee_process=cat
else
	tee_process="tee $3"
fi

hydrology_verification_process $entity $run_date | $tee_process
