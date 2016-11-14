file (GLOB gps_sources sensor_core.c)

set(EXAMPLE_BIN ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/sensor_core CACHE STRING "sensor_core dir")

add_executable(gps_process ${gps_sources})

target_link_libraries(gps_process ublox_gps temp_sensor mpu-9150 cc_beacon_iface)

#file (GLOB beacon_reader_sources beacon_recv.c)

#set(EXAMPLE_BIN ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/beacon_recv CACHE STRING "beacon_recv dir")

#add_executable(beacon_reader ${beacon_reader_sources})

#target_link_libraries(beacon_reader cc_beacon_iface)
