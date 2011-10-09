# Set project's name
project(StaticVector)
cmake_minimum_required(VERSION 2.6)


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")


find_package(Boost COMPONENTS unit_test_framework REQUIRED)

link_directories ( ${Boost_LIBRARY_DIRS} )
include_directories ( ${Boost_INCLUDE_DIRS} )

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
		boost_unit_test_framework
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
	


