/**
 *  @file   StaticVectorTest.cpp
 *  @date   Apr 19, 2011
 *  @author Andrew Hundt <ahundt@cmu.edu>
 *
 *  (C) Carnegie Mellon University 2011
 *
 *  Distributed under the Boost Software License, Version 1.0. (See
 *  accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  @brief  StaticVectorTest.cpp
 *
 *  StaticVector is not accepted as part of boost.
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE StaticVectorTests

#include <StaticVector.hpp>
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <vector>
using namespace boost;

template <typename T, std::size_t U>
void FillVectorWithPushBack(StaticVector<T,U>& vec){
  for(std::size_t i = 0; i < U; i++){
    vec.push_back(i);
  }
}

template <typename T>
void PushBackN(T& vec, std::size_t n){
  std::size_t N=n;
  for(; n > 0; n--){
    vec.push_back(N-n);
  }
}

#define VEC_SIZE 5

BOOST_AUTO_TEST_CASE(StaticVectorConstructor){
  StaticVector<std::size_t,1> one;
  StaticVector<std::size_t,2> two;
  StaticVector<std::size_t,3> three;
  StaticVector<std::size_t,4> four;
}

BOOST_AUTO_TEST_CASE(ConstructFromArray){
  std::size_t array[VEC_SIZE] = {0,1,2,3,4};
  StaticVector<std::size_t,VEC_SIZE> vec(array,&array[VEC_SIZE]);
  BOOST_CHECK_EQUAL(vec.size(),VEC_SIZE);
}

BOOST_AUTO_TEST_CASE(PushBack){
  StaticVector<std::size_t,VEC_SIZE> vec;
  FillVectorWithPushBack(vec);
  BOOST_CHECK_EQUAL(vec.size(),VEC_SIZE);
}

BOOST_AUTO_TEST_CASE(OverFill){
  StaticVector<std::size_t,VEC_SIZE> vec;
  FillVectorWithPushBack(vec);
  BOOST_CHECK_EQUAL(vec.size(),VEC_SIZE);
  BOOST_CHECK_THROW( vec.push_back(VEC_SIZE),std::exception);
}

BOOST_AUTO_TEST_CASE(Clear){
  StaticVector<std::size_t,VEC_SIZE> vec;
  FillVectorWithPushBack(vec);
  BOOST_CHECK_EQUAL(vec.size(),VEC_SIZE);
  vec.clear();
  BOOST_CHECK_EQUAL(vec.size(),0);
}

BOOST_AUTO_TEST_CASE(AddRemove){
  StaticVector<std::size_t,VEC_SIZE> vec;
  FillVectorWithPushBack(vec);
  BOOST_CHECK_EQUAL(vec.size(),VEC_SIZE);
  vec.clear();
  BOOST_CHECK_EQUAL(vec.size(),0);
  vec.push_back(20);
  BOOST_CHECK_EQUAL(vec.size(),1);
  vec.pop_back();
  BOOST_CHECK_EQUAL(vec.size(),0);
  vec.push_back(20);
  vec.push_back(30);
  BOOST_CHECK_EQUAL(vec[1],30);
  BOOST_CHECK_EQUAL(vec[0],20);
  BOOST_CHECK_EQUAL(vec.size(),2);
  vec.pop_back();
  vec.pop_back();
  BOOST_CHECK_EQUAL(vec.size(),0);
}


BOOST_AUTO_TEST_CASE(PopEmpty){
  StaticVector<std::size_t,VEC_SIZE> vec;
  FillVectorWithPushBack(vec);
  vec.clear();
  BOOST_CHECK_THROW( vec.pop_back(),std::exception);
}

BOOST_AUTO_TEST_CASE(ReverseConstruction){
  StaticVector<std::size_t,VEC_SIZE> vec;
  FillVectorWithPushBack(vec);
  BOOST_CHECK_EQUAL(vec.size(),VEC_SIZE);
  StaticVector<std::size_t,VEC_SIZE> rvec(vec.rbegin(),vec.rend());
  BOOST_CHECK_EQUAL(rvec.size(),VEC_SIZE);
  BOOST_CHECK_EQUAL(vec.size(),rvec.size());
  for(std::size_t i = 0;  i < vec.size(); i++ ){
    BOOST_CHECK_EQUAL(vec[i],rvec[vec.size()-i-1]);
  }
}

BOOST_AUTO_TEST_CASE(CompareIteratorWithStdVector){
  StaticVector<std::size_t,VEC_SIZE> vec;
  std::vector<std::size_t> stdvec;
  PushBackN(vec,VEC_SIZE-2);
  PushBackN(stdvec,VEC_SIZE-2);
  BOOST_CHECK_EQUAL_COLLECTIONS(vec.begin(),vec.end(),stdvec.begin(),stdvec.end());
}

