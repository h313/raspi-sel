add_subdirectory(classify)
add_subdirectory(cpu-stat)
add_subdirectory(record_system)
add_subdirectory(record_ina3221)

add_executable(seelie main.cpp)
target_include_directories(seelie PUBLIC
	record_system
	record_ina3221
	classify
	cpu-stat
)
target_link_libraries(seelie PUBLIC
	record_system
	record_ina3221
	classify
	CPUSnapshot
)
target_link_options(seelie PUBLIC
    -static
)

