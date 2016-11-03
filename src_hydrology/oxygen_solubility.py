#!/usr/bin/python
#
# calculate oxygen solubility in umol kg^-1 for given temperature and salinity
#
# Equation from Garcia & Gordon, Oxygen Solubility..., Limn. Ocea. v37, 1992 equation 8, table 1
#
# temp range 	freezing_pt >= t >= 40 C
# sal range	0 >= S >= 42 ppt (papers choice of units)
# deviation +- 10^-3 umol kg^-1
# check value for 10 C and salinity of 35 = 274.647 umol kg^-1
#
import os
import sys
import math
import string


#if len(sys.argv) <> 3:
#	print " Calculation from Garcia & Gordon to calculate oxygen solubility at equilibrium"
#	print " Output in units of mg l^-1 "
#	print " Take a look at comments in this file for details"
#	sys.exit("\nUsage: %s salinity temperature\n"%(sys.argv[0]))
#salinity=float(sys.argv[1])
#temperature=float(sys.argv[2])
#
# coefficients from table 1 here
#
A0=5.80818
A1=3.20684
A2=4.11890
A3=4.93845
A4=1.01567
A5=1.41575
B0=-0.00701211
B1=-0.00725958
B2=-0.00793334
B3=-0.00554491
C0=-0.000000132412
#
# temperature transform relative to 25 C
# note 'log' in python is log[x,[base]] with base defaulting to e if not given

def get_oxygen_solubility( salinity, temperature ):

	TS=math.log((298.15 - temperature)*((273.15 + temperature)**-1))
#
# equation 8
	C0star=math.exp(A0 + (A1*TS) + (A2*(TS**2)) + (A3*(TS**3)) + (A4*(TS**4)) + (A5*(TS**5)) + salinity*(B0 + (B1*TS) + (B2*(TS**2)) + (B3*(TS**3))) + (C0*(salinity**2)))
	C0star=round(C0star,3)
# convert umol kg^-1 to mg l^-1
	C0starmgl=round(((C0star/31.998*(1000+salinity))/1000),3)
	return C0starmgl

# Main
# ----
if ( __name__ == "__main__" ):
	if len( sys.argv ) <> 3:
		sys.exit( "Usage: %s salinity_offset temperature_offset" % sys.argv[ 0 ] )

	salinity_offset = int( sys.argv[ 1 ] )
	temperature_offset = int( sys.argv[ 2 ] )

	while 1:
		try:
			line = raw_input()
		except EOFError:
			break
		substrings = string.split( line )
		salinity = float( substrings[ salinity_offset ] )
		temperature = float( substrings[ temperature_offset ] )
		oxygen_solubility = get_oxygen_solubility(
					salinity, temperature )
		print "%s %.4f" % (line, oxygen_solubility )
