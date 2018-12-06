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
 * win2D.h
 *
 *  Created on: 08/apr/2010
 *      Author: Giovanni Chierchia
 *      Notice: Codice estratto dal file "windows.cpp" della libreria SPUC (Signal processing using C++ - A DSP library)
 */

#ifndef WIN2D_H_
#define WIN2D_H_
#include <vector>
#include <string>
#include <stdexcept>
#include <opencv/cv.h>

#define SQRT2		1.41421356237309504880
#define HALFSQRT2	0.70710678118654752440
#define TWOPI		6.28318530717958647692		// 2*pi
#define PI			3.141592653589793238462643	// pi

template <typename ForwardIterator>
void hamming(int size, ForwardIterator dest, bool flag=false);

template <typename ForwardIterator>
void hanning(int size, ForwardIterator dest, bool flag=false);

template <typename ForwardIterator>
void blackman(int size, ForwardIterator dest, bool flag=false);

template <typename ForwardIterator>
void kaiser(int size, ForwardIterator dest, typename std::iterator_traits<ForwardIterator>::value_type beta);

template <typename Type>
class Win2D
{
	cv::Mat_<Type> win;

 public :

	Win2D() : win(0,0) { }

	typedef Type outType;

	void config( std::string name, int rows, int cols, Type beta=Type() )
	{
		// seleziona la finestra 1D
		std::vector<Type> winRow(rows);
		std::vector<Type> winCol(cols);
		if( name == "ones" ) {
			winRow = std::vector<Type>(rows, 1);
			winCol = std::vector<Type>(cols, 1);
		}
		else if( name == "hamming" ) {
			hamming( rows, winRow.begin() );
			hamming( cols, winCol.begin() );
		}
		else if( name == "hanning" ) {
			hanning( rows, winRow.begin() );
			hanning( cols, winCol.begin() );
		}
		else if( name == "blackman" ) {
			blackman( rows, winRow.begin() );
			blackman( cols, winCol.begin() );
		}
		else if( name == "kaiser" ) {
			kaiser( rows, winRow.begin(), beta );
			kaiser( cols, winCol.begin(), beta );
		}
		else {
			throw std::runtime_error( "[Win2D] " + name + ": finestra sconosciuta" );
		}

		// construisci la finestra 2D
		win.create(rows,cols);
		for(int i=0; i<rows; i++) {
			for(int j=0; j<cols; j++) {
				win(i,j) = winRow[i] * winRow[j];
			}
		}
	}

	const cv::Mat_<Type>& getMatrix() const {
		return win;
	}

	void operator()( const cv::Mat_<Type> &src, cv::Mat_<outType> &dest ) const {
		assert( win.rows != 0 && win.cols != 0 && "Il tipo di finestra non è stato configurato" );
		cv::multiply( src, win, dest );
	}
};

/*
 *	Finestra di Hamming: w(n) = 0.54 - 0.46 * cos(2*pi * n/N)
 *
 *	 -  size = lunghezza della finestra
 *   -  flag = tipo di campionamento (true = periodico, false = simmetrico)
 */
template <typename ForwardIterator>
void hamming(int size, ForwardIterator dest, bool flag)
{
	typedef typename std::iterator_traits<ForwardIterator>::value_type type;

	int N = flag ? size : size-1;
	for( type x=0; x<size; x++, ++dest ) {
		*dest = 0.54 - 0.46 * std::cos(TWOPI * x/N);
	}
}

/*
 * Finestra di Hanning: w(n) = 0.5 * ( 1 - cos(2*pi* n/N) )
 *
 *	 -  size = lunghezza della finestra
 *   -  flag = tipo di campionamento (true = periodico, false = simmetrico)
 */
template <typename ForwardIterator>
void hanning(int size, ForwardIterator dest, bool flag)
{
	typedef typename std::iterator_traits<ForwardIterator>::value_type type;

	int N = flag ? size : size-1;
	for( type x=0; x<size; x++, ++dest ) {
		*dest = 0.5 - 0.5 * std::cos(TWOPI * x/N);
	}
}

/*
 * Finestra di Blackman: w[x] = 0.42 - 0.5 * cos(2*pi*x / nf) + 0.08 * cos(2*pi*x / nf)
 *
 *	 -  size = lunghezza della finestra
 *   -  flag = tipo di campionamento (true = periodico, false = simmetrico)
 */
template <typename ForwardIterator>
void blackman(int size, ForwardIterator dest, bool flag)
{
	typedef typename std::iterator_traits<ForwardIterator>::value_type type;

	int N = flag ? size : size-1;
	for( type x=0; x<size; x++, ++dest ) {
		*dest = 0.42 - 0.5 * std::cos(TWOPI * x/N) + 0.08 * std::cos(2*TWOPI * x/N);
	}
}

/*
 * Funzione di Bessel (per la finestra di kaiser)
 */
template <typename Type>
Type bessel(Type x)
{
	double t = 1.0e-08, y = 0.5*x, e = 1.0, de = 1.0, sde = 1.0;
	for( double i=1; i<26; i++ ) {
		de *= y / i;
		sde = de*de;
		e += sde;
		if( (e*t-sde) > 0 ) break;
	}
	return e;
}

/*
 * Finestra di Kaiser
 */
template <typename ForwardIterator>
void kaiser(int size, ForwardIterator dest, typename std::iterator_traits<ForwardIterator>::value_type beta)
{
	typedef typename std::iterator_traits<ForwardIterator>::value_type type;

	// parametri fissi
	type bes = bessel( beta );
	type xi, xd = (size-1)*(size-1);

	// numero di pesi da calcolare
	int odd = size % 2;
	int N = (size+odd) / 2;

	// calcola i pesi della finestra
	std::vector<type> w(N);
	for( int i=0; i<N; i++ ) {
		xi = odd ? i : i + 0.5;
		xi = 4 * xi*xi;
		w[i] = bessel( beta * std::sqrt(1.0 - xi/xd) ) / bes;
	}

	// copia nell'uscita
	typename std::vector<type>::reverse_iterator reverse;
	for( reverse = w.rbegin(); reverse != w.rend(); ++reverse, ++dest ) {
		*dest = *reverse;
	}
	typename std::vector<type>::iterator forward;
	for( forward = w.begin()+odd; forward != w.end(); ++forward, ++dest ) {
		*dest = *forward;
	}
}

template <typename Type> inline
void multiply_and_accumulate( const cv::Mat_<Type> &src, const Type scale, cv::Mat_<Type> &dest ) {
	assert( src.rows == dest.rows && src.cols == dest.cols );

	for(int i=0; i<dest.rows; i++) {
		const Type *src_ptr = src[i];
		Type *dst_ptr = dest[i];

		for(int j=0; j<dest.cols; j++) {
			dst_ptr[j] += (*(src_ptr++)) * scale;
		}
	}
}

#endif /* WIN2D_H_ */
