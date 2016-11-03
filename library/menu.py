#!/usr/bin/python
#---------------------------
# menu.py
#
# The class menu defines operations to allow users to choose from a list.
#
# Tim Riley
# 2/99
#------------------------------------------------------------------------

import sys, os, string, timlib, myarray, tmpfile

class menu:
	# Instantiation section
	# ---------------------
	def __init__( self ):
		self.item_string_array = myarray.myarray()
		self.choice_string = ''

		self.choose_tmpfile = tmpfile.tmpfile( 'choose' )
		self.result_tmpfile = tmpfile.tmpfile( 'result' )

	# Set section
	# -----------
	def append( self, item ):
		self.item_string_array.append( item )

	# Process section
	# ---------------
	def cleanup( self ):
		self.choose_tmpfile.remove()
		self.result_tmpfile.remove()

	def interact( self ):

		if ( self.item_string_array.len() < 2 ):
			msg = \
		"Warning: menu.interact only has the following set: (%s)" % \
				self.item_string_array.display()
			timlib.write_stderr( msg )

		self.item_string_array.to_file( self.choose_tmpfile.display() )
		s = "choose_one.py inputfile=%s resultfile=%s" % \
			( self.choose_tmpfile.display(), 
			  self.result_tmpfile.display() )

		os.system( s )
		self.choice_string = timlib.get_from_file_string(
						self.result_tmpfile.display() )

	# Get section
	# -----------
	def get( self ): return self.get_choice_string()

	def get_choice_string( self ):
		return self.choice_string

	def __getitem__( self, i ):
		return self.item_string_array[ i ]

	def __len__( self ):
		return self.item_string_array.len()

	def display( self ):
		return self.item_string_array.display()

	def len( self ):
		return self.__len__()

	def is_empty( self ):
		if ( self.len() == 0 ):
			return 1
		else:
			return 0

# Main
# ----
if ( __name__ == "__main__" ):
	m = menu()
	m.append( 'one' )
	m.append( 'two' )
	m.append( 'three' )
	m.interact()
	print "got choice = %s" % m.get_choice_string()
	m.cleanup()
