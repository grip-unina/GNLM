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
 * buffers.h
 *
 *  Created on: 31/mar/2010
 *      Author: Giovanni Chierchia
 */

#ifndef BUFFERS_H_
#define BUFFERS_H_
#include <cassert>
#include <opencv/cv.h>
#include "accessors.h"

/*
 * La classe è un buffer di blocchi memorizzati in una griglia rettangolare.
 * La dimensione della griglia è "width x height", mentre la dimensione dei
 * singoli blocchi è "w x h". Questi parametri sono fissati dal costruttore.
 */
template <typename PixelType>
struct Block_Buffer
{
	typedef PixelType pixel_type;

 private:

	/* buffer dei blocchi */
	cv::Mat_<PixelType> buffer;

	/* riferimenti dei blocchi (contenuti nel buffer) */
	Distinct_Accessor<PixelType> cache;

	/* dimensioni del buffer (in blocchi) */
	int rows_;
	int cols_;

	/* dimensioni del singolo blocco */
	int block_rows_;  // numero di righe
	int block_cols_;  // numero di colonne

 public:

	/* Il costruttore inizializza le dimensioni del buffer.
	 *
	 * PARAMETRI D'INGRESSO:
	 *  1) grid_xxx = dimensioni (in blocchi) del buffer:
	 *       - grid_rows = numero di righe della griglia
	 *       - grid_cols = numero di colonne della griglia
	 *  2) block_xxx = dimensioni del singolo blocco:
	 *   	 - block_rows = numero di righe del blocco
	 *  	 - block_cols = numero di colonne del blocco
	 */
	Block_Buffer( int grid_rows, int grid_cols, int block_rows, int block_cols );

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

	/* Il metodo restituisce una "vista" del blocco di posizione (x,y).
	 * La "vista" è una sorta di riferimento al blocco, e può essere
	 * usato per leggere o scrivere il blocco dal/nel buffer.
	 */
	cv::Mat_<PixelType>& operator()(int row, int col) {
		assert( row >= 0 && row < rows() && col >= 0 && col < cols() );
		return cache(row,col);
	}
	const cv::Mat_<PixelType>& operator()(int row, int col) const {
		assert( row >= 0 && row < rows() && col >= 0 && col < cols() );
		return cache(row,col);
	}

//	/*
//	 * Il metodo stampa il contenuto del buffer sullo "std. output".
//	 *   - start_row = riga da cui iniziare la stampa
//	 *   - num_rows = numero di righe da stampare
//	 * NOTA: Stampa dalla riga "start_row" alla riga "start_row + num_rows - 1".
//	 */
//	void print(int start_row, int num_rows)
//	{
//		assert( start_row >= 0 && start_row < rows() );
//		assert( start_row + num_rows <= rows() );
//
//		for(int i = start_row; i < start_row + num_rows; i++) { // scorri le righe della griglia
//			for(int h=0 ; h < block_rows(); h++) { // scorri le righe dei blocchi
//				for(int j=0; j < cols(); j++) { // scorri le colonne della griglia
//					for(int k=0; k < block_cols(); k++) { // scori le colonne dei blocchi
//						std::cout << operator()(i,j)(h,k) << "\t";
//					}
//					std::cout << "|\t";
//				}
//				std::cout << "\n";
//			}
//			for(int j=0; j < cols(); j++) { // scorri le colonne della griglia
//				for(int k=0; k < block_cols(); k++) { // scori le colonne dei blocchi
//					std::cout << "--------";
//				}
//				std::cout << "--------";
//			}
//			std::cout << "\n";
//		}
//	}
//
//	void print() {
//		print(0, rows());
//	}
};



/*
 * La classe è un buffer di blocchi memorizzati in una griglia rettangolare.
 * La dimensione della griglia è "width x height", mentre la dimensione dei
 * singoli blocchi è "w x h". Questi parametri sono fissati dal costruttore.
 *
 * Il buffer può essere ruotato lungo la direzione verticale.
 */
template <typename PixelType>
class Rotable_Buffer : public Block_Buffer<PixelType>
{
	typedef Block_Buffer<PixelType> base_t;

	int head;  // indice della prima riga

 public:

	/* Il costruttore inizializza le dimensioni del buffer.
	 *
	 * PARAMETRI D'INGRESSO:
	 *  1) grid_xxx = dimensioni (in blocchi) del buffer:
	 *       - grid_rows = numero di righe della griglia
	 *       - grid_cols = numero di colonne della griglia
	 *  2) block_xxx = dimensioni del singolo blocco:
	 *   	 - block_rows = numero di righe del blocco
	 *  	 - block_cols = numero di colonne del blocco
	 */
	Rotable_Buffer( int grid_rows, int grid_cols, int block_rows, int block_cols );

	/* Il metodo restituisce una "vista" del blocco di posizione (x,y).
	 * La "vista" è una sorta di riferimento al blocco, e può essere
	 * usato per leggere o scrivere il blocco dal/nel buffer.
	 */
	cv::Mat_<PixelType>& operator()(int row, int col) {
		assert( row >= 0 && row < base_t::rows() );
		return base_t::operator()( (head + row) % base_t::rows(), col );
	}
	const cv::Mat_<PixelType>& operator()(int row, int col) const {
		assert( row >= 0 && row < base_t::rows() );
		return base_t::operator()( (head + row) % base_t::rows(), col );
	}

