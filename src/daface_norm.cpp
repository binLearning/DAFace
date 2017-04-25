/******************************************************************************
 * FileName: dafece_norm.cpp
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: align face & resize to unified size
 *
 * Reference: SeetaFace/FaceIdentification/tools/aligner.cpp
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "util/boost_app_common.h"
#include "util/boost_app_fs.h"

#include "daface_norm.h"

using namespace std;
using namespace cv;

static vector<float> CalcTransParam(vector<float> facial_points, 
                                    int new_size = 256);
static void TransformImage(string path_src, string path_dst,
                           vector<float> facial_points,
                           int new_size = 256);


void DafaceNorm(string file_info, string dir_dst, int new_size)
{
  const string SYMBOL_AT("@");
  const string SYMBOL_SPACING(" ");
  const int NUM_FACIAL_POINT = 5;
  //const string NAME_ADD("_norm");
  
  ifstream ifs_info(file_info, ios::in);
  string each_line;
  
  string dir_dst_norm = PathJoin(dir_dst, "norm");
  CreateDir(dir_dst_norm);
  
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
    
    string path_new = ChangeDir(path_src, dir_dst_norm);
    //string path_new = ChangeDirFine(path_src, dir_dst, NAME_ADD, 2);
    
    TransformImage(path_src, path_new, facial_points, new_size);
  }
  
  ifs_info.close();
}

static vector<float> CalcTransParam(vector<float> facial_points, int new_size)
{
  const int NUM_FACIAL_POINT = 5;
  //float std_points[10] = // 256*256, seeta-face
  //{
  //  89.3095, 72.9025,     // left eye
  //  169.3095, 72.9025,    // right eye
  //  127.8949, 127.0441,   // nose
  //  96.8796, 184.8907,    // left mouse corner
  //  159.1065, 184.7601,   // right mouse corner
  //};
  float std_points[10] = // 256*256, less background
  {
    82.1446, 62.6993,
    176.9594, 62.6993,
    127.8754, 126.8671,
    91.1166, 207.2779,
    164.867, 207.1231
  };
  //float std_points[10] = // 256*256, no background
  //{
  //  73.6523, 50.6066,
  //  186.023, 50.6066,
  //  127.8523, 127.8425,
  //  84.2863, 221.959,
  //  171.6942, 221.7755
  //};
  
  float scale = new_size / 256.0;
  for (int cnt=0; cnt<10; ++cnt)
  {
    std_points[cnt] *= scale;
  }

  float sum_x = 0, sum_y = 0;
  float sum_u = 0, sum_v = 0;
  float sum_xx_yy = 0;
  float sum_ux_vy = 0;
  float sum_vx__uy = 0;
  for (int cnt=0; cnt<NUM_FACIAL_POINT; ++cnt)
  {
    int x_off = cnt * 2;
    int y_off = x_off + 1; 
    sum_x += std_points[x_off];
    sum_y += std_points[y_off];
    sum_u += facial_points[x_off];
    sum_v += facial_points[y_off];
    sum_xx_yy += std_points[x_off] * std_points[x_off] + 
                 std_points[y_off] * std_points[y_off];
    sum_ux_vy += std_points[x_off] * facial_points[x_off] +
                 std_points[y_off] * facial_points[y_off];
    sum_vx__uy += facial_points[y_off] * std_points[x_off] -
                  facial_points[x_off] * std_points[y_off];
  }
  
  float q = sum_u - sum_x * sum_ux_vy / sum_xx_yy 
                   + sum_y * sum_vx__uy / sum_xx_yy;
                   
  float p = sum_v - sum_y * sum_ux_vy / sum_xx_yy 
                   - sum_x * sum_vx__uy / sum_xx_yy;

  float r = NUM_FACIAL_POINT - (sum_x * sum_x + sum_y * sum_y) / sum_xx_yy;

  float a = (sum_ux_vy - sum_x * q / r - sum_y * p / r) / sum_xx_yy;

  float b = (sum_vx__uy + sum_y * q / r - sum_x * p / r) / sum_xx_yy; 

  float c = q / r;

  float d = p / r;
  
  vector<float> trans_param;
  trans_param.push_back(a);
  trans_param.push_back(-b);
  trans_param.push_back(c);
  trans_param.push_back(b);
  trans_param.push_back(a);
  trans_param.push_back(d);
  
  return trans_param;
}

static void TransformImage(string path_src, string path_dst,
                           vector<float> facial_points, 
                           int new_size)
{
  Mat img_src, img_resize, img_norm;
  Size size_dst = Size(new_size, new_size);
  
  img_src = imread(path_src);
  resize(img_src, img_resize, size_dst);
  img_norm.create(size_dst, img_src.type());
  
  int ori_size = img_src.rows; // or img_src.cols
  float scale = static_cast<float>(new_size) / static_cast<float>(ori_size);

  for (vector<float>::iterator iter=facial_points.begin(); iter!=facial_points.end(); ++iter)
  {
    *iter *= scale;
  }
  
  vector<float> trans_param = CalcTransParam(facial_points, new_size);
  
  Mat map_x, map_y;
  map_x.create(size_dst, CV_32FC1);
  map_y.create(size_dst, CV_32FC1);
  
  // Get the source position of each point on the destination feature map
  for (int y=0; y<new_size; ++y)
  {
    for (int x=0; x<new_size; ++x)
    {
      map_x.at<float>(y,x) = trans_param[0]*x + trans_param[1]*y + trans_param[2];
      map_y.at<float>(y,x) = trans_param[3]*x + trans_param[4]*y + trans_param[5];
    }
  }
  
  remap(img_resize, img_norm, map_x, map_y, CV_INTER_LINEAR);
  
  imwrite(path_dst, img_norm);
}



