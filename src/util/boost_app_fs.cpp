/******************************************************************************
 * FileName: boost_app_fs.cpp
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: Use BOOST to deal with the operations related file system
 *****************************************************************************/

#include <iostream>
#include <string>
#include <vector>

#define BOOST_ALL_NO_LIB
#include "libs/system/src/error_code.cpp"               // system
#include "boost/filesystem.hpp"                         // filesystem
#include "libs/filesystem/src/codecvt_error_category.cpp"
#include "libs/filesystem/src/operations.cpp"
#include "libs/filesystem/src/path.cpp"
#include "libs/filesystem/src/path_traits.cpp"
#include "libs/filesystem/src/portability.cpp"
#include "libs/filesystem/src/unique_path.cpp"
#include "libs/filesystem/src/utf8_codecvt_facet.cpp"
#include "libs/filesystem/src/windows_file_codecvt.hpp"
#include "libs/filesystem/src/windows_file_codecvt.cpp"

#include "boost/optional.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/assert.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/math/special_functions/round.hpp"

#include "util/boost_app_common.h"
#include "util/boost_app_fs.h"

using namespace std;

namespace bfs = boost::filesystem;
typedef bfs::recursive_directory_iterator rdir_iterator;


vector<string> GetImageList(string dir_src)
{
  string image_type(".jpg.png.jpeg.JPG.PNG");
  
  vector<string> image_list;
  
  rdir_iterator end;
  for (rdir_iterator pos(dir_src); pos!=end; ++pos)
  {
    if (!bfs::is_directory(*pos))
    {
      string file_extension = pos->path().extension().string();
      if (boost::contains(image_type, file_extension))
      {
        image_list.push_back(pos->path().string());
      }
    }
  }
  
  return image_list;
}

string PathJoin(string path_a, string path_b)
{
  bfs::path bpath_a(path_a);
  bfs::path bpath_b(path_b);
  
  bpath_a /= bpath_b;
  
  return bpath_a.string();
}

void CreateDir(string dir_new)
{
  bfs::path bpath_dir_new(dir_new);
  if (!bfs::exists(bpath_dir_new))
  {
    bfs::create_directories(bpath_dir_new);
  }
}

void CreateDir(bfs::path bpath_dir_new)
{
  if (!bfs::exists(bpath_dir_new))
  {
    bfs::create_directories(bpath_dir_new);
  }
}

string ChangeDir(string file_path, string dir_dst)
{
  bfs::path bpath_file(file_path);
  bfs::path bpath_dir_dst(dir_dst);
  
  //BOOST_ASSERT(bfs::exists(bpath_file) && "File does not exist.");
  //BOOST_ASSERT(bfs::exists(bpath_dir_dst) && "File does not exist.");
  
  bpath_dir_dst /= bpath_file.filename();
  
  return bpath_dir_dst.string();
}

string ChangeDirFine(string file_path, string dir_dst, string mode, int level)
{
  bfs::path bpath_file(file_path);
  bfs::path bpath_dir_dst(dir_dst);
  
  //BOOST_ASSERT(bfs::exists(bpath_file) && "File does not exist.");
  //BOOST_ASSERT(bfs::exists(bpath_dir_dst) && "File does not exist.");

  const string SYMBOL_DOT(".");
  const string SYMBOL_SLASH("/");
  
  vector<string> path_each_level;
  boost::split(path_each_level,
               bpath_file.parent_path().string(),  // remove extension
               boost::is_any_of(SYMBOL_SLASH));

  vector<string> name_ext;
  boost::split(name_ext,
               bpath_file.filename().string(),
               boost::is_any_of(SYMBOL_DOT));
  
  int total_level = path_each_level.size();
  for (int l=level; l>0; --l)
  {
    bpath_dir_dst /= path_each_level[total_level-l];
    CreateDir(bpath_dir_dst);
  }

  string new_path = bpath_dir_dst.string();
  string new_name = SYMBOL_SLASH + name_ext[0] + mode + SYMBOL_DOT + name_ext[1];

  return new_path + new_name;
}

void CopyFile(string file_src, string file_dst)
{
  bfs::path bpath_src(file_src);
  bfs::path bpath_dst(file_dst);
  
  //BOOST_ASSERT(bfs::exists(bpath_src) && "File does not exist.");
  //BOOST_ASSERT(!bfs::exists(bpath_dst) && "File already exists.");
  
  if (bfs::exists(bpath_src) && !bfs::exists(bpath_dst))
  {
    bfs::copy_file(bpath_src, bpath_dst);
  }
}

bool IsExists(string path)
{
  bfs::path bpath(path);
  
  return bfs::exists(bpath);
}

string GetCurrentFolderPath()
{
  string current_folder = bfs::initial_path<bfs::path>().string();
  
  return current_folder;
}



