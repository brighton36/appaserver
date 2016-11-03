#!/usr/bin/ksh
# grace_chart_appaserver.sh
# -------------------------

# Environment
# -----------
export GRACE_HOME=/opt/bobcat/grace-5.0.2beta
export PATH=$PATH:$GRACE_HOME/grace/bin
export DISPLAY="crocodile:0"

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 agr_file_specification_wo_extension" 1>&2
	exit 1
fi

file_specification=$1
agr_file=${file_specification}.agr
ps_file=${file_specification}.ps
pdf_file=${file_specification}.pdf

#miny=3.6
#minx=2451545
#maxx=2451715

# Substitute the New Values into the par file from the template

#sed 's/NP205_STAGE/'$last_reading'/'		\
#$paramdir/popA_base_habitat.template		|
#sed 's/X_MIN/'$minx'/'				|
#sed 's/X_MAX/'$maxx'/'				|
#sed 's/Y_MIN/'$miny'/'	>			\
#$paramdir/popA_base_habitat.par

grace3=/opt/bobcat/grace-5.0.3gamma/src/xmgrace

cat $agr_file							|
/usr/local/grace/bin/gracebat 					\
	-noask							\
	-autoscale x 						\
	-hdevice EPS 						\
	-pipe							\
	-hardcopy						\
	-printfile $ps_file 

if [ ! -r $ps_file ]
then
	echo "ERROR in $0: Post Script file not created" 1>&2
	exit 1
fi

cat $ps_file							|
/opt/spoonbill/acrobat/bin/distill 2>/dev/null			|
cat > $pdf_file

echo 0
