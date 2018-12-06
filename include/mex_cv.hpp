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
 
#ifdef MATLAB_MEX_FILE
#ifndef MEX_CV_HPP
#define MEX_CV_HPP

#include <opencv/cv.hpp>
#include "mex.h"
#include "matrix.h"
#include <vector>


void mx2cv(const mxArray* mx, cv::Mat_<double> &mat) {
    
    // get the matrix size
    mwSize M = mxGetM(mx);
    mwSize N = mxGetN(mx);
    
    // check the type
    if( ! mxIsDouble(mx) ) mexErrMsgTxt("The array must be double");
    
    // create the matrix
    mat.create(M,N);

	// fill the matrix
	double* mx_real = mxGetPr(mx);
	for(int n=0; n < N; n++)
		for(int m=0; m < M; m++)
	{
		mat(m,n) = *(mx_real++);
	}


}

void mx2cv(const mxArray* mx, cv::Mat_<float> &mat) {

    // get the matrix size
    mwSize M = mxGetM(mx);
    mwSize N = mxGetN(mx);

    // check the type
    if( ! mxIsDouble(mx) ) mexErrMsgTxt("The array must be double");

    // create the matrix
	mat.create(M,N);

	// fill the matrix
	double* mx_real = mxGetPr(mx);
	for(int n=0; n < N; n++)
		for(int m=0; m < M; m++)
	{
		mat(m,n) = *(mx_real++);
	}

}


void mx2cv(const mxArray* mx, cv::Mat_<bool> &mat) {

    // get the matrix size
    mwSize M = mxGetM(mx);
    mwSize N = mxGetN(mx);
    mat.create(M,N);
    
    if (mxIsLogical(mx)) {
        // fill the matrix
        bool* mx_real = (bool*)mxGetData(mx);
        for(int n=0; n < N; n++)
            for(int m=0; m < M; m++)
                mat(m,n) =  (*(mx_real++));
    } else {
        // check the type
        if( ! mxIsDouble(mx) ) mexErrMsgTxt("The array must be double");
        // fill the matrix
        double* mx_real = mxGetPr(mx);
        for(int n=0; n < N; n++)
            for(int m=0; m < M; m++)
                mat(m,n) =  (bool)(*(mx_real++));
    }

}


mxArray* cv2mx(const cv::Mat_<double> mat)
{
	// get the matrix size
	mwSize M = mat.rows;
	mwSize N = mat.cols;

    // create the matrix
    mxArray* mx = mxCreateDoubleMatrix(M, N, mxREAL);

    // fill the matrix
    double* mx_real = mxGetPr(mx);
    for(int n=0; n < N; n++)
    	for(int m=0; m < M; m++)
    {
    		 *(mx_real++) = mat(m,n);
    }

    return mx;
    
}

mxArray* cv2mx(const cv::Mat_<float> mat)
{
	// get the matrix size
	mwSize M = mat.rows;
	mwSize N = mat.cols;

    // create the matrix
    mxArray* mx = mxCreateDoubleMatrix(M, N, mxREAL);

    // fill the matrix
    double* mx_real = mxGetPr(mx);
    for(int n=0; n < N; n++)
    	for(int m=0; m < M; m++)
    {
    		 *(mx_real++) = mat(m,n);
    }

    return mx;

}

mxArray* cv2mx(const cv::Mat_<int> mat)
{
	// get the matrix size
	mwSize M = mat.rows;
	mwSize N = mat.cols;

    // create the matrix
    mxArray* mx = mxCreateDoubleMatrix(M, N, mxREAL);

    // fill the matrix
    double* mx_real = mxGetPr(mx);
    for(int n=0; n < N; n++)
    	for(int m=0; m < M; m++)
    {
    		 *(mx_real++) = (double) mat(m,n);
    }

    return mx;

}


mxArray* cv2mx(const cv::Mat_<bool> mat)
{
	// get the matrix size
	mwSize M = mat.rows;
	mwSize N = mat.cols;

    // create the matrix
    mxArray* mx = mxCreateNumericMatrix(M, N, mxLOGICAL_CLASS, mxREAL);

    // fill the matrix
    bool* mx_real = (bool*)mxGetData(mx);
    for(int n=0; n < N; n++)
    	for(int m=0; m < M; m++)
    {
    		 *(mx_real++) = mat(m,n);
    }

    return mx;

}

#endif
#endif
