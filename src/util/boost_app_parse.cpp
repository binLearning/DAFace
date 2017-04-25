/******************************************************************************
 * FileName: boost_app_parse.cpp
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: Use BOOST to parse parameters from command line | config file
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define BOOST_ALL_NO_LIB
#include "boost/program_options.hpp"
#include "libs/program_options/src/cmdline.cpp"
#include "libs/program_options/src/config_file.cpp"
#include "libs/program_options/src/convert.cpp"
#include "libs/program_options/src/options_description.cpp"
#include "libs/program_options/src/parsers.cpp"
#include "libs/program_options/src/positional_options.cpp"
#include "libs/program_options/src/split.cpp"
#include "libs/program_options/src/utf8_codecvt_facet.cpp"
#include "libs/program_options/src/value_semantic.cpp"
#include "libs/program_options/src/variables_map.cpp"
#include "libs/program_options/src/winmain.cpp"

#include "boost/algorithm/string.hpp"
#include "boost/lexical_cast.hpp"

#include "util/terminal_color.h"
#include "util/boost_app_fs.h"
#include "util/boost_app_parse.h"

namespace bpo = boost::program_options;
using namespace std;

const int CCT_INFO   = 0;
const int CCT_STRESS = 1;
const int CCT_ERROR  = 2;
static void CustomCout(std::string output, int type = CCT_INFO);
static bool IsValidArgs(map<string,string>& param_map);


/*
NOTE:
modify libs/program_options/src/options_description.cpp
const unsigned options_description::m_default_line_length  80=>160
*/

bool ParamParsing(int argc, char* argv[], map<string,string>& param_map)
{
  string mode_cmdline;
  string src_dir_cmdline;
  string dst_dir_cmdline;
  string info_file_cmdline;
  string new_size_cmdline;
  string parse_type;
  string config_file;
  
  bpo::options_description opts_help("Usage");
  opts_help.add_options()
      ("help,h", "display help message\n\n"
          "\r     Detect      detect face region & 5 facial points\n"
          "\r     (det)       ./proc --mode=det --src_dir=sdir [--dst_dir=ddir]\n"
          "\r     Clean       clean the invalid facial pionts data\n"
          "\r     (clean)     ./proc --mode=clean --info_file=textfile "
          "[--dst_dir=ddir]\n"
          "\r     Draw        draw facial pionts\n"
          "\r     (draw)      ./proc --mode=draw --info_file=textfile "
          "[--dst_dir=ddir]\n"
          "\r     Crop        crop facial region & resize to unified size\n"
          "\r     (crop)      ./proc --mode=crop --info_file=textfile "
          "[--dst_dir=ddir] [--new_size=256]\n"
          "\r     Normalize   align face & resize to unified size\n"
          "\r     (norm)      ./proc --mode=norm --info_file=textfile "
          "[--dst_dir=ddir] [--new_size=256]"
      );
  
  bpo::options_description opts_cmdline("Args");
  opts_cmdline.add_options()
      ("mode", bpo::value<string>(&mode_cmdline), 
          "[required] work mode\n"
          "currently supported: det | clean | draw | crop | norm")
      ("src_dir", bpo::value<string>(&src_dir_cmdline),
          "[required] source directory\n"
          "directory to store the source images\n"
          "better to use absolute path")
      ("dst_dir", bpo::value<string>(&dst_dir_cmdline),
          "[optional] destination directory\n"
          "directory to store the processed images\n"
          "default: current folder")
      ("info_file", bpo::value<string>(&info_file_cmdline),
          "information file\n"
          "textfile to store the facial points information")
      ("new_size", bpo::value<string>(&new_size_cmdline),
          "[optional] unified size, effective range [64, 1280]\n"
          "default: 256")
      ("parse_type", bpo::value<string>(&parse_type),
          "[optional] parse type: cmdline | config\n"
          "parse parameters from command line or config file\n"
          "default: cmdline")
      ("config_file", bpo::value<string>(&config_file),
          "config file\n"
          "required if parse parameters from config file")
      ;
  
  bpo::options_description opts_showhelp;
  opts_showhelp.add(opts_help).add(opts_cmdline);
  
  bpo::variables_map vmap_cmdline;
  bpo::store(bpo::parse_command_line(argc, argv, opts_showhelp), vmap_cmdline);
  
  if(vmap_cmdline.count("help") || vmap_cmdline.size() == 0)
  {
    cout << opts_showhelp << endl;
    
    return false;
  }
  
  bpo::notify(vmap_cmdline);
  
  bool is_valid = false;
  
  const string PARSE_TYPE_CONFIG("config");
  if (parse_type == PARSE_TYPE_CONFIG)
  {
    CustomCout("Parse parameters from config file", CCT_STRESS);
    
    if (!IsExists(config_file))
    {
      CustomCout("Could not find config file " + config_file, CCT_ERROR);
      
      return false;
    }
    
    string mode_config;
    string src_dir_config;
    string dst_dir_config;
    string info_file_config;
    string new_size_config;
    
    bpo::options_description opts_config;
    opts_config.add_options()
      ("mode", bpo::value<string>(&mode_config), "work mode")
      ("src_dir", bpo::value<string>(&src_dir_config), "source directory")
      ("dst_dir", bpo::value<string>(&dst_dir_config), "destination directory")
      ("info_file", bpo::value<string>(&info_file_config), "information file")
      ("new_size", bpo::value<string>(&new_size_config), "unified size")
      ;
    
    bpo::variables_map vmap_config;
    ifstream ifs_config(config_file);
    
    bpo::store(bpo::parse_config_file(ifs_config, opts_config, true), vmap_config);
    bpo::notify(vmap_config);
    
    param_map["mode"] = mode_config;
    param_map["src_dir"] = src_dir_config;
    param_map["dst_dir"] = dst_dir_config;
    param_map["info_file"] = info_file_config;
    param_map["new_size"] = new_size_config;
    
    is_valid = IsValidArgs(param_map);
    
    return is_valid;
  }
  
  param_map["mode"] = mode_cmdline;
  param_map["src_dir"] = src_dir_cmdline;
  param_map["dst_dir"] = dst_dir_cmdline;
  param_map["info_file"] = info_file_cmdline;
  param_map["new_size"] = new_size_cmdline;

  is_valid = IsValidArgs(param_map);
  
  return is_valid;
}

