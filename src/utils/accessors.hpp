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
 * accessors.hpp
 *
 *  Created on: 12/apr/2010
 *      Author: Giovanni Chierchia
 */

#ifndef _ACCESSORS_HPP_
#define _ACCESSORS_HPP_
#include "accessors.h"

template <typename Type>
Block_Accessor<Type>::Block_Accessor( cv::Mat_<Type> &source, int block_rows, int block_cols, int row_step, int col_step )
	: src(source), block_rows_(block_rows), block_cols_(block_cols)
{
	assert( src.rows > 0 && src.cols > 0 && block_rows > 0 && block_cols > 0 && row_step > 0 && col_step > 0 );
	assert( src.rows >= block_rows && src.cols >= block_cols );

	// calcola gli indici di 'inizio riga'
	std::vector<int> start_rows;
	for( int idx=0; idx < src.rows-block_rows+1; idx += row_step ) {
		start_rows.push_back( idx );
	}
	assert( start_rows.back() == src.rows-block_rows && "I blocchi devono coprire tutte le righe della matrice" );

	// calcola gli indici di 'inizio colonna'
	std::vector<int> start_cols;
	for( int idx=0; idx < src.cols-block_cols+1; idx += col_step ) {
		start_cols.push_back( idx );
	}
	assert( start_cols.back() == src.cols-block_cols && "I blocchi devono coprire tutte le colonne della matrice" );

	// inizializza le dimensioni della cache
	rows_ = start_rows.size();
	cols_ = start_cols.size();
	cache.resize( rows_ * cols_ );
	row_ptr.resize( rows_ );

	// riempi la cache con i riferimenti dei blocchi
	for(int i=0; i < rows_; i++) {
		row_ptr[i] = i * cols_;

		for(int j=0; j < cols_; j++) {
			cv::Range row_range( start_rows[i], start_rows[i] + block_rows_ );
			cv::Range col_range( start_cols[j], start_cols[j] + block_cols_ );
			cache[row_ptr[i] + j] = src( row_range, col_range );
		}
	}
}

#endif /* _ACCESSORS_HPP_ */
