//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// Copyright (c) 2012 Image Processing Research Group of University Federico II of Naples ('GRIP-UNINA').
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
 * neighborhood.h
 *
 *  Created on: 05/09/2012
 *      Author: Davide Cozzolino
 */

#ifndef NEIGHBORHOOD_H_
#define NEIGHBORHOOD_H_

#include <cassert>
#include <vector>
#include <opencv/cv.h>
#include "accessors.h"


class Neighborhood {

 public:

	typedef std::pair<int,int> pair;

	/*
	 * Il metodo serve per impostare la posizione centrale
	 */
	virtual void set_center(pair point) =0;

	/*
	 * Il metodo restituisce la posizione ASSOLUTA del centro
	 */
	virtual pair central() const =0;

	/*
	 * Il metodo restituisce la posizione ASSOLUTA del vertice IN ALTO A SINISTRA del vicinato:
	 */
	virtual pair topleft() const =0;

	/*
	 * Il metodo restituisce la posizione ASSOLUTA del vertice IN BASSO A DESTRA del vicinato:
	 */
	virtual pair downright() const =0;

	/*
	 * Il metodo restituisce il raggio (massimo) del vicinato
	 */
	virtual int radius() const = 0;

	/*
	 * Il metodo verifica se il punto appartiene al vicinato.
	 */
	virtual bool validate(pair point) const =0;


	virtual ~Neighborhood() {};


};

template <typename ElementType>
class Iterator {
 public:

	virtual void reset() =0;

	virtual	bool hasNext() =0;

	virtual ElementType next() =0;

	virtual ~Iterator() {};

 };

class NeighborhoodRect: public Neighborhood
{

 private:

	/* blocchi contenuti nell'immagine */
	int rows_;  // num. di blocchi su una colonna
	int cols_;  // num. di blocchi su una riga

	/* raggio (massimo) del vicinato */
	int radius_;

	/* posizioni assolute del vicinato (relative al contenitore) */
	pair central_point;		// punto centrale
	pair topleft_point;		// vertice in alto a sinistra
	pair downright_point;	// vertice in basso a destra

	/* dimensioni del vicinato (relativi alla posizione del blocco centrale) */
	int left_limit;  // valore negativo (o nullo): offset della colonna più a sinistra inclusa nel vicinato
	int upper_limit; // valore negativo (o nullo): offset della riga più in alto inclusa nel vicinato
	int right_limit; // valore positivo (o nullo): offset della colonna più a destra inclusa nel vicinato
	int lower_limit; // valore positivo (o nullo): offset della riga più in basso inclusa nel vicinato

 public:

	NeighborhoodRect(int rows, int cols, int diameter)
		: rows_(rows), cols_(cols), radius_( (diameter-1)/2 ) {
			assert( diameter > 0 && diameter % 2 != 0 );
			assert( radius_ < rows_ && radius_ < cols_ );

			set_center(std::make_pair(0,0));
	}


	virtual void set_center(pair point) {
		assert( 0 <= point.first  && point.first  < rows_ );
		assert( 0 <= point.second && point.second < cols_ );

		// aggiorna la dimensione del vicinato
		upper_limit = (point.first  - radius_) >= 0     ? -radius_ : -point.first;
		lower_limit = (point.first  + radius_) <  rows_ ?  radius_ : rows_-1 - point.first;
		left_limit  = (point.second - radius_) >= 0     ? -radius_ : -point.second;
		right_limit = (point.second + radius_) <  cols_ ?  radius_ : cols_-1 - point.second;

		// aggiorna la posizione del blocco centrale
		central_point = point;

		// aggiorna la posizione del vertice in alto a sinistra
		topleft_point.first  = point.first + upper_limit;
		topleft_point.second = point.second + left_limit;

		// aggiorna la posizione del vertice in basso a destra
		downright_point.first  = point.first + lower_limit;
		downright_point.second = point.second + right_limit;
	}

	virtual void set_center(pair point, int diameters_) {
		assert( 0 <= point.first  && point.first  < rows_ );
		assert( 0 <= point.second && point.second < cols_ );

		// aggiorna la dimensione del vicinato
		int radius = (diameters_-1)/2;
		upper_limit = (point.first  - radius) >= 0     ? -radius : -point.first;
		lower_limit = (point.first  + radius) <  rows_ ?  radius : rows_-1 - point.first;
		left_limit  = (point.second - radius) >= 0     ? -radius : -point.second;
		right_limit = (point.second + radius) <  cols_ ?  radius : cols_-1 - point.second;

		// aggiorna la posizione del blocco centrale
		central_point = point;

		// aggiorna la posizione del vertice in alto a sinistra
		topleft_point.first  = point.first + upper_limit;
		topleft_point.second = point.second + left_limit;

		// aggiorna la posizione del vertice in basso a destra
		downright_point.first  = point.first + lower_limit;
		downright_point.second = point.second + right_limit;
	}

