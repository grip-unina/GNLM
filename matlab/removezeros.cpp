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

#include <opencv/cv.h>
#include "mex.h"
#include "mex_cv.hpp"
#include <limits>
#include "../src/core/speckle/utility_sar.h"

typedef float Type;

void mexFunction(int nlhs, mxArray *plhs[],
    int nrhs, const mxArray *prhs[]) {
    
	cv::Mat_<Type> image;

	/* check for proper number of arguments */
	if(nrhs!=1) {
		mexErrMsgIdAndTxt("ToolboxUnina:GRIP:removezeros",
						  "One input required.");
	}
	if(nlhs!=1) {
		mexErrMsgIdAndTxt("ToolboxUnina:GRIP:removezeros",
						  "One output required.");
	}
	
	mx2cv(prhs[0], image);
    
    int range_min_p = range_min_pow2(image);
	range_min_p = std::max(range_min_p-2,
						std::min(range_min_p,
								-std::numeric_limits<Type>::digits)
						);
	Type zero_value = pow(2.0,range_min_p);
	remove_zeros(image, zero_value);
	plhs[0] = cv2mx(image);

}
