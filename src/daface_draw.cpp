/******************************************************************************
 * FileName: dafece_draw.cpp
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: draw facial pionts
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "util/boost_app_common.h"
#include "util/boost_app_fs.h"

#include "daface_draw.h"

using namespace std;


void DafaceDraw(string file_info, string dir_dst)
{
  const string SYMBOL_AT("@");
  const string SYMBOL_SPACING(" ");
  const int NUM_FACIAL_POINT = 5;
  
  ifstream ifs_info(file_info, ios::in);
  string each_line;
  
  string dir_dst_draw = PathJoin(dir_dst, "draw");
  CreateDir(dir_dst_draw);
  
  ///while (!ifs_info.eof())
  while (ifs_info.peek() != EOF)
  {
    getline(ifs_info, each_line);
    
    vector<string> vec_path_points, vec_points;
    StringSplit(vec_path_points, each_line, SYMBOL_AT);
    
    string path_src(vec_path_points[0]);
    
    // Remove the redundant spacing
    vec_path_points[1].erase(0, vec_path_points[1].find_first_not_of(SYMBOL_SPACING));
    vec_path_points[1].erase(vec_path_points[1].find_last_not_of(SYMBOL_SPACING)+1);
    
    StringSplit(vec_points, vec_path_points[1], SYMBOL_SPACING);
    
    if (vec_points.size() != NUM_FACIAL_POINT*2)
    {
      cout << "[INFO ERROR]" << path_src << endl;
      continue;
    }
    
    cv::Mat img_src = cv::imread(path_src);
    for (int index=0; index<NUM_FACIAL_POINT; ++index)
    {
      int point_x = String2Int(vec_points[index*2]);
      int point_y = String2Int(vec_points[index*2+1]);
      cv::Point center = cv::Point(point_x, point_y);
      
      cv::circle(img_src, center, 2, cv::Scalar(0,255,0), -1, 8, 0);
    }
    
    string path_new = ChangeDir(path_src, dir_dst_draw);
    cv::imwrite(path_new, img_src);
  }
  
  ifs_info.close();
}



