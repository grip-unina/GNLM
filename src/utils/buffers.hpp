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
 * buffers.hpp
 *
 *  Created on: 12/apr/2010
 *      Author: Giovanni Chierchia
 */

#ifndef _BUFFERS_HPP_
#define _BUFFERS_HPP_
#include "buffers.h"

template <typename PixelType>
Block_Buffer<PixelType>::Block_Buffer( int grid_rows, int grid_cols, int block_rows, int block_cols )
	: buffer( grid_rows*block_rows, grid_cols*block_cols ), cache( buffer, block_rows, block_cols )
{
	assert( grid_rows > 0 && grid_cols > 0 && block_rows > 0 && block_cols > 0 );

	// inizializza le dimensioni del buffer
	rows_ = grid_rows;
	cols_ = grid_cols;
	block_rows_ = block_rows;
	block_cols_ = block_cols;
}

template <typename PixelType>
Rotable_Buffer<PixelType>::Rotable_Buffer( int grid_rows, int grid_cols, int block_rows, int block_cols )
	: base_t( grid_rows, grid_cols, block_rows, block_cols )
{
	head = 0;
}

template <typename PixelType>
Stack_Buffer<PixelType>::Stack_Buffer(int rows, int cols, int blks)
	: buffer( 1, blks, rows, cols )
{ }

template <typename PixelType>
template <typename BlockAccessor>
Stack_Buffer<PixelType>::Stack_Buffer( const BlockAccessor &src )
	: buffer( 1, src.rows()*src.cols(), src.block_rows(), src.block_cols() )
{
	// copia tutti i blocchi nel buffer
	int Nr = src.rows(), Nc = src.cols();
	for(int i=0; i<Nr; i++) {
		for(int j=0; j<Nc; j++) {
			src(i,j).copyTo( buffer(0,i*Nc+j) );
		}
	}
}

template <typename PixelType>
template <typename BlockAccessor>
Stack_Buffer<PixelType>::Stack_Buffer( const BlockAccessor &src, const std::vector< std::pair<int,int> > &points, int zero_pad )
	: buffer( 1, points.size()+zero_pad, src.block_rows(), src.block_cols() )
{
/*	// copia i blocchi selezionati nel buffer
	for(size_t i=0; i<points.size(); i++) {
		src(points[i].first, points[i].second).copyTo( buffer(0,i) );
	}

	// zero padding
	for(int i=points.size(); i < blks(); i++) {
		buffer(0,i) = typename BlockAccessor::pixel_type();	// zero
	}*/
	fromBlock( src,points,points.size());
}

template <typename PixelType>
template <typename BlockAccessor>
void Stack_Buffer<PixelType>::fromBlock( const BlockAccessor &src, const std::vector< std::pair<int,int> > &points,int length)
{
	assert( length <= buffer.cols() &&
			buffer.block_rows() == src.block_rows() &&
			buffer.block_cols() == src.block_cols());

	// copia i blocchi selezionati nel buffer
	for(int i=0; i<length; i++) {
		src(points[i].first, points[i].second).copyTo( buffer(0,i) );
	}

	// zero padding
	if (length< blks()) for(int i=length; i < blks(); i++) {
		buffer(0,i) = typename BlockAccessor::pixel_type();	// zero
	}
}

#endif /* _BUFFERS_HPP_ */
