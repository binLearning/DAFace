/******************************************************************************
 * FileName: boost_app_filesystem.h
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: Use BOOST to deal with the operations related file system
 *****************************************************************************/

#ifndef _BOOST_APP_FS_H_
#define _BOOST_APP_FS_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

string ChangeDir(string file_path, string dir_dst);
string ChangeDirFine(string file_path, string dir_dst, 
                     string mode = string(""), int level = 1);
void CopyFile(string file_src, string file_dst);
void CreateDir(string dir_new);
vector<string> GetImageList(string dir_src);
string PathJoin(string path_a, string path_b);
bool IsExists(string path);
string GetCurrentFolderPath();

#endif

