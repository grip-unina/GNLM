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
 * accessor.h
 *
 *  Created on: 19/mar/2010
 *      Author: Giovanni Chierchia
 */

#ifndef ACCESSORS_H_
#define ACCESSORS_H_

#include <cassert>
#include <vector>
#include <opencv/cv.h>

template <typename Type>
struct Block_Accessor
{
	typedef Type pixel_type;

 private:

	/* immagine sorgente */
	cv::Mat_<Type> &src;

	/* dimensione dei blocchi */
	int block_rows_;
	int block_cols_;

	/* blocchi contenuti nell'immagine */
	int rows_;  // num. di blocchi su una colonna
	int cols_;  // num. di blocchi su una riga

	/* cache dei riferimenti ai blocchi (contenuti nell'immagine) */
	std::vector<int> row_ptr;
	std::vector< cv::Mat_<Type> > cache;

 public:

	Block_Accessor( cv::Mat_<Type> &source, int block_rows, int block_cols, int row_step, int col_step );

	const cv::Mat_<Type>& operator()(int row, int col) const {
		assert( 0 <= row && row < rows_ && 0 <= col && col < cols_ );
		return cache[row_ptr[row] + col];
	}
	cv::Mat_<Type>& operator()(int row, int col) {
		assert( 0 <= row && row < rows_ && 0 <= col && col < cols_ );
		return cache[row_ptr[row] + col];
	}

	int rows() const {
		return rows_;
	}

	int cols() const {
		return cols_;
	}

	int block_rows() const {
		return block_rows_;
	}

	int block_cols() const {
		return block_cols_;
	}
};


template <typename Type>
struct Sliding_Accessor : public Block_Accessor<Type>
{
	Sliding_Accessor( cv::Mat_<Type> &source, int block_rows, int block_cols )
		: Block_Accessor<Type>(source, block_rows, block_cols, 1, 1)
	{ }
};


template <typename Type>
struct Distinct_Accessor : public Block_Accessor<Type>
{
	Distinct_Accessor( cv::Mat_<Type> &source, int block_rows, int block_cols )
		: Block_Accessor<Type>(source, block_rows, block_cols, block_rows, block_cols)
	{
		assert( source.rows % block_rows == 0 && source.cols % block_cols == 0 );
	}
};


template <typename BlockAccessor>
struct Neighborhood_Rect_Accessor
{
	typedef typename BlockAccessor::pixel_type pixel_type;

 private:

	/* contenitore dei blocchi sorgente */
	BlockAccessor &src;

	/* raggio (massimo) del vicinato */
	int radius;

	/* posizioni assolute del vicinato (relative al contenitore) */
	cv::Point_<int> central_point;		// punto centrale
	cv::Point_<int> topleft_point;		// vertice in alto a sinistra
	cv::Point_<int> downright_point;	// vertice in basso a destra

	/* dimensioni del vicinato (relativi alla posizione del blocco centrale) */
	int left_limit;  // valore negativo (o nullo): offset della colonna più a sinistra inclusa nel vicinato
	int upper_limit; // valore negativo (o nullo): offset della riga più in alto inclusa nel vicinato
	int right_limit; // valore positivo (o nullo): offset della colonna più a destra inclusa nel vicinato
	int lower_limit; // valore positivo (o nullo): offset della riga più in basso inclusa nel vicinato

 public:

	Neighborhood_Rect_Accessor(BlockAccessor &accessor, int diameter)
		: src(accessor), radius( (diameter-1)/2 )
	{
		assert( diameter > 0 && diameter % 2 != 0 );
		assert( radius < src.rows() && radius < src.cols() );

		set_center(0, 0);
	}

	void set_center(int row_index, int col_index)
	{
		assert( 0 <= row_index && row_index < src.rows() );
		assert( 0 <= col_index && col_index < src.cols() );

		// aggiorna la dimensione del vicinato
		upper_limit = (row_index - radius) >= 0 ? -radius : -row_index;
		lower_limit = (row_index + radius) < src.rows() ? radius : src.rows()-1 - row_index;
		left_limit  = (col_index - radius) >= 0 ? -radius : -col_index;
		right_limit = (col_index + radius) < src.cols() ? radius : src.cols()-1 - col_index;

		// aggiorna la posizione del blocco centrale
		central_point.x = row_index;
		central_point.y = col_index;

		// aggiorna la posizione del vertice in alto a sinistra
		topleft_point.x = row_index + upper_limit;
		topleft_point.y = col_index + left_limit;

		// aggiorna la posizione del vertice in basso a destra
		downright_point.x = row_index + lower_limit;
		downright_point.y = col_index + right_limit;
	}

