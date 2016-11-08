file (GLOB_RECURSE imu_source *)

add_library(mpu-9150  SHARED  ${imu_source})

set_target_properties(mpu-9150 PROPERTIES
			VERSION 0.1
			SOVERSION 1) 

target_link_libraries(mpu-9150)
