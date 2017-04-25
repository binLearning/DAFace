/******************************************************************************
 * FileName: dafece_clean.cpp
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: clean the invalid facial points data
 *****************************************************************************/

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "util/boost_app_common.h"
#include "util/boost_app_fs.h"

#include "daface_clean.h"

using namespace std;


void DafaceClean(string file_info, string dir_dst)
{
  const string SYMBOL_AT("@");
  const string SYMBOL_SPACING(" ");
  const int NUM_FACIAL_POINT = 5;
  
  // for LFW
  const float MIN_W = 20.0;
  const float MIN_H = 30.0;
  const float OFFSET_RATIO = 0.3;
  
  string dir_dst_clean = PathJoin(dir_dst, "clean");
  CreateDir(dir_dst_clean);
  
  string file_doubt = PathJoin(dir_dst_clean, "doubt.txt");
  string file_cleaned = PathJoin(dir_dst_clean, "cleaned.txt");
  
  ifstream ifs_info(file_info, ios::in);
  ofstream ofs_doubt(file_doubt, ios::out);
  ofstream ofs_cleaned(file_cleaned, ios::out);
  
  string each_line;
  
  int cnt = 0;
  int cnt_doubt = 0;
  
  ///while (!ifs_info.eof())
  while (ifs_info.peek() != EOF)
  {
    ++cnt;
    if (cnt % 10000 == 0)
    {
      cout << cnt << endl;
    }
    
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
    
    vector<float> points_x, points_y;
    for (vector<string>::iterator iter=vec_points.begin(); iter!=vec_points.end(); iter+=2)
    {
      points_x.push_back(String2Float(*iter));
      points_y.push_back(String2Float(*(iter+1)));
    }
    float x_min = min_element(begin(points_x), end(points_x))[0];
    float x_max = max_element(begin(points_x), end(points_x))[0];
    float y_min = min_element(begin(points_y), end(points_y))[0];
    float y_max = max_element(begin(points_y), end(points_y))[0];
    
    float center_x = (x_max - x_min) / 2.0f + x_min;
    float center_y = (y_max - y_min) / 2.0f + y_min;
    
    float face_w = x_max - x_min;
    float face_h = y_max - y_min;
    
    cv::Mat img_src = cv::imread(path_src);
    int img_w = img_src.cols;
    int img_h = img_src.rows;
    
    float min_center_x = img_w * OFFSET_RATIO;
    float max_center_x = img_w * (1.0f - OFFSET_RATIO);
    float min_center_y = img_h * OFFSET_RATIO;
    float max_center_y = img_h * (1.0f - OFFSET_RATIO);
    
    if (face_w < MIN_W || face_h < MIN_H ||
        center_x < min_center_x || center_x > max_center_x ||
        center_y < min_center_y || center_y > max_center_y)
    {
      ++cnt_doubt;
      ofs_doubt << each_line << endl;
    }
    else
    {
      ofs_cleaned << each_line << endl;
    }
  }
  
  cout << endl << "[DOUBT]   " << cnt_doubt << endl;
  cout << "[VALID]   " << cnt - cnt_doubt << endl;
  
  ifs_info.close();
  ofs_doubt.close();
  ofs_cleaned.close();
}



