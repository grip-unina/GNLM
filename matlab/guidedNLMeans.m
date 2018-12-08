function [ y, w_sum ] = guidedNLMeans( z, numLook, guide, stack_size, sharpness, balance, th_sar, block_size, win_size, stride)
% GNLM - Guided Non-Local Means
% Date released 10/12/2018, version BETA.
% Code for the guided denoising of a SAR image corrupted
% by multiplicative speckle noise with the technique described in 
% "Guided patch-wise nonlocal SAR despeckling", 
% written by Sergio Vitale, Davide Cozzolino, Giuseppe Scarpa, Luisa Verdoliva and Giovanni Poggi, 
% Submitted, 2018.
% Please refer to this papers for a more detailed description of the algorithm.
%
%
%   IMA_FIL = guidedNLMeans(IMA_NSE, L, GUIDE, STACK_SIZE, SHARPNESS, BALANCE, TH_SAR, BLOCK_SIZE, WIN_SIZE, STRIDE)
%
%       ARGUMENT DESCRIPTION:
%               IMA_NSE    - Noisy image (in square root intensity) 
%               L          - Number of looks of the noisy image
%               GUIDE      - Guide image (in [0,255] range)
%               STACK_SIZE - Maximum size of the 3rd dimension of the stack (default 256, for sharpness configuration)
%               SHARPNESS  - Decay parameter, see paper for more details (default 0.002, for sharpness configuration)
%               BALANCE    - Balance parameter, see paper for more details (default 0.15)
%               TH_SAR     - Test threshold, see paper for more details (default 2.0)
%               BLOCK_SIZE - Number of rows/cols of the block (default 8)
%               WIN_SIZE   - Diameter of the search area (default 39)
%               STRIDE     - Dimension of step in sliding window processing (default 3)
%
%       OUTPUT DESCRIPTION:
%               IMA_FIL  - Filtered image (in square root intensity)
%
%       NOTE:
%         for smoothness configuration, set SHARPNESS to 0.004 and STACKSIZE to WIN_SIZE^2
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 
% Copyright (c) 2018 Image Processing Research Group of University Federico II of Naples ('GRIP-UNINA').
% All rights reserved.
% This software should be used, reproduced and modified only for informational and nonprofit purposes.
% 
% By downloading and/or using any of these files, you implicitly agree to all the
% terms of the license, as specified in the document LICENSE.txt
% (included in this package) and online at
% http://www.grip.unina.it/download/LICENSE_OPEN.txt
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	%%% Default parameters
	if nargin<10, stride     =  3;    end;
	if nargin< 9, win_size   = 39;    end;
	if nargin< 8, block_size =  8;    end;
	if nargin< 7, th_sar     = 2.0;   end;
	if nargin< 6, balance    = 0.15;  end;
	if nargin< 5, sharpness  = 0.002; end;
	if nargin< 4, stack_size = 256;   end;
	
    %%% Removal of zeros
    z = removezeros(double(z));
    
    %%% Guida:
    guide = double(guide);
    numBands = size(guide,3);

    %%%% Statistic:
    sigm_sar = block_size * sqrt(0.5*psi(1, numLook) - psi(1, 2*numLook));
    mu_sar   = block_size * block_size * (psi(0,2*numLook)-psi(0,numLook)-log(2));
    mu_guide = block_size * block_size * numBands;
    
    %%%% Parameters:
    opt = struct();
    opt.B_dim     = double(block_size); %% Number of rows/cols of block
    opt.S_dim     = double(stack_size); %% Maximum size of the 3rd dimension of a stack
    opt.SA_dim    = double(  win_size); %% Diameter of search area, must be odd
    opt.N_step    = double(    stride); %% Dimension of step in sliding window processing
    opt.tau_match = inf();              %% Threshold for the block-distance
    opt.beta      = 2.0;                %% Parameter of the 2D Kaiser window used in the reconstruction
    opt.alpha     = 0.0;
    opt.thDist    = sigm_sar * th_sar + mu_sar;
    opt.lambda1   = sharpness*      balance  / mu_sar;
    opt.lambda2   = sharpness*(1.0- balance) / mu_guide;
    
    %%%% Elaboration:
    z_int = z.^2;
    if numBands<=4
		[y_int,w_sum] = guidedNLMeans_b04(z_int, guide, true(size(z)), opt);
	elseif numBands<=8
		[y_int,w_sum] = guidedNLMeans_b08(z_int, guide, true(size(z)), opt);
	elseif numBands<=16
	    [y_int,w_sum] = guidedNLMeans_b16(z_int, guide, true(size(z)), opt);
	else
	    [y_int,w_sum] = guidedNLMeans_b32(z_int, guide, true(size(z)), opt);
	end
	y = sqrt(y_int);
end