	/*
	 * Il metodo restituisce la posizione ASSOLUTA del punto centrale del vicinato:
	 *   - Point_<int>::x  =  indice di riga
	 *   - Point_<int>::y  =  indice di colonna
	 */
	virtual pair central() const {
		return central_point;
	}

	/*
	 * Il metodo restituisce la posizione ASSOLUTA del vertice IN ALTO A SINISTRA del vicinato:
	 *   - Point_<int>::x  =  indice di riga
	 *   - Point_<int>::y  =  indice di colonna
	 */
	virtual pair topleft() const {
		return topleft_point;
	}

	/*
	 * Il metodo restituisce la posizione ASSOLUTA del vertice IN BASSO A DESTRA del vicinato:
	 *   - Point_<int>::x  =  indice di riga
	 *   - Point_<int>::y  =  indice di colonna
	 */
	virtual pair downright() const {
		return downright_point;
	}

	virtual int radius() const {
		return radius_;
	}

	virtual bool validate(pair point) const {
		return  (point.first  >= topleft_point.first) &&
				(point.first  <= downright_point.first) &&
				(point.second >= topleft_point.second) &&
				(point.second <= downright_point.second);
	}

};


class NeighborhoodRectSA: public Neighborhood
{

 private:

	/* blocchi contenuti nell'immagine */
	int rows_;  // num. di blocchi su una colonna
	int cols_;  // num. di blocchi su una riga

	/* raggio (massimo) del vicinato */
	int radius_;
	const cv::Mat_<unsigned char> &diameters_;

	/* posizioni assolute del vicinato (relative al contenitore) */
	pair central_point;		// punto centrale
	pair topleft_point;		// vertice in alto a sinistra
	pair downright_point;	// vertice in basso a destra

	/* dimensioni del vicinato (relativi alla posizione del blocco centrale) */
	int left_limit;  // valore negativo (o nullo): offset della colonna più a sinistra inclusa nel vicinato
	int upper_limit; // valore negativo (o nullo): offset della riga più in alto inclusa nel vicinato
	int right_limit; // valore positivo (o nullo): offset della colonna più a destra inclusa nel vicinato
	int lower_limit; // valore positivo (o nullo): offset della riga più in basso inclusa nel vicinato

 public:

	NeighborhoodRectSA(int rows, int cols, const cv::Mat_<unsigned char> &diameters)
		: rows_(rows), cols_(cols), diameters_( diameters ) {
			set_center(std::make_pair(0,0));
	}


	virtual void set_center(pair point) {
		assert( 0 <= point.first  && point.first  < rows_ );
		assert( 0 <= point.second && point.second < cols_ );

		radius_ = (diameters_[point.first][point.second]-1)/2;
		// aggiorna la dimensione del vicinato
		upper_limit = (point.first  - radius_) >= 0     ? -radius_ : -point.first;
		lower_limit = (point.first  + radius_) <  rows_ ?  radius_ : rows_-1 - point.first;
		left_limit  = (point.second - radius_) >= 0     ? -radius_ : -point.second;
		right_limit = (point.second + radius_) <  cols_ ?  radius_ : cols_-1 - point.second;

		// aggiorna la posizione del blocco centrale
		central_point = point;

		// aggiorna la posizione del vertice in alto a sinistra
		topleft_point.first  = point.first + upper_limit;
		topleft_point.second = point.second + left_limit;

		// aggiorna la posizione del vertice in basso a destra
		downright_point.first  = point.first + lower_limit;
		downright_point.second = point.second + right_limit;
	}

	/*
	 * Il metodo restituisce la posizione ASSOLUTA del punto centrale del vicinato:
	 *   - Point_<int>::x  =  indice di riga
	 *   - Point_<int>::y  =  indice di colonna
	 */
	virtual pair central() const {
		return central_point;
	}

