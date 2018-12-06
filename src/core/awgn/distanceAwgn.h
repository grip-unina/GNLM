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
 * ditanceAwgn.h
 *
 *  Created on: 01/03/2018
 *      Author: Davide Cozzolino
 *
 */
#ifndef _DISTANCEAWGN_H_
#define _DISTANCEAWGN_H_
#include <vector>
#include "../block_matching.h"

template <typename Type>
class DistanceAwgn
{
 public :
	typedef Type DistanceType;
	const int max_matched;		 // numero massimo di blocchi da selezionare
	const Type max_distance;	 // distanza massima tra 2 blocchi

	DistanceAwgn(BlockMatchOptions<Type> opt) :
		max_matched(opt.max_matched), max_distance(opt.max_distance) {}

	inline Type computeDistance( const cv::Mat_<Type> &src1, const cv::Mat_<Type> &src2, Type sup_distance) const;

	inline Type computeDistance2( const cv::Mat_<Type> &srcY, const cv::Mat_<Type> &srcZ, const cv::Mat_<Type> &refY, const cv::Mat_<Type> &refZ, Type sup_distance) const;

	inline DistanceType getMaxMatched() const;

	inline DistanceType getMaxMatched2() const;

};

#include "distanceAwgn.hpp"
#endif
