cmake_minimum_required( VERSION 3.3 )

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake-utils/Modules )
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/std:c++latest>)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/D_HAS_AUTO_PTR_ETC=1>)

find_package( GBenchmark REQUIRED )
set( BENCHMARK_INCLUDES  ${GBenchmark_INCLUDE_DIRS} )

if(WIN32)
	set( SHLWAPI Shlwapi.lib ) 
else()
	set( SHLWAPI )
endif()

set( BENCHMARK_LIBRARIES
		${SHLWAPI}
		${GBenchmark_LIBRARIES} )


###############################################################################
# Benchmarks
###############################################################################
file( GLOB benchsrc benchmarks/*.cpp )
foreach( file ${benchsrc} )
	get_filename_component( bench_name ${file} NAME_WE )
	set( bench_project_name ${bench_name} )
	
	include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/source ${BENCHMARK_INCLUDES} )
	add_executable( ${bench_project_name} ${file} )
	target_link_libraries( ${bench_project_name} ${BENCHMARK_LIBRARIES} )
	set_target_properties( ${bench_project_name} PROPERTIES FOLDER benchmarks )
endforeach()