	/*
	 * Il metodo restituisce la posizione ASSOLUTA del vertice IN ALTO A SINISTRA del vicinato:
	 *   - Point_<int>::x  =  indice di riga
	 *   - Point_<int>::y  =  indice di colonna
	 */
	virtual pair topleft() const {
		return topleft_point;
	}

	/*
	 * Il metodo restituisce la posizione ASSOLUTA del vertice IN BASSO A DESTRA del vicinato:
	 *   - Point_<int>::x  =  indice di riga
	 *   - Point_<int>::y  =  indice di colonna
	 */
	virtual pair downright() const {
		return downright_point;
	}

	virtual int radius() const {
		return radius_;
	}

	virtual bool validate(pair point) const {
		return  (point.first  >= topleft_point.first) &&
				(point.first  <= downright_point.first) &&
				(point.second >= topleft_point.second) &&
				(point.second <= downright_point.second);
	}

};

class IteratorScan : public Iterator<Neighborhood::pair> {

 public:

	IteratorScan(const Neighborhood& parent_) : parent(parent_) {
		reset();
	}

	virtual void reset() {
		flagHasNext = true;
		position = parent.topleft();
		while (((!parent.validate(position)) && flagHasNext)) add();
	}

	virtual bool hasNext() {
		return flagHasNext;
	}

	virtual Neighborhood::pair next() {
		if (flagHasNext) {
			Neighborhood::pair ret = position;
			add();
			while ((!parent.validate(position) && flagHasNext)) add();
			return ret;
		} else {
			return std::make_pair(0,0);
		}
	}

 private:

	const Neighborhood& parent;
	bool flagHasNext;

	Neighborhood::pair position;
	void add() {
		if ((++position.second)>parent.downright().second) {
			position.second = parent.topleft().second;
			if ((++position.first)>parent.downright().first) {
				flagHasNext = false;
			}
		}
	}
 };

class IteratorScanFast : public Iterator<Neighborhood::pair> {

 public:

	IteratorScanFast(const Neighborhood& parent_) : parent(parent_) {
		reset();
	}

	virtual void reset() {
		flagHasNext = true;
		topleft   = parent.topleft();
		downright = parent.downright();
		position  = topleft;
	}

	virtual bool hasNext() {
		return flagHasNext;
	}

	virtual Neighborhood::pair next() {
		if (flagHasNext) {
			Neighborhood::pair ret = position;
			add();
			return ret;
		} else {
			return std::make_pair(0,0);
		}
	}

 private:

	const Neighborhood& parent;
	bool flagHasNext;
	Neighborhood::pair downright;
	Neighborhood::pair topleft;
	Neighborhood::pair position;

	inline void add() {
		if ((++position.second)>downright.second) {
			position.second = topleft.second;
			if ((++position.first)>downright.first) {
				flagHasNext = false;
			}
		}
	}
 };


class IteratorScan2Fast : public Iterator<Neighborhood::pair> {

 public:

	IteratorScan2Fast(const Neighborhood& parent_) : parent(parent_) {
		reset();
	}

	virtual void reset() {
		flagHasNext = true;
		flagPos   = true;
		topleft   = parent.topleft();
		downright = parent.downright();
		central   = parent.central();
		position.first  = central.first;
		position.second = topleft.second;
	}

	virtual bool hasNext() {
		return flagHasNext;
	}

	virtual Neighborhood::pair next() {
		if (flagHasNext) {
			Neighborhood::pair ret = position;
			add();
			return ret;
		} else {
			return std::make_pair(0,0);
		}
	}

 private:

	const Neighborhood& parent;
	bool flagHasNext;
	bool flagPos;
	Neighborhood::pair downright;
	Neighborhood::pair topleft;
	Neighborhood::pair central;
	Neighborhood::pair position;

	inline void add() {
		if ((++position.second)>downright.second) {
			position.second = topleft.second;
			if (flagPos) {
				if ((++position.first)>downright.first) {
					position.first = topleft.first;
					flagPos = false;
				}
			} else {
				if ((++position.first)>=central.first) {
					flagHasNext = false;
				}
			}
		}
	}
 };

class IteratorSpiral : public Iterator<Neighborhood::pair> {

 public:

	IteratorSpiral(const Neighborhood& parent_) : parent(parent_) {
		reset();
	}

	virtual void reset() {
		flagHasNext = true;
		delta = std::make_pair(0,0);
		position = parent.central();
		direction = 3;
		while (((!parent.validate(position)) && flagHasNext)) add();
	}

	virtual bool hasNext() {
		return flagHasNext;
	}

