/******************************************************************************
 * FileName: main.cpp
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: main function
 *****************************************************************************/

#include <iostream>
#include <string>

#include "util/boost_app_common.h"
#include "util/boost_app_parse.h"

#include "daface_det.h"
#include "daface_clean.h"
#include "daface_draw.h"
#include "daface_crop.h"
#include "daface_norm.h"

using namespace std;


int main(int argc, char *argv[])
{
  map<string,string> param_map;
  bool is_valid = ParamParsing(argc, argv, param_map);
  
  if (!is_valid)
    return -1;
  
  const string MODE_DET("det");
  const string MODE_CLEAN("clean");
  const string MODE_DRAW("draw");
  const string MODE_CROP("crop");
  const string MODE_NORM("norm");
  
  string mode = param_map["mode"];
  string src_dir = param_map["src_dir"];
  string dst_dir = param_map["dst_dir"];
  string info_file = param_map["info_file"];
  int new_size = String2Int(param_map["new_size"]);
  
  if (mode == MODE_DET)
    DafaceDet(src_dir, dst_dir);
    
  if (mode == MODE_CLEAN)
    DafaceClean(info_file, dst_dir);
    
  if (mode == MODE_DRAW)
    DafaceDraw(info_file, dst_dir);
    
  if (mode == MODE_CROP)
    DafaceCrop(info_file, dst_dir, new_size);
    
  if (mode == MODE_NORM)
    DafaceNorm(info_file, dst_dir, new_size);
  
  return 0;
}