	/* Il metodo restituisce una "vista" del blocco che occupa
	 * la colonna "x" sulla prima riga della griglia.
	 */
	cv::Mat_<PixelType>& on_first_row(int col) {
		return operator()(0,col);
	}
	const cv::Mat_<PixelType>& on_first_row(int col) const {
		return operator()(0,col);
	}

	/* Il metodo restituisce una "vista" del blocco che occupa
	 * la colonna "x" sull'ultima riga della griglia.
	 */
	cv::Mat_<PixelType>& on_last_row(int col) {
		return operator()( base_t::rows()-1, col );
	}
	const cv::Mat_<PixelType>& on_last_row(int col) const {
		return operator()( base_t::rows()-1, col );
	}

	/* Il metodo effettua una rotazione della griglia di blocchi.
	 * La seconda riga viene spostata sulla prima, la terza riga
	 * viene spostata sulla seconda, ..., l'ultima riga viene
	 * spostata sulla penultima, e la prima riga viene spostata
	 * sull'ultima. L'operazione è molto efficiente, perché nessun
	 * blocco viene spostato fisicamente (ruotano solo gli indici di riga).
	 *
	 */
	void rotate(int step) {
		assert( 0 <= step && step < base_t::rows() );
		head = (head + step) % base_t::rows();
	}

// public:
//
//	/*
//	 * Il metodo stampa il contenuto del buffer sullo "std. output".
//	 *   - num_rows = numero di righe da stampare
//	 * NOTA: se num_rows=3, stampa le righe 0->2
//	 */
//	void print(int num_rows) const
//	{
//		assert( num_rows >= 0 && num_rows <= base_t::rows() );
//
//		// scorri le righe della griglia
//		for(int i=0; i < num_rows; i++) {
//			base_t::print( (head + i) % base_t::rows(), 1 );
//		}
//		std::cout << "\n";
//	}
//
//	void print() {
//		print( base_t::rows() );
//	}
};



template <typename PixelType>
struct Stack_Buffer
{
	typedef PixelType pixel_type;
	class iterator;

 private:

	Block_Buffer<PixelType> buffer;

 public:

	Stack_Buffer(int rows, int cols, int blks);

	template <typename BlockAccessor>
	Stack_Buffer( const BlockAccessor &src );

	template <typename BlockAccessor>
	Stack_Buffer( const BlockAccessor &src, const std::vector< std::pair<int,int> > &points, int zero_pad=0 );

	template <typename BlockAccessor>
	void fromBlock( const BlockAccessor &src, const std::vector< std::pair<int,int> > &points, int length);

	int rows() const {
		return buffer.block_rows();
	}

	int cols() const {
		return buffer.block_cols();
	}

	int blks() const {
		return buffer.cols();
	}

	/*
	 * Il metodo ritorna il blocco k-esimo dello stack.
	 */
	cv::Mat_<pixel_type>& operator[](int idx) {
		assert( idx >= 0 && idx < blks() );
		return buffer(0, idx);
	}
	const cv::Mat_<pixel_type>& operator[](int idx) const {
		assert( idx >= 0 && idx < blks() );
		return buffer(0, idx);
	}

	/*
	 * Il metodo restituisce l'iteratore lungo la direzione spettrale (i,j).
	 */
	iterator begin(int row, int col) {
		assert( 0 <= row && row < rows() && 0 <= col && col < cols() );
		return iterator( buffer(0,0)[row]+col, 0, cols() );
	}
	const iterator begin(int row, int col) const {
		assert( 0 <= row && row < rows() && 0 <= col && col < cols() );
		return iterator( (pixel_type*)buffer(0,0)[row]+col, 0, cols() );
	}

	/*
	 * Il metodo restituisce l'iteratore lungo la direzione spettrale (i,j).
	 */
	iterator end(int row, int col) {
		assert( 0 <= row && row < rows() && 0 <= col && col < cols() );
		return iterator( buffer(0,0)[row]+col, blks(), cols() );
	}
	const iterator end(int row, int col) const {
		assert( 0 <= row && row < rows() && 0 <= col && col < cols() );
		return iterator( (pixel_type*)buffer(0,0)[row]+col, blks(), cols() );
	}

	/*
	 * La classe rappresenta un vettore lungo la direzione spettrale.
	 */
	class iterator : public std::iterator<std::forward_iterator_tag, pixel_type>
	{
		int step;			// offset tra due elementi successivi
		pixel_type *ptr;	// puntatore all'elemento corrente

	public:

		iterator(pixel_type *ptr_, int idx, int step_) : step(step_), ptr( ptr_+idx*step) { }

		/*
		 * Il metodo accede all'elemento corrente dell'iteratore.
		 */
		pixel_type& operator*() { return *ptr; }
		const pixel_type& operator*() const { return *ptr; }

		/*
		 * Il metodo accede a un elemento arbitrario, rispetto alla posizione corrente dell'iteratore.
		 */
		pixel_type& operator[](int idx) { return ptr[idx*step]; }
		const pixel_type& operator[](int idx) const { return ptr[idx*step]; }

		/*
		 * Il metodo avanza l'iteratore (di una posizione) lungo la direzione spettrale.
		 */
		void operator++() {	ptr += step; }

		/*
		 * Il metodo restituisce "true" se i due iteratori puntano a diverse posizioni spettrali.
		 */
		bool operator!=(const iterator &that) const { return (ptr != that.ptr);	}
	};
};

#include "buffers.hpp"
#endif
