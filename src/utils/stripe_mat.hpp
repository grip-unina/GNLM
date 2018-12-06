//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// Copyright (c) 2010 Image Processing Research Group of University Federico II of Naples ('GRIP-UNINA').
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
 * stripe_mat.hpp
 *
 *  Created on: 12/apr/2010
 *      Author: Giovanni Chierchia
 */

#ifndef _STRIPE_MAT_HPP_
#define _STRIPE_MAT_HPP_
#include "stripe_mat.h"


template <typename SrcPixelType, typename PixelFunction>
StripeMtxOp<SrcPixelType, PixelFunction>::StripeMtxOp(const cv::Mat_<SrcPixelType> &source,const PixelFunction &function, int search_diameter, int block_rows, int block_cols)
 	 : src(source),op(function),
 	   rows_(src.rows-block_rows+1), cols_(src.cols-block_cols+1),
 	   block_rows_(block_rows),block_cols_(block_cols),
 	   start_index(0), middle_index(0),
 	   finish_index(0), finish_index_block(0),
 	   mod_buffer(search_diameter+block_rows), head_buffer(0),
 	   buffer( mod_buffer+block_rows-1, src.cols),
 	   cache(buffer,block_rows,block_cols),
 	   radius((search_diameter-1)/2) {
   	assert( src.rows > 0 && src.cols > 0 && block_rows > 0 && block_cols > 0 );
   	assert( src.rows >= block_rows && src.cols >= block_cols );
   	assert( search_diameter > 0 && (search_diameter % 2 != 0) );
   	assert( rows_ >= search_diameter );
}

template <typename SrcPixelType, typename PixelFunction>
void StripeMtxOp<SrcPixelType, PixelFunction>::move_forward(int new_middle)
{
	assert( new_middle >= 0 && new_middle < rows_ );
	assert( new_middle - middle_index >= 0 );

	// calcola i nuovi indici di inizio e fine striscia
	int new_start  = std::max( new_middle - radius, 0);
	finish_index_block = std::min( new_middle + radius + 1, rows_ );
	int new_finish = finish_index_block+block_rows_-1;

	// se "inizio-striscia" è cambiato, rimuovi i blocchi di testa
	head_buffer += (new_start - start_index);
	head_buffer = head_buffer%mod_buffer;

	// se "fine-striscia" è cambiato, aggiungi i blocchi in coda
	int start = std::max( finish_index, new_start );

	for(int i=start, j=(start-new_start+head_buffer)%mod_buffer;
			i < new_finish;
			i++, j=(j+1)%mod_buffer) {
		getSrcData(i,j);//getSrcData(i,buffer[j]);
		if (j<block_rows_-1) {
			std::copy(buffer[j],buffer[j+1] ,buffer[j+mod_buffer]);
		}
	}

	// aggiorna gli indici della striscia
	start_index  = new_start;
	middle_index = new_middle;
	finish_index = new_finish;
}

template <typename SrcPixelType, typename PixelFunction>
void StripeMtxOp<SrcPixelType, PixelFunction>::getSrcData(int row, pixel_type* dest) {
	const SrcPixelType* row_src = src[row];
	for(int i=0;i<this->src.cols;i++) {
		*(dest++) = op(*(row_src++));
	}

}

template <typename SrcPixelType, typename PixelFunction>
void StripeMtxOp<SrcPixelType, PixelFunction>::getSrcData(int row, int row_dest) {
	cv::Range r_s(row,row+1);
	cv::Range r_d(row_dest,row_dest+1);
	cv::Mat_<pixel_type> dest = buffer(r_d,cv::Range::all());
	op(src(r_s,cv::Range::all()),dest);
}

#endif
