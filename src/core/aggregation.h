//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// Copyright (c) 2010 Image Processing Research Group of University Federico II of Naples ('GRIP-UNINA').
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
 * aggregation.h
 *
 *  Created on: 08/apr/2010
 *      Author: Giovanni Chierchia
 */

#ifndef _AGGREGATION_H_
#define _AGGREGATION_H_
#include <vector>
#include "../utils/buffers.h"
#include "../utils/accessors.h"
#include "win2D.h"

template <typename PixelType>
struct AggregationOptions
{
	Win2D<PixelType> win2D;	// finestra 2D
};

template <typename BlockAccessor, typename ScaleType>
void aggregation( const Stack_Buffer<typename BlockAccessor::pixel_type> &stack, const std::vector< std::pair<int,int> > &matched, const ScaleType &scale, Neighborhood_Rect_Accessor<BlockAccessor> &image, Neighborhood_Rect_Accessor<BlockAccessor> &weight, const AggregationOptions<typename BlockAccessor::pixel_type> &opt );

#include "aggregation.hpp"
#endif
