/******************************************************************************
 * FileName: dafece_crop.h
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: crop facial region image & resize to unified size
 *****************************************************************************/

#ifndef _DAFACE_CROP_H_
#define _DAFACE_CROP_H_

#include <string>
#include <vector>

void DafaceCrop(std::string file_info, std::string dir_dst,
                int new_size = 256);

#endif

