:
# ps2pdf.sh
# ---------------------------------------------
# This is a wrapper around ps2pdf or distill.
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 5 ]
then
	echo "Usage: $0 /path/ps_file.ps /path/pdf_file.pdf distill_directory \"distill_landscape_flag\" ghost_script_directory" 1>&2
	exit 1
fi

ps_file=$1
pdf_file=$2
distill_directory=$3
distill_landscape_flag=$4
GS_LIB=$5

#which ps2pdf >/dev/null
#got_ps2pdf_return_flag=$?

#if [ $got_ps2pdf_return_flag = 0 ]
#then

if [ ! -f $ps_file ]
then
	echo "File not found error in $0: $ps_file" 1>&2
	exit 1
fi

export GS_LIB
PATH=$GS_LIB:$GS_LIB/obj:$GS_LIB/bin:$PATH;export PATH
cd $GS_LIB/fonts 2>/dev/null
ps2pdf $ps_file $pdf_file
exit $?

#fi

#which distill >/dev/null
#got_distill_return_flag=$?

#if [ $got_distill_return_flag = 0 ]
#then
	#cat $ps_file						|
	#distill $distill_landscape_flag > $pdf_file
	#exit 0
#fi

#if [ "$distill_directory" != "" ]
#then
	#if [ -x ${distill_directory}/distill ]
	#then
		#cat $ps_file						|
		#${distill_directory}/distill $distill_landscape_flag > $pdf_file
		#exit 0
	#fi
#fi

#exit 1
