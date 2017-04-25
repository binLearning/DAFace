/******************************************************************************
 * FileName: boost_app_common.h
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: Use BOOST to deal with common application
 *****************************************************************************/

#ifndef _BOOST_APP_COMMON_H_
#define _BOOST_APP_COMMON_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

int RoundCalc(double dnum);
int String2Int(string str_num);
float String2Float(string str_num);
void StringSplit(vector<string>& vec_substr, string src_str, string split_symbol);

//#define BOOST_ALL_NO_LIB
//#include "boost/lexical_cast.hpp"
//template <typename T>
//T String2Num(string str_num)
//{
//  T num = boost::lexical_cast<T>(str_num);
//  return num;
//}

#endif

