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
 * utility_sar.h
 *
 *  Created on: 18/05/2012
 *      Author: Davide Cozzolino
 *
 */
#ifndef _UTILITY_SAR_H_
#define _UTILITY_SAR_H_

#include <cmath>
#include <opencv/cv.h>

template <typename PixelType>
void sqrt_int(int L, cv::Mat_<PixelType> &out) {
	out = PixelType();
	int d = 2*L;
	double b = 1.0/(sqrt((double)d));
	cv::Mat_<PixelType> tmp( out.size() );
	for(int i=0;i<d;i++) {
		cv::randn( tmp, cv::Scalar::all(0), cv::Scalar::all(b) );
		cv::accumulateSquare(tmp,out);
	}
	cv::sqrt(out,out);
}

template <typename PixelType>
void clip(cv::Mat_<PixelType> &y, PixelType min_value, PixelType max_value, PixelType range) {
	cv::Mat_<PixelType> tmp1( y.size() );
	cv::randu( tmp1, cv::Scalar::all(min_value), cv::Scalar::all(range+min_value) );

	cv::Mat_<PixelType> tmp2( y.size() );
	cv::randu( tmp2, cv::Scalar::all(max_value-range), cv::Scalar::all(max_value) );

	cv::Mat_<uchar> min_flag( y.size() );
	cv::compare(y , min_value, min_flag, cv::CMP_LT);

	cv::Mat_<uchar> max_flag( y.size() );
	cv::compare(y , max_value, max_flag, cv::CMP_GT);

	accumulateWeighted( tmp1, y, 1, min_flag );
	accumulateWeighted( tmp2, y, 1, max_flag );

}

template <typename PixelType>
void correggi(cv::Mat_<PixelType> &y, const cv::Mat_<PixelType> &noisy, double L) {
	cv::Mat_<PixelType> r( y.size() );
	cv::divide(noisy, y, r);
	PixelType soglia;
	if (L<=1)
		soglia = 4;
	else if (L<=2)
		soglia = 3.5;
	else if  (L<5)
		soglia = 3;
	else
		soglia = 2.5;

	cv::Mat_<uchar> flag( y.size() );
	cv::compare(r , soglia, flag, cv::CMP_GT);

	accumulateWeighted( noisy, y, 1, flag );

}

template <typename PixelType>
void remove_zeros(cv::Mat_<PixelType> &img, PixelType zero_value) {
	PixelType* row;
	for(int m=0; m < img.rows; m++) {
		row = img[m];
		for(int n=0; n < img.cols; n++)
			if (row[n]<zero_value) row[n] = zero_value;
	}
}

template <typename PixelType>
PixelType range_max_value(cv::Mat_<PixelType> &y) {
	PixelType range_max = 0;
	for(int n=0; n < y.cols; n++)
			for(int m=0; m < y.rows; m++) {
		if (range_max<y(m,n)) range_max = y(m,n);
	}
	return range_max;
}

template <typename PixelType>
PixelType range_min_value(cv::Mat_<PixelType> &y) {
	PixelType range_min = y(0,0);
	for(int n=0; n < y.cols; n++)
			for(int m=0; m < y.rows; m++) {
		if (y(m,n)>0 && (range_min>y(m,n) || range_min<=0)) range_min = y(m,n);
	}
	return range_min;
}

template <typename PixelType>
int range_min_pow2(const cv::Mat_<PixelType> &y) {
	PixelType range_min = y(0,0);
	const PixelType* row;
	for(int m=0; m < y.rows; m++) {
		row = y[m];
		for(int n=0; n < y.cols; n++) {
			if (row[n]>0 && (range_min>row[n] || range_min<=0)) range_min = row[n];
		}
	}
	return floor(log(range_min)/log(2.0));
}

template <typename PixelType>
std::pair<int,int> range_pow2(const cv::Mat_<PixelType> &y) {
	PixelType range_min = y(0,0);
	PixelType range_max = 0;
	const PixelType* row;
	for(int m=0; m < y.rows; m++) {
		row = y[m];
		for(int n=0; n < y.cols; n++) {
			if (row[n]>0 && (range_min>row[n] || range_min<=0)) range_min = row[n];
			if (range_max<row[n]) range_max = row[n];
		}
	}
	return std::make_pair(floor(log(range_min)/log(2.0)),floor(log(range_max)/log(2.0))+1);
}

template <typename TypeIn, typename TypeOut>
int threshold_Mat(const cv::Mat_<TypeIn> &x, cv::Mat_<TypeOut> &y,TypeIn th, TypeOut downTh, TypeOut upTh ) {
	int count = 0;
	const TypeIn* rowIn;
	TypeOut* rowOut;
	for(int m=0; m < x.rows; m++) {
		rowIn  = x[m];
		rowOut = y[m];
		for(int n=0; n < x.cols; n++) {
			if (rowIn[n]>th)
				rowOut[n] = upTh;
			else {
				rowOut[n] = downTh;
				count++;
			}
		}
	}
	return count;
}

template <typename TypeIn, typename TypeOut>
int threshold2_Mat(const cv::Mat_<TypeIn> &x, cv::Mat_<TypeOut> &y,TypeIn thL,TypeIn thH, TypeOut downTh, TypeOut inTh , TypeOut upTh ) {
	int count = 0;
	const TypeIn* rowIn;
	TypeOut* rowOut;
	for(int m=0; m < x.rows; m++) {
		rowIn  = x[m];
		rowOut = y[m];
		for(int n=0; n < x.cols; n++) {
			if (rowIn[n]>thH)
				rowOut[n] = upTh;
			else if (rowIn[n]>thL) {
				rowOut[n] = inTh;
				count++;
			} else
				rowOut[n] = downTh;
		}
	}
	return count;
}


#endif