	int block_rows() const {
		return src.block_rows();
	}

	int block_cols() const {
		return src.block_cols();
	}

	int rows() const {
		return lower_limit - upper_limit + 1;
	}

	int cols() const {
		return right_limit - left_limit + 1;
	}

	int left() const {
		return left_limit;
	}

	int right() const {
		return right_limit;
	}

	int upper() const {
		return upper_limit;
	}

	int lower() const {
		return lower_limit;
	}

	/*
	 * Il metodo restituisce la posizione ASSOLUTA del punto centrale del vicinato:
	 *   - Point_<int>::x  =  indice di riga
	 *   - Point_<int>::y  =  indice di colonna
	 */
	cv::Point_<int> central() const {
		return central_point;
	}

	/*
	 * Il metodo restituisce la posizione ASSOLUTA del vertice IN ALTO A SINISTRA del vicinato:
	 *   - Point_<int>::x  =  indice di riga
	 *   - Point_<int>::y  =  indice di colonna
	 */
	cv::Point_<int> topleft() const {
		return topleft_point;
	}

	/*
	 * Il metodo restituisce la posizione ASSOLUTA del vertice IN BASSO A DESTRA del vicinato:
	 *   - Point_<int>::x  =  indice di riga
	 *   - Point_<int>::y  =  indice di colonna
	 */
	cv::Point_<int> downright() const {
		return downright_point;
	}

	/*
	 * Il metodo accede ai vicini usando le posizioni (indici relativi al BLOCCO IN ALTO A SINISTRA):
	 *   a. (0,0) è il blocco in alto a sinistra
	 *   b. (rows-1,cols-1) è il blocco in basso a destra
	 */
	cv::Mat_<pixel_type>& operator()(int row, int col) {
		assert( 0 <= row && row < rows() && 0 <= col && col < cols() );
		return src( row+topleft_point.x, col+topleft_point.y );
	}
	const cv::Mat_<pixel_type>& operator()(int row, int col) const {
		assert( 0 <= row && row < rows() && 0 <= col && col < cols() );
		return src( row+topleft_point.x, col+topleft_point.y );
	}

	/*
	 * Il metodo accede ai vicini usando gli offset (indici relativi al BLOCCO CENTRALE):
	 *   a. (0,0)  è il blocco centrale
	 *   b. (0,1)  è il blocco a destra di quello centrale
	 *   c. (0,-1) è il blocco a sinistra di quello centrale
	 *   d. (1,0)  è il blocco in basso a quello centrale
	 *   e. (-1,0) è il blocco in alto a quello centrale
	 *   f. (up,left) è il blocco in alto a sinistra del vicinato
	 *   g. (down,right) è il blocco in basso a destra del vicinato
	 */
	cv::Mat_<pixel_type>& get(int row, int col) {
		assert( upper_limit <= row && row <= lower_limit && left_limit <= col && col <= right_limit );
		return src( row+central_point.x, col+central_point.y );
	}
	const cv::Mat_<pixel_type>& get(int row, int col) const {
		assert( upper_limit <= row && row <= lower_limit && left_limit <= col && col <= right_limit );
		return src( row+central_point.x, col+central_point.y );
	}

	// class Iterator {
	//		/*
	//	 	* Il metodo avanza l'iteratore di una posizione. La direzione di
	//	 	* avanzamento è lungo le righe, ma quando supera l'ultima colonna
	//	 	* salta alla prima colonna della riga successiva. Restituisce
	//	 	* "false" quando supera l'ultima colonna dell'ultima riga.
	//	 	*/
	//		bool operator++()
	//		{
	//			col++;
	//			if( col > ptr->right() ) {
	//				col = ptr->left();
	//				row++;
	//			}
	//			return is_valid();
	//		}
	// };
};

#include "accessors.hpp"
#endif /* ACCESSORS_H_ */