	virtual Neighborhood::pair next() {
		if (flagHasNext) {
			Neighborhood::pair ret = position;
			add();
			while ((!parent.validate(position) && flagHasNext)) add();
			return ret;
		} else {
			return std::make_pair(0,0);
		}
	}

 private:

	const Neighborhood& parent;
	bool flagHasNext;

	Neighborhood::pair position;
	Neighborhood::pair delta;
	int direction;
	/*
	 * direction: value from 0 to 4
	 *
	 *          2
	 *  +------->---+
	 *  | +--->---+ |
	 * 1^ | +---+ | |
	 *  | ^ | . | | |
	 *  | | +-+ | v |
	 *  | +---<-+ | v3
	 *  +---<-----+ |
	 *      0     --+
	 */

	void add() {
		if (delta.second==delta.first || delta.second==(-delta.first)) {
			direction++;
		}
		if (direction>3) {
			direction = 0;
			flagHasNext = ((++delta.first)<=parent.radius());
		} else if (direction==0) {
			delta.second--;
		} else if (direction==1) {
			delta.first--;
		} else if (direction==2) {
			delta.second++;
		} else if (direction==3) {
			delta.first++;
		}


		position.first  = delta.first  + parent.central().first;
		position.second = delta.second + parent.central().second;
	}
 };


class IteratorSpiralFast : public Iterator<Neighborhood::pair> {

 public:

	IteratorSpiralFast(const Neighborhood& parent_) : parent(parent_) {
		reset();
	}

	virtual void reset() {
		flagHasNext = true;
		central   = parent.central();
		topleft   = parent.topleft();
		downright = parent.downright();
		topleft.first    -= central.first;
		topleft.second   -= central.second;
		downright.first  -= central.first;
		downright.second -= central.second;
		delta = std::make_pair(0,0);
		position  = central;
		direction = 3;
		radius = 0;
	}

	virtual bool hasNext() {
		return flagHasNext;
	}

	virtual Neighborhood::pair next() {
		if (flagHasNext) {
			Neighborhood::pair ret = position;
			add();
			return ret;
		} else {
			return std::make_pair(0,0);
		}
	}

 private:

	const Neighborhood& parent;
	bool flagHasNext;

	Neighborhood::pair position;
	Neighborhood::pair delta;
	Neighborhood::pair central;
	Neighborhood::pair downright;
	Neighborhood::pair topleft;
	int radius;
	int direction;
	/*
	 * direction: value from 0 to 4
	 *
	 *          2
	 *  +------->---+
	 *  | +--->---+ |
	 * 1^ | +---+ | |
	 *  | ^ | . | | |
	 *  | | +-+ | v |
	 *  | +---<-+ | v3
	 *  +---<-----+ |
	 *      0     --+
	 */

	void add() {
		bool flagValidate;
		if (delta.second==delta.first || delta.second==(-delta.first)) {
			direction++;
		}
		if (direction>3) {
			direction = 0;
			radius++;
			flagHasNext = ((++delta.first)<=parent.radius());
		} else if (direction==0) {
			delta.second--;
		} else if (direction==1) {
			delta.first--;
		} else if (direction==2) {
			delta.second++;
		} else if (direction==3) {
			delta.first++;
		}

		if (flagHasNext) {
		do {
			flagValidate = false;
			if (delta.first  > downright.first) {
				if (direction!=0) radius++;
				flagValidate = true;
				delta.first  = downright.first;
				delta.second = -radius;
				direction    = 1;
			}
			if (delta.second < topleft.second) {
				if (direction==3) radius++;
				flagValidate = true;
				delta.first  = -radius;
				delta.second = topleft.second;
				direction    = 2;
			}
			if (delta.first  < topleft.first) {
				if (direction==0) radius++;
				flagValidate = true;
				delta.first  = topleft.first;
				delta.second = radius;
				direction    = 3;
			}
			if(delta.second > downright.second) {
				flagValidate = true;
				delta.second = downright.second;
				delta.first  = (++radius);
				direction    = 0;
			}
			if (flagValidate)
				flagValidate = flagHasNext  = (radius<=parent.radius());
		} while (flagValidate);
		}
		//if (topleft.second==0 && downright.first==18) {
		//	std::cout << delta.first <<'v' << delta.second << '\n';
		//	std::cout.flush();
		//}

		position.first  = delta.first  + central.first;
		position.second = delta.second + central.second;
	}
 };


#include "neighborhood.hpp"
#endif
