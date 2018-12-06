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
#ifndef MEX_CV_4B_HPP
#define MEX_CV_4B_HPP

#include <opencv/cv.h>
#include "mex.h"
#include "matrix.h"
#include <vector>

template <int K>
void mx2cv(const mxArray* mx, cv::Mat_< cv::Vec<float, K> > &mat) {

    // check the type
    if( ! mxIsDouble(mx) ) mexErrMsgTxt("The array must be double"); 
    // get the matrix size
    mwSize D = mxGetNumberOfDimensions(mx);
    if (D<3) mexErrMsgTxt("The array must be 3d");
    const size_t* DD = mxGetDimensions(mx);
    

    mwSize M = DD[0];
    mwSize N = DD[1];
    mwSize KK = DD[2];
    mwSize Slide = M*N;
    if (KK>K) mexErrMsgTxt("The array has too bands");

    //mexPrintf(" %d x %d x %d \n", M,N,DD[2]);
    cv::Vec<float, K> elem;
    for(int k=0; k<K; k++) elem[k] = 0.0;

    // create the matrix
	mat.create(M,N);
    
    //mexPrintf("\n %d %d %d %d \n",mat.elemSize(),mat.step1(),mat.rows,mat.cols );
 	// fill the matrix
  	double* mx_real = mxGetPr(mx);
  	for(int n=0; n < N; n++) {
  		for(int m=0; m < M; m++) {
            for(int k=0; k<KK; k++)
                elem[k] = (float)(*(mx_real+k*Slide));
            mat(m,n) = elem;
            //mexPrintf(" (%d %d) %f x %f x %f x %f \n", m,n, elem[0],elem[1],elem[2],elem[3]);
            mx_real++;
        }
    }
}

mxArray* cv2mx(const cv::Mat_< cv::Vec<float, 4> > mat)
{
	// get the matrix size
    mwSize M = mat.rows;//-3;
    mwSize N = mat.cols; //-3;
	mwSize dims[] = {M, N, 4};
    mwSize Slide2 = M*N;
    mwSize Slide3 = 2*Slide2;
    mwSize Slide4 = 3*Slide2;
    mexPrintf(" %d x %d x %d \n", M,N,4);
    // create the matrix
    mxArray* mx = mxCreateNumericArray(3, dims, mxDOUBLE_CLASS, mxREAL);

    // fill the matrix
    double* mx_real = mxGetPr(mx);
    for(int n=0; n < N; n++)
    	for(int m=0; m < M; m++) {
             cv::Vec<float, 4> elem = mat(m,n); 
             *(mx_real+Slide4) = (double)elem[3]; 
             *(mx_real+Slide3) = (double)elem[2];
             *(mx_real+Slide2) = (double)elem[1];
             *(mx_real       ) = (double)elem[0];
             mx_real++;
             //mexPrintf(" (%d %d) %f x %f x %f x %f \n", m,n, elem[0],elem[1],elem[2],elem[3]);
    }

    return mx;

}


#endif
#endif
