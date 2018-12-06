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
 * stripe.hpp
 *
 *  Created on: 12/apr/2010
 *      Author: Giovanni Chierchia
 */

#ifndef _STRIPE_HPP_
#define _STRIPE_HPP_
#include "stripe.h"

template <typename BlockAccessor, typename BlockFunction>
Stripe<BlockAccessor,BlockFunction>::Stripe(const BlockAccessor &accessor, const BlockFunction &function, int search_diameter)
	: src(accessor), modify_and_copy(function), block_rows_(accessor.block_rows()),block_cols_(accessor.block_cols()), buffer( search_diameter, accessor.cols(), accessor.block_rows(), accessor.block_cols() )
{
	assert( search_diameter > 0 && search_diameter % 2 != 0 );
	assert( src.rows() >= search_diameter );

	// inizializza i parametri della striscia
	radius = (search_diameter - 1) / 2;
	start_index  = 0;
	middle_index = 0;
	finish_index = 0;
}

template <typename BlockAccessor, typename BlockFunction>
Stripe<BlockAccessor,BlockFunction>::Stripe(const BlockAccessor &accessor, const BlockFunction &function, int search_diameter, int out_block_rows, int out_block_cols)
	: src(accessor), modify_and_copy(function), block_rows_(out_block_rows),block_cols_(out_block_cols), buffer( search_diameter, accessor.cols(), out_block_rows, out_block_cols )
{
	assert( search_diameter > 0 && search_diameter % 2 != 0 );
	assert( src.rows() >= search_diameter );

	// inizializza i parametri della striscia
	radius = (search_diameter - 1) / 2;
	start_index  = 0;
	middle_index = 0;
	finish_index = 0;
}

template <typename BlockAccessor, typename BlockFunction>
void Stripe<BlockAccessor,BlockFunction>::move_forward(int new_middle)
{
	assert( new_middle >= 0 && new_middle < src.rows() );
	assert( new_middle - middle_index >= 0 );

	// calcola i nuovi indici di inizio e fine striscia
	int new_start  = std::max( new_middle - radius, 0);
	int new_finish = std::min( new_middle + radius + 1, src.rows() );

	// se "inizio-striscia" è cambiato, rimuovi i blocchi di testa
	int olds = new_start - start_index;
	if( 0 < olds && olds < buffer.rows() ) {
		buffer.rotate(olds);
	}

	// se "fine-striscia" è cambiato, aggiungi i blocchi in coda
	int start = std::max( finish_index, new_start );
	for(int i=start; i < new_finish; i++) {
		for(int j=0; j < src.cols(); j++) {
			modify_and_copy( src(i,j), buffer(i-new_start,j) );
		}
	}

	// aggiorna gli indici della striscia
	start_index  = new_start;
	middle_index = new_middle;
	finish_index = new_finish;
}

#endif
