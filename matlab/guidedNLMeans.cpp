// GNLM - Guided Non-Local Means
// Date released 10/12/2018, version BETA.
// Code for the guided denoising of a SAR image corrupted
// by multiplicative speckle noise with the technique described in 
// "Guided patch-wise nonlocal SAR despeckling", 
// written by Sergio Vitale, Davide Cozzolino, Giuseppe Scarpa, Luisa Verdoliva and Giovanni Poggi, 
// Submitted, 2018.
// Please refer to this papers for a more detailed description of the algorithm.
//
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

#include <opencv/cv.h>
#include "mex.h"
#include "mex_cv_4b.hpp"
#include "mex_cv.hpp"
#include "../src/GNLM.hpp"

#ifndef GUIDA_NUM_BANDS
#define GUIDA_NUM_BANDS 4
#endif

static const char tool_id[] = "ToolboxUnina:GRIP:GuidedNLMeans";



template<typename T>
void mx2GuidedNLMeansProfile(const mxArray* mx, GuidedNLMeansProfile<T> &opt) {
     if (   mxIsEmpty(mx)               ) mexErrMsgIdAndTxt(tool_id, "The parameters are not definited");
     if ( ! mxGetField(mx,0,"B_dim")    ) mexErrMsgIdAndTxt(tool_id, "The parameter 'B_dim' is not definited");
     if ( ! mxGetField(mx,0,"S_dim")    ) mexErrMsgIdAndTxt(tool_id, "The parameter 'S_dim' is not definited");
     if ( ! mxGetField(mx,0,"SA_dim")   ) mexErrMsgIdAndTxt(tool_id, "The parameter 'SA_dim' is not definited");
     if ( ! mxGetField(mx,0,"N_step")   ) mexErrMsgIdAndTxt(tool_id, "The parameter 'N_step' is not definited");
     if ( ! mxGetField(mx,0,"tau_match")) mexErrMsgIdAndTxt(tool_id, "The parameter 'tau_match' is not definited");
     if ( ! mxGetField(mx,0,"beta")     ) mexErrMsgIdAndTxt(tool_id, "The parameter 'beta' is not definited");
     if ( ! mxGetField(mx,0,"alpha")    ) mexErrMsgIdAndTxt(tool_id, "The parameter 'alpha' is not definited");
     if ( ! mxGetField(mx,0,"lambda1")  ) mexErrMsgIdAndTxt(tool_id, "The parameter 'lambda1' is not definited");
     if ( ! mxGetField(mx,0,"lambda2")  ) mexErrMsgIdAndTxt(tool_id, "The parameter 'lambda2' is not definited");
     if ( ! mxGetField(mx,0,"thDist")   ) mexErrMsgIdAndTxt(tool_id, "The parameter 'thDist' is not definited");
     
     
     int NB      = (int) mxGetScalar( mxGetField(mx,0,"B_dim")    ); // numero di righe/colonne del singolo blocco
     int NS      = (int) mxGetScalar( mxGetField(mx,0,"S_dim")    ); // lunghezza max dello stack
     int NA      = (int) mxGetScalar( mxGetField(mx,0,"SA_dim")    ); // diametro del vicinato (cio�, l'ampiezza della zona di ricerca per il block-matching)
     int Nstep   = (int) mxGetScalar( mxGetField(mx,0,"N_step") ); // distanza tra i "reference block"
     T tau_match = (T)   mxGetScalar( mxGetField(mx,0,"tau_match") ); //parametro per la distanza massima tra 2 blocchi
     T beta      = (T)   mxGetScalar( mxGetField(mx,0,"beta") );  // [AGGREGATION] parametro della finestra di Kaiser 
     T alpha     = (T)   mxGetScalar( mxGetField(mx,0,"alpha") ); 
     T lambda1   = (T)   mxGetScalar( mxGetField(mx,0,"lambda1") ); 
     T lambda2   = (T)   mxGetScalar( mxGetField(mx,0,"lambda2") ); 
     T thDist    = (T)   mxGetScalar( mxGetField(mx,0,"thDist") ); 
     
     if (alpha    < 0) mexErrMsgIdAndTxt(tool_id, "The parameter 'alpha' is not set correctly");
     if (alpha    > 1) mexErrMsgIdAndTxt(tool_id, "The parameter 'alpha' is not set correctly");
     if (lambda1  < 0) mexErrMsgIdAndTxt(tool_id, "The parameter 'lambda1' is not set correctly");
     if (lambda2  < 0) mexErrMsgIdAndTxt(tool_id, "The parameter 'lambda2' is not set correctly");
     if (NB       < 2) mexErrMsgIdAndTxt(tool_id, "The parameter 'B_dim' is not set correctly");
     if (NS       < 1) mexErrMsgIdAndTxt(tool_id, "The parameter 'S_dim' is not set correctly");
     if (NA       < 2) mexErrMsgIdAndTxt(tool_id, "The parameter 'SA_dim' is not set correctly");
     if (Nstep    < 1) mexErrMsgIdAndTxt(tool_id, "The parameter 'Nstep' is not set correctly");
     if (tau_match<=0) mexErrMsgIdAndTxt(tool_id, "The parameter 'tau_match' is not set correctly");
     if (beta     <=0) mexErrMsgIdAndTxt(tool_id, "The parameter 'beta' is not set correctly");
     
     opt.config(NB,NS,NA,Nstep, tau_match, beta, alpha, thDist, lambda1, lambda2);
     
}

typedef float PixelType;
typedef cv::Vec<PixelType, GUIDA_NUM_BANDS> PixelGuidaType;

void mexFunction(int nlhs, mxArray *plhs[],
    int nrhs, const mxArray *prhs[]) {
    
	cv::Mat_< PixelType > noisy;
    cv::Mat_< PixelGuidaType > guida;
    cv::Mat_<bool> valClass;
	
    GuidedNLMeansProfile<PixelType> opt;

	/* check for proper number of arguments */
	if (nrhs!=4) mexErrMsgIdAndTxt(tool_id, "Four inputs are required.");
	if (nlhs< 1) mexErrMsgIdAndTxt(tool_id, "At least One output is required.");
	if (nlhs> 2) mexErrMsgIdAndTxt(tool_id, "Max Twe outputs are required.");
	
	mx2cv(prhs[0], noisy);
    mx2cv(prhs[1], guida);
    mx2cv(prhs[2], valClass);
    mx2GuidedNLMeansProfile(prhs[3],opt);
	if (noisy.rows<opt.block_rows) mexErrMsgIdAndTxt(tool_id, "The noisy image is not valid");
    if (noisy.cols<opt.block_cols) mexErrMsgIdAndTxt(tool_id, "The noisy image is not valid");
    
    cv::Mat_<PixelType> denoised( noisy.size() );
    cv::Mat_<PixelType> weights( noisy.size() );
	guided_nlmeans<PixelType, PixelGuidaType, DistanceSar_int_sum<PixelType>, DistanceAwgnVec<PixelType, GUIDA_NUM_BANDS > >(
            noisy, guida, valClass, denoised, weights, opt);

	plhs[0] = cv2mx(denoised);
	if (nlhs>1) plhs[1] = cv2mx(weights);

}
