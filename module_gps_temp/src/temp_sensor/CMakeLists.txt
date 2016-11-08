file (GLOB_RECURSE temp_source *)

add_library(temp_sensor  SHARED  ${temp_source})

set_target_properties(temp_sensor PROPERTIES
			VERSION 0.1
			SOVERSION 1) 

target_link_libraries(temp_sensor usb m)