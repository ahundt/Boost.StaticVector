# Set project's name
project(StaticVector)
cmake_minimum_required(VERSION 2.6)


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")


# ---------- Boost Dependencies -------------------------

set(USE_INCLUDED_Boost_DEFAULT ON)
if("${CMAKE_SYSTEM}" MATCHES "Darwin")
set(USE_INCLUDED_Boost_DEFAULT OFF)
endif()

option(USE_INCLUDED_Boost "Use the Boost package included in the repository" ${USE_INCLUDED_Boost_DEFAULT})
if(USE_INCLUDED_Boost)
if (WIN32)
 set (BOOST_INCLUDEDIR ${CMAKE_SOURCE_DIR}/external/install/boost/include/boost-1_49)
 set (BOOST_LIBRARYDIR ${CMAKE_SOURCE_DIR}/external/install/boost/lib)
 set (Boost_USE_STATIC_RUNTIME OFF)
 set (Boost_USE_STATIC_LIBS ON)
endif(WIN32)
set(Boost_DIR "${CMAKE_SOURCE_DIR}/external/install/boost")
# Boost_DIR is the most correct variable to set, following the cmake standardized variable names.
# However, some odd behavior has been spotted, so BOOST_ROOT and BOOSTROOT are set here to be extra confident it will look in the right place.
set(BOOST_ROOT "${Boost_DIR}")
set(BOOSTROOT "${BOOST_ROOT}")
endif(USE_INCLUDED_Boost)

find_package(Boost 1.49 COMPONENTS thread unit_test_framework program_options system date_time filesystem serialization chrono timer REQUIRED)
link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )

# Setup and build boost::shared_ptr debug hooks (off by default)
option(BOOST_SHARED_PTR_ENABLE_DEBUG_HOOKS "Enable Boost::shared_ptr debug hooks for finding circular references")
if(BOOST_SHARED_PTR_ENABLE_DEBUG_HOOKS)
# If the debug hooks are enabled, find the necessary file and build the library
add_definitions(-DBOOST_SP_ENABLE_DEBUG_HOOKS)
find_path(BOOST_SHARED_PTR_ENABLE_DEBUG_HOOKS_PATH sp_collector.cpp HINTS ${CMAKE_SOURCE_DIR}/external/boost/libs/smart_ptr/src ${Boost_DIR})
if(BOOST_SHARED_PTR_ENABLE_DEBUG_HOOKS_PATH)
add_library(BoostSPDebugHooks ${BOOST_SHARED_PTR_ENABLE_DEBUG_HOOKS_PATH}/sp_collector.cpp ${BOOST_SHARED_PTR_ENABLE_DEBUG_HOOKS_PATH}/sp_debug_hooks.cpp )
list(APPEND Boost_LIBRARIES BoostSPDebugHooks)
else(BOOST_SHARED_PTR_ENABLE_DEBUG_HOOKS_PATH)
message(STATUS "Could not find boost::shared_ptr debug hooks, BOOST_SHARED_PTR_ENABLE_DEBUG_HOOKS_PATH: ${BOOST_SHARED_PTR_ENABLE_DEBUG_HOOKS_PATH}")
endif(BOOST_SHARED_PTR_ENABLE_DEBUG_HOOKS_PATH)
endif(BOOST_SHARED_PTR_ENABLE_DEBUG_HOOKS)

set(BOOST_LIBRARIES ${Boost_LIBRARIES})

# ---------- Setup output Directories -------------------------
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/lib
  CACHE PATH
  "Single Directory for all Libraries"
  )

# --------- Setup the Executable output Directory -------------
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/bin
  CACHE PATH
  "Single Directory for all Executables."
  )

# --------- Setup the Executable output Directory -------------
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${PROJECT_BINARY_DIR}/lib
  CACHE PATH
  "Single Directory for all static libraries."
  )



	# ------- Enable the CTest testing. Use make test to run all tests ---------------
INCLUDE(CTest)
option(BUILD_TESTING "Build Testing" ON)
if(BUILD_TESTING)
  ENABLE_TESTING()

	set (STATIC_VECTOR_TESTS
	  ${CMAKE_SOURCE_DIR}/StaticVectorTests.cpp
	)

	add_executable ( staticVectorTests ${STATIC_VECTOR_TESTS} )
	
	target_link_libraries( staticVectorTests
		${Boost_LIBRARIES}
	)
	

  ADD_TEST(AllStaticVectorTests bin/staticVectorTests)

endif(BUILD_TESTING)


include_directories("${CMAKE_SOURCE_DIR}")


	set (STATIC_VECTOR_EXAMPLE
	  ${CMAKE_SOURCE_DIR}/StaticVectorExample.cpp
	)


add_executable ( staticVectorExample ${STATIC_VECTOR_EXAMPLE} )

#target_link_libraries( staticVectorExample
#  boost_graph
#)

add_executable(benchStaticVector benchStaticVector.cpp)
	


