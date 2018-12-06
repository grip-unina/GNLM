//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// Copyright (c) 2018 Image Processing Research Group of University Federico II of Naples ('GRIP-UNINA').
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
 * distanceSar_int.hpp
 *
 *  Created on: 27/10/2018
 *      Author: Davide Cozzolino
 *
 *  Questo block_matching lavora sull'immagini intensita'
 */
#ifndef _DISTANCESAR_INT_SUM_HPP_
#define _DISTANCESAR_INT_SUM_HPP_

#include <limits>
#include "../block_matching.h"
#include <assert.h>
	
template <typename Type>
class DistanceSar_int_sum
{
 public :
	typedef Type DistanceType;
	const int max_matched;		 // numero massimo di blocchi da selezionare
	const Type max_distance;	 // distanza massima tra 2 blocchi
	


	DistanceSar_int_sum(BlockMatchOptions<Type> opt, double L=1) :
		max_matched(opt.max_matched),
        max_distance(opt.max_distance) {}

	inline Type computeDistance( const cv::Mat_<Type> &src1, const cv::Mat_<Type> &src2, Type sup_distance) const {
		assert( src1.rows == src2.rows && src1.cols == src2.cols );
		Type dist = 0;
		Type sump;
		const Type *row1;
		const Type *row2;

		/* accumula la SSD */
		for(int i=0; i < src1.rows; i++) {
			// puntatori della riga i-esima
			row1 = src1[i];
			row2 = src2[i];

			for(int j=0; j<src1.cols; j++) {
				if (row1[j]!=row2[j]) {
					sump = (row1[j]+row2[j]);

					dist += std::log(sump*sump/(4*row2[j]*row1[j]))/2.0;
				}
			}
		}

		return dist;
	}
    
	inline DistanceType getMaxMatched() const {
		return max_distance;
	}


};

#endif
