file (GLOB example_sources_rx example_rx.c)

set(EXAMPLE_BIN ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/example CACHE STRING "example_rx dir")

add_executable(example_rx ${example_sources_rx})

target_link_libraries(example_rx cc_beacon_iface)


file (GLOB example_sources_tx example_tx.c)

set(EXAMPLE_BIN ${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/example CACHE STRING "example_tx dir")

add_executable(example_tx ${example_sources_tx})

target_link_libraries(example_tx cc_beacon_iface)