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
 * stepper.h
 *
 *  Created on: 17/mar/2010
 *      Author: Giovanni Chierchia
 */

#ifndef STEPPER_H_
#define STEPPER_H_

#include <cassert>
#include <vector>

class Stepper
{
	/* indici di posizione */
	unsigned int row_ptr;	// indice (relativo) di riga
	unsigned int col_ptr;	// indice (relativo) di colonna

	/* indici assoluti (riferiti all'immagine) dei "reference block" */
	std::vector<int> row_index;  // indici di riga
	std::vector<int> col_index;  // indici di colonna

	/* num. di "ref. block" su righe e colonne */
	unsigned int rows;
	unsigned int cols;

 public:

	/*
	 * PARAMETRI D'INGRESSO:
	 *   1) image_rows = num. di righe dell'immagine
	 *   2) image_cols = num. di colonne dell'immagine
	 *   3) block_rows = num. di righe dei blocchi
	 *   4) block_cols = num. di colonne dei blocchi
	 *   5) step     = distanza tra i "ref. block"
	 */
	Stepper( int image_rows, int image_cols, int block_rows, int block_cols, int step )
	{
		assert( block_rows > 0 && block_cols > 0 );
		assert( image_rows >= block_rows && image_cols >= block_cols );
		assert( step > 0 );

		// inizializza gli indici di riga dei "ref. block"
		int last_row = image_rows - block_rows;
		for(int i=0; i < last_row; i += step) {
			row_index.push_back(i);
		}
		row_index.push_back( last_row );

		// inizializza gli indici di colonna dei "ref. block"
		int last_col = image_cols - block_cols;
		for(int j=0; j < last_col; j += step) {
			col_index.push_back(j);
		}
		col_index.push_back( last_col );

		// inizializza le dimensioni
		rows = row_index.size();
		cols = col_index.size();
	}

	int begin_row() {
		return row_index[row_ptr = 0];
	}

	bool has_row() const {
		return row_ptr < rows;
	}

	int next_row() {
		return row_index[++row_ptr % rows];
	}

	int begin_col() {
		return col_index[col_ptr = 0];
	}

	bool has_col() const {
		return col_ptr < cols;
	}

	int next_col() {
		return col_index[++col_ptr % cols];
	}

	/* NON CANCELLARE */
//	row_ptr = -1;
//	col_ptr = col_index.size() - 1;
//
//	bool next()
//	{
//		col++;
//		if( col == col_index.size() )
//		{
//			col = 0;
//			row++;
//			if( row == row_index.size() )
//				return false;
//			else
//				advance( row_index[row] );
//		}
//		change_position( row_index[row], col_index[col] );
//		return true;
//	}
//	void update_horizontal() {
//		left_bound  = (current_col() - radius) >= 0 ? -radius : -current_col();
//		right_bound = (current_col() + radius) <= col_index.back() ? radius : col_index.back()-current_col();
//	}
//
//	void update_vertical() {
//		upper_bound = (current_row() - radius) >= 0 ? -radius : -current_row();
//		lower_bound = (current_row() + radius) <= row_index.back() ? radius : row_index.back()-current_row();
//	}
};

//#include "stepper.cpp"
#endif
