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
 * distanceAwgn.hpp
 *
 *  Created on: 01/03/2018
 *      Author: Davide Cozzolino
 *
 */
#ifndef _DISTANCEAWGN_HPP_
#define _DISTANCEAWGN_HPP_

#include <limits>
#include "distanceAwgn.h"

#include <assert.h>
	
template <typename Type>
inline Type DistanceAwgn<Type>::computeDistance( const cv::Mat_<Type> &src1, const cv::Mat_<Type> &src2, Type sup_distance) const {
	assert( src1.rows == src2.rows && src1.cols == src2.cols );
	Type dist  = Type();
	Type diff;
	const Type *row1;
	const Type *row2;

	/* accumula la SSD */
	for(int i=0; i < src1.rows; i++) {
		// puntatori della riga i-esima
		row1 = src1[i];
		row2 = src2[i];

		for(int j=0; j<src1.cols; j++) {
			diff = row1[j]-row2[j];

			dist += diff*diff;
			if (dist>sup_distance) return dist; //PDE-classic
		}
	}

	return dist;
}

template <typename Type>
inline Type DistanceAwgn<Type>::computeDistance2( const cv::Mat_<Type> &srcY, const cv::Mat_<Type> &srcZ, const cv::Mat_<Type> &refY, const cv::Mat_<Type> &refZ, Type sup_distance) const {
	assert( srcY.rows == refY.rows && srcY.cols == refY.cols );
	assert( srcZ.rows == refZ.rows && srcZ.cols == refZ.cols );

	const Type *row1;
	const Type *row2;
	Type diff;
	Type dist = Type();

	//calcolo distanza 1
	for(int i=0; i < srcY.rows; i++) {
		// puntatori della riga i-esima
		row1 = srcY[i];
		row2 = refY[i];

		for(int j=0; j<srcY.cols; j++) {
			diff = row1[j]-row2[j];
			dist += diff*diff;
			if (dist>sup_distance) return dist; //PDE-classic
		}
	}


	//calcolo distanza 2
	for(int i=0; i < srcZ.rows; i++) {
		// puntatori della riga i-esima
		row1 = srcZ[i];
		row2 = refZ[i];

		for(int j=0; j<srcZ.cols; j++) {
			if (row1[j]!=row2[j]) {
				diff = row1[j]-row2[j];
				dist += diff*diff;
				if (dist>sup_distance) return dist; //PDE-classic
			}
		}
	}

	return dist;
}

template <typename Type>
inline typename DistanceAwgn<Type>::DistanceType DistanceAwgn<Type>::getMaxMatched() const {
	return max_distance*max_distance;
}

template <typename Type>
inline typename DistanceAwgn<Type>::DistanceType DistanceAwgn<Type>::getMaxMatched2() const {
	return max_distance*max_distance;
}



#endif
