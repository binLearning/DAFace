/******************************************************************************
 * FileName: dafece_norm.h
 * Author:   binLearning
 * Version:  1.0.0
 *
 * Description: align face & resize to unified size
 *****************************************************************************/

#ifndef _DAFACE_NORM_H_
#define _DAFACE_NORM_H_

#include <string>
#include <vector>

void DafaceNorm(std::string file_info, std::string dir_dst, int new_size = 256);

#endif

