# sensors core .c
file (GLOB gps_sources sensor_core.c)

set(EXAMPLE_BIN ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/sensor_core CACHE STRING "sensor_core dir")

add_executable(gps_process ${gps_sources})

target_link_libraries(gps_process ublox_gps temp_sensor mpu-9150 cc_beacon_iface)

