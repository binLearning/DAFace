/******************************************************************************
 * FileName: boost_app_common.cpp
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: Use BOOST to deal with common application
 *****************************************************************************/

#include <iostream>

#define BOOST_ALL_NO_LIB
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/math/special_functions/round.hpp"

#include "util/boost_app_common.h"


int RoundCalc(double dnum)
{
  int inum = boost::math::round(dnum);
  
  return inum;
}

int String2Int(string str_num)
{
  int num = boost::lexical_cast<int>(str_num);
  
  return num;
}

float String2Float(string str_num)
{
  float num = boost::lexical_cast<float>(str_num);
  
  return num;
}

void StringSplit(vector<string>& vec_substr, string src_str, string split_symbol)
{
  boost::split(vec_substr, src_str, boost::is_any_of(split_symbol));
}



