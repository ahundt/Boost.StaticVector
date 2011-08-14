StaticVector Class

Purpose:
  StaticVector is a statically allocated fixed capacity vector. The class is header only.

Motivation:
  The motivation for this class came from using boost.array in an interprocess library, when an adjustable size boost.array without the complexity of the vector class in the interprocess library became desirable. The result is StaticVector, which is boost.array directly modified with a size in front of the array, and added facilities to match std::vector. 

Building:

  Go to the top level directory and type:
    cmake .
    make

Testing:

  To run the unit tests after make is complete type:
    make test

  This will run all the unit tests and notify whether any of the tests have failed.
  To see detailed information about failed unit tests, see Testing/Temporary/LatestTest.log
  It is suggested that unit tests are run every time changes to the code are made.

Requirements:
  boost
  cmake (for included Test build script only)