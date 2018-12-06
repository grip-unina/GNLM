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
 * collaborative_means.hpp
 *
 *  Created on: 29/10/2012
 *  Update  on: 27/10/2018
 *      Author: Davide cozzolino
 */


#ifndef _COLLABORATIVE_MEANS_HPP_
#define _COLLABORATIVE_MEANS_HPP_
#include "collaborative_means.h"
#include <assert.h>
#include "win2D.h"

template <typename PixelType>
PixelType collaborative_means( Stack_Buffer<PixelType> &stackT3D, std::vector<PixelType> &dists, PixelType filter_parameter, int Nb) {

	// dimensioni degli stack
	int Nr = stackT3D.rows();
	int Nc = stackT3D.cols();
	PixelType w_sum;
	PixelType w;
	PixelType d_min = PixelType(0.0);
	filter_parameter *= filter_parameter;
	cv::Mat_<PixelType> blockSup(Nr,Nc,PixelType());
	
	if (Nb>1) {
		d_min = dists[1];
		for(int k=1; k < Nb; k++ ) {
			if (dists[k]<d_min) {
				d_min = dists[k];
			}
		}
	}
	
	if (d_min>16*filter_parameter) {
		w_sum = 1.0;
		multiply_and_accumulate(stackT3D[0],PixelType(1.0),blockSup);
		mexPrintf("corrected  %d %f %f \n", Nb, dists[0], d_min);
	} else {
		w_sum = 1.0;
		multiply_and_accumulate(stackT3D[0],PixelType(1.0),blockSup);
		for(int k=1; k < Nb; k++ ) {
			w = exp(-(dists[k]-d_min)/filter_parameter);
			multiply_and_accumulate(stackT3D[k],w,blockSup);
			w_sum += w;
		}
	}
	
	blockSup /= w_sum;

	for( int k=0; k < Nb; k++ ) {
		blockSup.copyTo(stackT3D[k]);
	}

	// restituisci somma pesi
	return w_sum;
}



#endif
