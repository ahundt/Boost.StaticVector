/**
 *  @file   StaticVectorExample.cpp
 *  @date   Aug 14, 2011
 *  @author Andrew Hundt <ATHundt@gmail.com>
 *
 *  (C) Andrew Hundt 2011 <ATHundt@gmail.com>
 *
 *  Distributed under the Boost Software License, Version 1.0. (See
 *  accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  @brief  StaticVectorExample.cpp demonstrates the use of StaticVector
 *
 *  StaticVector is not accepted as part of boost.
 */
 
#include <iostream>
#include <string>
#include <set>
#include "StaticVector.hpp"

using namespace std;
using namespace boost;

typedef StaticVector<std::set<std::size_t>,3> ThreeSetType;
  
void print(std::size_t value){
  cout << " " << value;
}


void printSet(std::set<std::size_t> value){
  cout << " (";
  std::for_each(value.begin(),value.end(),print);
  cout << ")";
}

ThreeSetType makeThreeSet(){
  ThreeSetType t;
  std::set<std::size_t> s;
  s.insert(3);
  t.push_back(s);
  s.insert(2);
  t.push_back(s);
  s.insert(1);
  t.push_back(s);
  return t;
}

int main(int argc, char** argv){
  
  cout << "Creating StaticVector of capacity 3" << std::endl;
  StaticVector<std::size_t,3> three;

  cout << "Push back 5" << std::endl;
  three.push_back(5);
  cout << "Push back 2" << std::endl;
  three.push_back(2);
  cout << "size: " << three.size() << " capacity: "  << three.capacity() << std::endl; // size: 2 capacity: 3
  
  cout << "Push back 1" << std::endl;
  three.push_back(1);
  
  cout << "Values:" << std::endl;
  std::for_each(three.begin(),three.end(),print);
  
  cout << std::endl << "Adding one more than the capacity:" << std::endl;
  try {
    three.push_back(3); // throws std::out_of_range exception indicating the capacity has been exceeded
  } catch(std::out_of_range e) {
    cout << "Caught exception: " << e.what();
  }
  
  cout << std::endl << "After overfill:";
  cout << "size: " << three.size() << " capacity: "  << three.capacity() << std::endl; // size: 3 capacity: 3
  cout << "Values:" << std::endl;
  std::for_each(three.begin(),three.end(),print);
  cout << std::endl;
  cout << "Creating threeSet, a StaticVector of 3 std::set objects containing (3), (3 2), and (3 2 1), respectively" << std::endl;
  ThreeSetType threeSet = makeThreeSet();
  cout << "threeSet Values:" << std::endl;
  std::for_each(threeSet.begin(),threeSet.end(),printSet);
  
  cout << std::endl;
  cout << "Sorting threeSet:" << std::endl;
  std::sort(threeSet.begin(), threeSet.end());
  std::for_each(threeSet.begin(),threeSet.end(),printSet);
  
  cout << std::endl << "Success!" << std::endl;
  
}