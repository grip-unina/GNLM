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
 * stripe_mat.h
 *
 *  Created on: 15/mar/2010
 *      Author: Giovanni Chierchia
 */

#ifndef _STRIPE_MAT_H_
#define _STRIPE_MAT_H_
#include <cassert>
#include <opencv/cv.h>
#include "buffers.h"

/*
 * La classe memorizza i pixel dell'immagine che appartengono
 * a una striscia di dimensioni almeno pari a "search_diameter".
 */
template <typename SrcPixelType, typename PixelFunction>
struct StripeMtxOp
{
	typedef typename PixelFunction::outType pixel_type;
	typedef SrcPixelType in_pixel_type;

	 private:
		const cv::Mat_<SrcPixelType> &src;
		const PixelFunction& op;

		/* dimensione dei blocchi di output*/
		int rows_;
		int cols_;

		/* dimensione dei blocchi di output*/
		int block_rows_;
		int block_cols_;

		/* Indici della striscia (riferiti all'immagine) */
		int start_index;   // indice della prima riga della striscia
		int middle_index;  // indice della riga "centrale" della striscia (non tenendo conto dei bordi)
		int finish_index;  // indice SUCCESSIVO all'ultima riga della striscia
		int finish_index_block;  // indice SUCCESSIVO all'ultima riga della striscia

		int mod_buffer;  // indice della prima riga
		int head_buffer;  // indice della prima riga
		/* Buffer dei blocchi */
		cv::Mat_<pixel_type> buffer;
		/* riferimenti dei blocchi (contenuti nel buffer) */
		Sliding_Accessor<pixel_type> cache;

		/* Parametri della striscia */
		int radius;	// raggio della zona di ricerca


	 public:

		/* Il costruttore inizializza le dimensioni e il contenuto della striscia.
		 *
		 * PARAMETRI D'INGRESSO:
		 *
		 *  1) accessor = contenitore dei blocchi sorgente. DEVE AVERE LE SEGUENTI FUNZIONI:
		 *                  - rows() = num. di righe del contenitore (num. di blocchi su una colonna)
		 *                  - cols() = num. di colonne del contenitore (num. di blocchi su una riga)
		 *                  - block_rows() = num. di righe di un blocco
		 *                  - block_cols() = num. di colonne di un blocco
		 *                  - operator()(int i, int j) = "vista" del blocco di posizione (i,j)
		 *
		 * 2) function = "oggetto funzione" che incapsula l'operazione da eseguire sui singoli blocchi
		 *                prima di essere copiati nel buffer. DEVE AVERE LE SEGUENTI FUNZIONI:
		 *                  - operator()(Mat_<T1> &src, Mat_<T2> &dest)
		 *                  - typedef dest_type;
		 *
		 */
	 	StripeMtxOp(const cv::Mat_<SrcPixelType> &source,const PixelFunction &function, int search_diameter, int block_rows, int block_cols);


		/*
		 * Indice (riferito alla sorgente) della prima riga della striscia.
		 */
		int begin() const {
			return start_index;
		}

		/*
		 * Indice (riferito alla sorgente) della riga centrale della striscia (non tenendo conto dei bordi).
		 */
		int center() const {
			return middle_index;
		}

		/*
		 * Indice (riferito alla sorgente) SUCCESSIVO all'ultima riga della striscia.
		 */
		int end() const {
			return finish_index_block;
		}

		/*
		 * Altezza della striscia, cioè il numero di righe del contenitore
		 * sorgente che sono contenute nella striscia.
		 */
		int height() const {
			return end() - begin();
		}

		/*
		 * Numero di righe del contenitore sorgente.
		 */
		int rows() const {
			return rows_;
		}

		/*
		 * Numero di colonne del contenitore sorgente.
		 */
		int cols() const {
			return cols_;
		}

		int block_rows() const {
			return block_rows_;
		}

		int block_cols() const {
			return block_cols_;
		}

		/*
		 * Il metodo restituisce il blocco di posizione <row,col>.
		 * Gli indici sono assoluti, cioè riferiti al contenitore.
		 */
		const cv::Mat_<pixel_type>& operator()(int row, int col) const {
			assert( row >= start_index && row < finish_index_block && col >= 0 && col < cols_ );
			return cache((row+head_buffer-start_index)%mod_buffer,col);
		}
		cv::Mat_<pixel_type>& operator()(int row, int col) {
			assert( row >= start_index && row < finish_index_block && col >= 0 && col < cols_ );
			return cache((row+head_buffer-start_index)%mod_buffer,col);
		}

		/*
		 * Il metodo centra la striscia sulla riga con indice (assoluto) "new_middle".
		 * L'indice della nuova riga deve essere MAGGIORE (o uguale) alla riga corrente.
		 *
		 * Dopo aver chiamato il costruttore, la striscia è vuota. Alla prima chiamata,
		 * il metodo riempime la striscia con le righe comprese nella zona di ricerca
		 * della riga "new_middle". Alle chiamate successive, il metodo sposta la
		 * striscia verso il basso, centrandola sulla nuova riga "new_middle".
		 *
		 * Le prime volte che viene chiamato il metodo, la striscia cresce verso
		 * il basso, ma rimane allineata al bordo superiore del contenitore.
		 * Quando l'altezza diventa "2*radius+1", la striscia smette di crescere
		 * e inizia a spostarsi verso il basso. Quando la striscia raggiunge il bordo
		 * inferiore del contenitore, essa smette di muoversi e inizia a ridursi.
		 */
		void move_forward(int new_middle);
	 protected:
		void getSrcData(int row, pixel_type* dest);
		void getSrcData(int row, int row_dest);
};


#include "stripe_mat.hpp"
#endif
