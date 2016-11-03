:
# killprocess.sh
# --------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 username process_name really_yn" 1>&2
	exit 1
fi

username=$1
process_name=$2
really_yn=$3

if [ "$really_yn" = "y" ]
then
	kill_process="xargs -t -n1 kill"
else
	kill_process="xargs -t -n1 echo kill"
fi

#if [ "$2" = "reverse" ]
#then
	#sort_command="sort -n -r"
#else
	#sort_command="sort -n"
#fi

sort_command="sort -n"

# Don't add any pipes here because the process shows up.
# ------------------------------------------------------
process_list=`ps -fu $username | grep -v grep | grep $process_name`

echo "$process_list"		|
grep -v PID			|
#tee /dev/tty			|
awk '{print $2}'		|
$sort_command			|
$kill_process

#echo
#echo $0 completed.
exit 0
