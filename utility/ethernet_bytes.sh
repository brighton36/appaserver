:
# utility/ethernet_bytes.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 ethernet_interface" 1>&2
	exit 1
fi

interface=$1

results=`/sbin/ifconfig "$interface"	|
	 joinlines.e ';'		|
	 cat`

line2=`	echo "$results"			| 
	piece.e ';' 1`

ip_address=`	echo $line2		| 
		column.e 1		|
		piece.e ':' 1`

line8=`	echo "$results"			| 
	piece.e ';' 7`

receive_bytes=`	echo "$line8"		| 
		column.e 1		|
		piece.e ':' 1`

transmit_bytes=`echo "$line8"		| 
		column.e 5		|
		piece.e ':' 1`

echo $ip_address $receive_bytes $transmit_bytes

exit 0
