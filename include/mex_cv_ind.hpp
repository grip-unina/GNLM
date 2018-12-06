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
#ifndef MEX_CV_IND_HPP
#define MEX_CV_IND_HPP

#include <utility>
#include <opencv/cv.hpp>
#include "mex.h"
#include "matrix.h"
#include <vector>
#include <map>

#ifndef MATCHES_TYPE
#define MATCHES_TYPE
typedef std::vector< std::pair<int,int> >  matchesType;
typedef std::map< std::pair<int,int>, matchesType > matchesListType;
#endif

void mx2cv(const mxArray* mx, matchesListType &matches) {
    // check the type
    if (mxGetClassID(mx)!=mxCELL_CLASS) mexErrMsgTxt("The array must be a class");
    
    // get the matrix size
    mwSize M = mxGetM(mx);
    mwSize N = mxGetN(mx);
    
    // create the matrix
    matches.clear();
	
	// fill the matrix
	mwIndex index = 0;
	for(int n=0; n < N; n++)
		for(int m=0; m < M; m++) {
 			mxArray* A  = mxGetCell(mx, index++);
  			if (A != NULL && (mxGetClassID(A)==mxINT32_CLASS) ) {
  				mwSize K = mxGetN(A);
                matchesType vet(K);
    			int* mx_data = (int*) mxGetData(A);
                for(int k=0; k<K; k++) {
                      int x = *(mx_data++); int y = *(mx_data++);
                      vet.at(k) = std::make_pair(x,y);
                 }
                 matches.insert(std::make_pair(std::make_pair(m,n),vet));
  			}
	}
}

mxArray* cv2mx(matchesListType &matches, mwSize M, mwSize N) {
    // create the matrix
    mxArray* mx = mxCreateCellMatrix(M, N);

    // show content:
    for (matchesListType::iterator it = matches.begin(); it!=matches.end(); ++it) {
        matchesType& vet = it->second;
        mwSize K = vet.size();
        if (K>0) {
            int index  = ((it->first).second)*M+((it->first).first);
            mxArray* A  = mxCreateNumericMatrix(2,K,mxINT32_CLASS,mxREAL);
            int* mx_data = (int*) mxGetData(A);
            for(int k=0; k < K; k++) {
                 int x = vet[k].first; int y = vet[k].second;
                *(mx_data++) = vet[k].first;
                *(mx_data++) = vet[k].second;
            }
            mxSetCell(mx,index,A);
        }
    }
    
    return mx;
    
}

#endif
#endif
