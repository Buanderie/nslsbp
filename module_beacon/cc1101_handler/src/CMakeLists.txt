file (GLOB_RECURSE cc_beacon_iface *)

add_library(cc_beacon_iface  SHARED  ${cc_beacon_iface})

# From: $cmake --help-property SOVERSION
#		For shared libraries VERSION and SOVERSION can be used to specify
#		the build version and api version respectively.
#
# Edit it as appropriate to be in line with the src/lib_common/of_openfec_api.c::more_about() version
#
set_target_properties(cc_beacon_iface PROPERTIES
			VERSION 0.1
			SOVERSION 1) 

# Feel free to edit as appropriate the "target_link_libraries".
#
# Add the IL (DevIL) library if you want to have the possibility to
# produce H/G matrix images with LDPC codes.
# More precisely, we are relying on the DevIL library
# (http://openil.sourceforge.net/).
# Please install it on your machine before compiling the OpenFEC
# library if needed.
# Otherwise remove the IL library.

target_link_libraries(cc_beacon_iface)
