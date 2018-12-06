//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// Copyright (c) 2012 Image Processing Research Group of University Federico II of Naples ('GRIP-UNINA').
// All rights reserved.
// This software should be used, reproduced and modified only for informational and nonprofit purposes.
// 
// By downloading and/or using any of these files, you implicitly agree to all the
// terms of the license, as specified in the document LICENSE.txt
// (included in this package) and online at
// http://www.grip.unina.it/download/LICENSE_OPEN.txt
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*
 * collaborative_means.h
 *
 *  Created on: 29/10/2012
 *      Author: Davide Cozzolino
 */

#ifndef _COLLABORATIVE_MEANS_H_
#define _COLLABORATIVE_MEANS_H_
#include "../utils/buffers.h"

template <typename PixelType>
PixelType collaborative_means( Stack_Buffer<PixelType> &stackT3D, std::vector<PixelType> &dest_dist, int Nb);

#include "collaborative_means.hpp"
#endif
