:
# utility/cpio_tape.sh
# --------------------

if [ `whoami` != root ]
then
	echo "root must run this" 1>&2
	exit 1
fi

output_device=/dev/nht0
mt -f $output_device rewind

cd /
find $1 -type f -print  			|
grep -v "/proc/"				|
grep -v "/cdrom/"				|
grep -v "/floppy/"				|
grep -v "/tmp/"					|
sed 's|^/||'					|
cpio -ocv 					|
compress > $output_device
exit 0
