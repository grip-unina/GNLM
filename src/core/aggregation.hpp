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
 * aggregation.hpp
 *
 *  Created on: 12/apr/2010
 *      Author: Giovanni Chierchia
 */

#ifndef _AGGREGATION_HPP_
#define _AGGREGATION_HPP_
#include "aggregation.h"
#include <iostream>

template <typename BlockAccessor, typename ScaleType>
void aggregation( const Stack_Buffer<typename BlockAccessor::pixel_type> &stackT2D, const std::vector< std::pair<int,int> > &matched, const ScaleType &scale, Neighborhood_Rect_Accessor<BlockAccessor> &image, Neighborhood_Rect_Accessor<BlockAccessor> &weights, const AggregationOptions<typename BlockAccessor::pixel_type> &opt )
{
	typedef typename BlockAccessor::pixel_type pixel_type;
	assert( stackT2D.blks() >= (int) matched.size() && "La lunghezza dello stack 'sorgente' deve essere compatibile con il numero di blocchi 'matched'" );

	/* NOTA: Lo stack 'sorgente' può contenere dei blocchi 'fittizi' che
	 *       non corrispondono a nessun blocco 'reale' dell'immagine:
	 *         - blocchi generati dal collab. filter a causa dello zero-padding
	 *         - blocchi di iterazioni precedenti che, per motivi di efficienza, non sono stati deallocati
	 *       Tali blocchi devono essere ignorati nella fase di 'aggregation'.
	 *
	 *       Le informazioni circa il numero e la posizione dei blocchi 'corretti' è
	 *       presente nel vettore "matched", e bisogna far SEMPRE riferimento ad esso.
	 */

	// finestra 2D
	const cv::Mat_<pixel_type>& winMat = opt.win2D.getMatrix();

	// blocco di 'appoggio'
	cv::Mat_<pixel_type> blockMat( stackT2D.rows() , stackT2D.cols() );

	/* scorri i blocchi "matched" */
	for(size_t i=0; i<matched.size(); i++)
	{

		// applicazione della finestra 2D sul blocco (per ridurre gli effetti ai bordi)
		opt.win2D( stackT2D[i], blockMat );


		// posizione del blocco (relativa al vicinato)
		int row = matched[i].first;
		int col = matched[i].second;

		// aggiornamento delle immagini d'uscita
		multiply_and_accumulate( blockMat, scale, image(row,col) );		//   image(row,col) += blockMat * scale;
		multiply_and_accumulate( winMat, scale, weights(row,col) );		// weights(row,col) +=   winMat * scale;
	}
}

template <typename PixelType, typename ScaleType>
void aggregation_iter( const Stack_Buffer<PixelType> &stackT2D, const std::vector< std::pair<int,int> > &matched, const ScaleType &scale, Sliding_Accessor<PixelType> &image, Sliding_Accessor<PixelType> &weights, const AggregationOptions<PixelType> &opt, int Nb )
{

	assert( stackT2D.blks() >= (int) matched.size() && "La lunghezza dello stack 'sorgente' deve essere compatibile con il numero di blocchi 'matched'" );

	/* NOTA: Lo stack 'sorgente' può contenere dei blocchi 'fittizi' che
	 *       non corrispondono a nessun blocco 'reale' dell'immagine:
	 *         - blocchi generati dal collab. filter a causa dello zero-padding
	 *         - blocchi di iterazioni precedenti che, per motivi di efficienza, non sono stati deallocati
	 *       Tali blocchi devono essere ignorati nella fase di 'aggregation'.
	 *
	 *       Le informazioni circa il numero e la posizione dei blocchi 'corretti' è
	 *       presente nel vettore "matched", e bisogna far SEMPRE riferimento ad esso.
	 */

	// finestra 2D
	const cv::Mat_<PixelType>& winMat = opt.win2D.getMatrix();


	// blocco di 'appoggio'
	cv::Mat_<PixelType> blockMat( stackT2D.rows() , stackT2D.cols() );



	/* scorri i blocchi "matched" */
	for(size_t i=0; i<Nb; i++)
	{

		// applicazione della finestra 2D sul blocco (per ridurre gli effetti ai bordi)
		opt.win2D( stackT2D[i], blockMat );

		// posizione del blocco (relativa al vicinato)
		int row = matched[i].first;
		int col = matched[i].second;

		// aggiornamento delle immagini d'uscita
		multiply_and_accumulate( blockMat, scale, image(row,col) );		//   image(row,col) += blockMat * scale;
		multiply_and_accumulate( winMat  , scale, weights(row,col) );		// weights(row,col) +=   winMat * scale;

	}
}

template <typename PixelType, typename ScaleType>
void aggregation1( const cv::Mat_<PixelType> &block,  std::pair<int,int> pos, const ScaleType &scale, Sliding_Accessor<PixelType> &image, Sliding_Accessor<PixelType> &weights, const AggregationOptions<PixelType> &opt ) {

	int row = pos.first;
	int col = pos.second;

	const cv::Mat_<PixelType>& winMat = opt.win2D.getMatrix();
	cv::Mat_<PixelType> blockMat( block.rows , block.cols);
	opt.win2D( block, blockMat );

	// aggiornamento delle immagini d'uscita
	multiply_and_accumulate( blockMat, scale, image(row,col) );		//   image(row,col) += blockMat * scale;
	multiply_and_accumulate(   winMat, scale, weights(row,col) );	// weights(row,col) +=   winMat * scale;

}

template <typename PixelType, typename ScaleType>
void aggregation1pixel( const cv::Mat_<PixelType> &block,  std::pair<int,int> pos, const ScaleType &scale, Sliding_Accessor<PixelType> &image, Sliding_Accessor<PixelType> &weights) {

	int row = pos.first;
	int col = pos.second;
	cv::Mat_<PixelType>& blockImg =   image(row,col);
	cv::Mat_<PixelType>& blockW   = weights(row,col);
	int Mc = (block.rows - 1)/2;
	int Nc = (block.cols - 1)/2;
	blockImg(Mc,Nc) +=  scale*block(Mc,Nc);
	blockW(Mc,Nc)   +=  scale;


}

#endif /* AGGREGATION_CPP_ */