static bool IsValidArgs(map<string,string>& param_map)
{
  string mode = param_map["mode"];
  string src_dir = param_map["src_dir"];
  string dst_dir = param_map["dst_dir"];
  string info_file = param_map["info_file"];
  string new_size = param_map["new_size"];
  
  const string MODE_TYPES("det|clean|draw|crop|norm");
  const int DEFAULT_SIZE = 256;
  const int MIN_SIZE = 64;
  const int MAX_SIZE = 1280;
  
  bool is_valid = true;
  string error_info("Param Setting Error > ");
  
  // [required] mode
  if (mode.empty() || !boost::contains(MODE_TYPES, mode))
  {
    CustomCout(error_info + "mode", CCT_ERROR);
    CustomCout("currently supported: det | clean | draw | crop | norm", CCT_INFO);
    is_valid = false;
  }
  
  // [required] src_dir or info_dir
  if (mode == "det") // det require src_dir
  {
    if (!src_dir.empty())
    {
      if (!IsExists(src_dir))
      {
        CustomCout(error_info + "src_dir", CCT_ERROR);
        CustomCout(src_dir + " does NOT exist", CCT_INFO);
        is_valid = false;
      }
    }
    else
    {
      CustomCout(error_info + "src_dir", CCT_ERROR);
      CustomCout("src_dir is required in mode(det)", CCT_INFO);
      is_valid = false;
    }
  }
  else // clean|draw|crop|norm require info_file
  {
    if (!info_file.empty())
    {
      if (!IsExists(info_file))
      {
        CustomCout(error_info + "info_file", CCT_ERROR);
        CustomCout(info_file + " does NOT exist", CCT_INFO);
        is_valid = false;
      }
    }
    else
    {
      CustomCout(error_info + "info_file", CCT_ERROR);
      CustomCout("info_file is required in mode(clean|draw|crop|norm)", CCT_INFO);
      is_valid = false;
    }
  }
  
  // [optional] dst_dir
  if (!dst_dir.empty())
  {
    if (!IsExists(dst_dir))
    {
      CustomCout(error_info + "dst_dir", CCT_ERROR);
      CustomCout(dst_dir + " does NOT exist", CCT_INFO);
      is_valid = false;
    }
  }
  else
  {
    dst_dir = GetCurrentFolderPath();
  }
  
  // [optional] new_size
  if (mode == "crop" || mode == "norm") // crop|norm require new_size
  {
    if (!new_size.empty())
    {
      int temp_size = boost::lexical_cast<int>(new_size);
      
      if (temp_size < MIN_SIZE || temp_size > MAX_SIZE)
      {
        CustomCout(error_info + "new_size", CCT_ERROR);
        CustomCout("effective range [64, 1280]", CCT_INFO);
        is_valid = false;
      }
    }
    else
    {
      new_size = boost::lexical_cast<string>(DEFAULT_SIZE);
    }
  }
  
  if (is_valid)
  {
    param_map["mode"] = mode;
    param_map["src_dir"] = src_dir;
    param_map["dst_dir"] = dst_dir;
    param_map["info_file"] = info_file;
    param_map["new_size"] = new_size;
    
    // overview
    cout << "\n--------------------------------------------------" << endl;
    cout << "mode:        " << mode << endl;
    if (mode == "det")
      cout << "src_dir:     " << src_dir << endl;
    else
      cout << "info_file:   " << info_file << endl;
    cout << "dst_dir:     " << dst_dir << endl;
    if (mode == "crop" || mode == "norm")
      cout << "new_size:    " << new_size << endl;
    cout << "--------------------------------------------------\n" << endl;
  }
  
  return is_valid;
}

static void CustomCout(string output, int type)
{
  switch (type)
  {
    case CCT_INFO:
      cout << TC_BOLDMAGENTA << output << TC_RESET << endl;
      break;
    case CCT_STRESS:
      cout << TC_BOLDGREEN << output << TC_RESET << endl;
      break;
    case CCT_ERROR:
      cout << TC_BOLDRED << output << TC_RESET << endl;
      break;
    default:
      cout << output << endl;
      break;
  }
}



