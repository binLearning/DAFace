/******************************************************************************
 * FileName: dafece_det.cpp
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: use seeta-face to detect face region & 5 facial points
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "seeta_face/detection/face_detection.h"
#include "seeta_face/alignment/face_alignment.h"

#include "util/terminal_color.h"

#include "util/boost_app_common.h"
#include "util/boost_app_fs.h"

#include "daface_det.h"

using namespace std;


void DafaceDet(string dir_src, string dir_dst)
{
  vector<string> image_list = GetImageList(dir_src);
  
  string model_folder("../model/seeta/");
  
  // Initialize face detection model
  string detection_model(model_folder + "seeta_fd_frontal_v1.0.bin");
  seeta::FaceDetection detector(detection_model.c_str());
  detector.SetMinFaceSize(50);
  detector.SetScoreThresh(2.f);
  detector.SetImagePyramidScaleFactor(0.8f);
  detector.SetWindowStep(2,2);
  
  // Initialize face alignment model
  string alignment_model(model_folder + "seeta_fa_v1.1.bin");
  seeta::FaceAlignment point_detector(alignment_model.c_str());
  
  string dir_dst_det = PathJoin(dir_dst, "det");
  CreateDir(dir_dst_det);
  
  // Create sub-directories to store the processed images
  //string dir_dst_detected = PathJoin(dir_dst_det, "detected");
  //string dir_dst_undetected = PathJoin(dir_dst_det, "undetected");
  //CreateDir(dir_dst_detected);
  //CreateDir(dir_dst_undetected);
  
  // Create text file to store the location information
  string log_detected = PathJoin(dir_dst_det, "detected.txt");
  string log_undetected = PathJoin(dir_dst_det, "undetected.txt");
  string log_multiface = PathJoin(dir_dst_det, "multiface.txt");
  ofstream ofs_detected(log_detected, ios::out);
  ofstream ofs_undetected(log_undetected, ios::out);
  ofstream ofs_multiface(log_multiface, ios::out);
  
  int num_total_images = image_list.size();
  int cnt_proc = 0;
  
  int cnt_undetected = 0;
  int cnt_multiface = 0;
  
  long time_start = cv::getTickCount();
  long time_end = 0;
  double time_total = 0.0;
  double time_avg = 0.0;
  
  long time_start_per = cv::getTickCount();
  long time_end_per = 0;
  double time_total_per = 0.0;
  double time_avg_per = 0.0;
  for (vector<string>::iterator iter=image_list.begin(); iter!=image_list.end(); iter++)
  {
    ++cnt_proc;
    
    cv::Mat img_src = cv::imread(*iter);
    cv::Mat img_gray;

    if (img_src.channels() != 1)
      cv::cvtColor(img_src, img_gray, cv::COLOR_BGR2GRAY);
    else
      img_gray = img_src;
    
    seeta::ImageData img_data;
    img_data.data = img_gray.data;
    img_data.width = img_gray.cols;
    img_data.height = img_gray.rows;
    img_data.num_channels = 1;
    
    // detect facial region
    vector<seeta::FaceInfo> faces = detector.Detect(img_data);
    
    int num_face = faces.size();
    
    if (num_face == 0)
    {
      //string path_new_undetected = ChangeDir(*iter, dir_dst_undetected);
      //CopyFile(*iter, path_new_undetected);
      
      ofs_undetected << *iter << endl;
      ++cnt_undetected;
      
      //cout << TC_BOLDRED << "[Undetected]  " << *iter << TC_RESET << endl;
      continue;
    }
    
    ofs_detected << *iter << "@";
    if (num_face > 1)
    {
      ofs_multiface << *iter << "@";
      ++cnt_multiface;
    }

    int index_selected = 0;
    
    ///int temp_max_size = -1; // ERROR: LFW Michael_Schumacher_0015
    int temp_min_dist = 10000;
    int img_w = img_src.cols;
    int img_h = img_src.rows;
    
    for (int cnt_face=0; cnt_face<num_face; ++cnt_face)
    {
      // draw facial region boundary
      //cv::Rect face_rect;
      //face_rect.x = faces[cnt_face].bbox.x;
      //face_rect.y = faces[cnt_face].bbox.y;
      //face_rect.width = faces[cnt_face].bbox.width;
      //face_rect.height = faces[cnt_face].bbox.height;
      //cv::rectangle(img_src, face_rect, cv::Scalar(0,255,0), 2, 8, 0);
      
      ///int temp_size = faces[cnt_face].bbox.width * faces[cnt_face].bbox.height;
      ///if (temp_size > temp_max_size)
      ///{
      ///  temp_max_size = temp_size;
      ///  index_selected = cnt_face;
      ///}
      
      int center_x = faces[cnt_face].bbox.x + faces[cnt_face].bbox.width/2;
      int center_y = faces[cnt_face].bbox.y + faces[cnt_face].bbox.height/2;
      int temp_dist = abs(center_x-img_w/2) + abs(center_y-img_h/2);
      if (temp_dist < temp_min_dist)
      {
        temp_min_dist = temp_dist;
        index_selected = cnt_face;
      }
      
      // detect facial points
      seeta::FacialLandmark points[5];
      point_detector.PointDetectLandmarks(img_data, faces[cnt_face], points);
      for (int cnt_point=0; cnt_point<5; ++cnt_point)
      {
        int point_x = RoundCalc(points[cnt_point].x);
        int point_y = RoundCalc(points[cnt_point].y);
        if (num_face > 1)
        {
          ofs_multiface << point_x << " " << point_y << " ";
        }
        
        // draw facial points
        //cv::Point center = cv::Point(point_x, point_y);
        //cv::circle(img_src, center, 2, cv::Scalar(0,255,0), -1, 8, 0);
      }
    }
    if (num_face > 1)
    {
      ofs_multiface << endl;
    }
    
    // detect facial points
    seeta::FacialLandmark points[5];
    point_detector.PointDetectLandmarks(img_data, faces[index_selected], points);
    for (int cnt_point=0; cnt_point<5; ++cnt_point)
    {
      int point_x = RoundCalc(points[cnt_point].x);
      int point_y = RoundCalc(points[cnt_point].y);
      ofs_detected << point_x << " " << point_y << " ";
    }
    ofs_detected << endl;
    
    // save painted image
    //string path_new_detected = ChangeDir(*iter, dir_dst_detected);
    //cv::imwrite(path_new_detected, img_src);
	  
	  if (cnt_proc % 100 == 0)
    {
      time_end_per = cv::getTickCount();
      time_avg_per = ((time_end_per-time_start_per) / cv::getTickFrequency()) / 100;
      
      cout << "[Processed]   " << cnt_proc << "/" << num_total_images 
           << "   " << time_avg_per << endl;
      
      time_start_per = cv::getTickCount();
    }
  }
  
  time_end = cv::getTickCount();
  time_total = (time_end-time_start) / cv::getTickFrequency();
  time_avg = time_total / num_total_images;
  cout << endl << TC_BOLDGREEN << "[Average Time]   " 
       << time_avg << TC_RESET << endl << endl;
  
  cout << TC_BOLDRED << "[Undetected]   " << cnt_undetected << TC_RESET  << endl;
  cout << TC_BOLDRED << "[MultiFace]    " << cnt_multiface << TC_RESET  << endl;
  
  // Close file stream
  ofs_detected.close();
  ofs_undetected.close();
  ofs_multiface.close();
}



