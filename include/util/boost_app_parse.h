/******************************************************************************
 * FileName: boost_app_parse.h
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: Use BOOST to parse parameters from command line | config file
 *****************************************************************************/

#ifndef _BOOST_APP_PARSE_H_
#define _BOOST_APP_PARSE_H_

#include <map>
#include <string>

bool ParamParsing(int argc, char* argv[], std::map<std::string,std::string>&);

#endif

