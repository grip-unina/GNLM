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
 * ditanceAwgnVec.h
 *
 *  Created on: 01/03/2018
 *      Author: Davide Cozzolino
 *
 */
#ifndef _DISTANCEAWGNVEC_H_
#define _DISTANCEAWGNVEC_H_
#include <vector>
#include <limits>
#include <assert.h>
#include "../block_matching.h"

template <typename Type, int nc>
class DistanceAwgnVec
{
 public :
	typedef Type DistanceType;
    typedef cv::Vec<Type, nc> ElementType;
	const int max_matched;		 // numero massimo di blocchi da selezionare
	const Type max_distance;	 // distanza massima tra 2 blocchi

	DistanceAwgnVec(BlockMatchOptions<Type> opt) :
		max_matched(opt.max_matched), max_distance(opt.max_distance) {}

	inline Type computeDistance( const cv::Mat_<ElementType> &src1, const cv::Mat_<ElementType> &src2, Type sup_distance) const {
    	assert( src1.rows == src2.rows && src1.cols == src2.cols );
        Type dist = Type();
        Type diff;

        /* accumula la SSD */
        for(int i=0; i < src1.rows; i++) {
            for(int j=0; j<src1.cols; j++) {
                ElementType el1 = src1(i,j);
                ElementType el2 = src2(i,j);
                for(int k=0; k < nc; k++) {
                    diff = el1[k] - el2[k];
                    dist += diff*diff;
                }
                if (dist>sup_distance) return dist; //PDE-classic
            }
        }

        return dist;
    }

	//inline Type computeDistance2( const cv::Mat_<ElementType> &srcY, const cv::Mat_<ElementType> &srcZ, const cv::Mat_<ElementType> &refY, const cv::Mat_<ElementType> &refZ, Type sup_distance) const;

	inline DistanceType getMaxMatched() const {
        return max_distance*max_distance;
    }
    
    inline DistanceType getMaxMatched2() const {
        return max_distance*max_distance;
    }

};


#endif
