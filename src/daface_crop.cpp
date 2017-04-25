/******************************************************************************
 * FileName: dafece_crop.cpp
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: crop facial region image & resize to unified size
 *****************************************************************************/

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "util/boost_app_common.h"
#include "util/boost_app_fs.h"

#include "daface_crop.h"

using namespace std;
using namespace cv;

static void CropFacialRegion(std::string path_src, std::string path_dst,
                             std::vector<float> facial_points, 
                             int new_size = 256);


void DafaceCrop(string file_info, string dir_dst, int new_size)
{
  const string SYMBOL_AT("@");
  const string SYMBOL_SPACING(" ");
  const int NUM_FACIAL_POINT = 5;
  //const string NAME_ADD("_crop");
  
  ifstream ifs_info(file_info, ios::in);
  string each_line;
  
  string dir_dst_crop = PathJoin(dir_dst, "crop");
  CreateDir(dir_dst_crop);
  
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
    
    vector<float> facial_points;
    for (int cnt=0; cnt<NUM_FACIAL_POINT; ++cnt)
    {
      int point_x = String2Int(vec_points[cnt*2]);
      int point_y = String2Int(vec_points[cnt*2+1]);

      facial_points.push_back(point_x);
      facial_points.push_back(point_y);
    }
    
    string path_new = ChangeDir(path_src, dir_dst_crop);
    //string path_new = ChangeDirFine(path_src, dir_dst_crop, NAME_ADD, 2);
    
    CropFacialRegion(path_src, path_new, facial_points, new_size);
  }
  
  ifs_info.close();
}

static void CropFacialRegion(string path_src, string path_dst,
                             vector<float> facial_points, 
                             int new_size)
{
  Mat img_src = imread(path_src);
  int img_w = img_src.cols;
  int img_h = img_src.rows;
  
  vector<float> points_x, points_y;
  for (vector<float>::iterator iter=facial_points.begin(); iter!=facial_points.end(); iter+=2)
  {
    points_x.push_back(*iter);
    points_y.push_back(*(iter+1));
  }
  
  float x_min = min_element(begin(points_x), end(points_x))[0];
  float x_max = max_element(begin(points_x), end(points_x))[0];
  float y_min = min_element(begin(points_y), end(points_y))[0];
  float y_max = max_element(begin(points_y), end(points_y))[0];
  
  float center_x = (x_max - x_min) / 2.0 + x_min;
  float center_y = (y_max - y_min) / 2.0 + y_min;
  
  float face_w = x_max - x_min;
  float face_h = y_max - y_min;
  
  float face_size = std::max(face_w, face_h);
  
  float scale = 0.92;
  
  Rect rect_crop;
  rect_crop.x = std::max(.0f, center_x - face_size * scale);
  rect_crop.y = std::max(.0f, center_y - face_size * scale);
  rect_crop.width = face_size * scale * 2;
  rect_crop.height = face_size * scale * 2;
  
  if ((rect_crop.x + rect_crop.width) > img_w ||
      (rect_crop.y + rect_crop.height) > img_h)
  {
    float temp_w = img_w - rect_crop.x * 2;
    float temp_h = img_h - rect_crop.y * 2;
    
    rect_crop.width = rect_crop.height = std::min(temp_w, temp_h);
  }
  
  if (rect_crop.x < 0 || rect_crop.y < 0 ||
      rect_crop.width < 0 || rect_crop.height < 0 || 
      (rect_crop.x + rect_crop.width) > img_w ||
      (rect_crop.y + rect_crop.height) > img_h)
  {
    cout << path_src << "   ";
    cout << rect_crop.x << " ";
    cout << rect_crop.y << " ";
    cout << rect_crop.width << " ";
    cout << rect_crop.height << endl;
  }
  else
  {
    Mat img_crop = img_src(rect_crop);
  
    Size size_dst = Size(new_size, new_size);
    Mat img_resize;
    resize(img_crop, img_resize, size_dst);
    
    imwrite(path_dst, img_resize);
  }
}



