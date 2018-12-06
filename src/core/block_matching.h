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
 * block_matching.h
 *
 *  Created on: 16/09/2012
 *      Author: Davide Cozzolino
 *
 */
#ifndef _BLOCK_MATCHING_H_
#define _BLOCK_MATCHING_H_
#include <utility>

template <typename PixelType>
struct BlockMatchOptions {
	int max_matched;		 // numero massimo di blocchi da selezionare
	PixelType max_distance;	 // distanza massima tra 2 blocchi

	BlockMatchOptions()
			: max_matched(0), max_distance(PixelType()) {}

	BlockMatchOptions( int matched, PixelType distance)
		: max_matched(matched), max_distance(distance) {}
};

template <typename Type, typename TypePoint>
class BlockMatchingList {

	struct BlockMatchingItem {
		Type dist; //distanze
		TypePoint point; //std::pair<int,int>
		BlockMatchingItem* next;
	};

	size_t size_;
	size_t max_size;
	Type max_dist;
	BlockMatchingItem* head;
	BlockMatchingItem* allocate;

 public:

	BlockMatchingList( size_t max_length, Type max_distance)
	: size_(0),max_size(max_length),max_dist(max_distance),
	  head(0),allocate(new BlockMatchingItem[max_length]) {};

	~BlockMatchingList() {
		delete allocate;
	}

	inline size_t size() {
		return size_;
	}
	inline void getMatchingList(std::vector< TypePoint > &dest, size_t Q) {
		BlockMatchingItem* pos = head;
		for(size_t i = size_; i>0;) {
			if ((--i)<Q) dest[i] = pos->point;
			pos = pos->next;
		}
	}

	inline void getMatchingList(std::vector< Type > &dest_dist, std::vector< TypePoint > &dest_point, size_t Q) {
			BlockMatchingItem* pos = head;
			for(size_t i = size_; i>0;) {
				if ((--i)<Q) {
					dest_point[i] = pos->point;
					dest_dist[i]  = pos->dist;
				}
				pos = pos->next;
			}
	}

	inline Type insert( Type dist, TypePoint point ) {
		BlockMatchingItem* pos_next = 0;
		BlockMatchingItem* item = 0;
		if (size_ < max_size) {
			if (dist > max_dist) {
				return max_dist;
			} else  {
				item = allocate+(size_++);
			}
		} else {
			if( dist >= (head->dist) ) {
				return head->dist;
			} else {
				item = head;
				head = item->next;
			}
		}

		if (head==0) {
			head = item;
		} else if ((head->dist)<dist) {
			pos_next = head;
			head = item;
		} else {
			BlockMatchingItem* pos = head;
			pos_next = pos->next;
			while (pos_next !=0  && (pos_next->dist)>dist) {
				pos = pos_next;
				pos_next = pos->next;
			}
			pos->next  = item;
		}

		item->dist  = dist;
		item->point = point;
		item->next  = pos_next;
		if (size_<max_size)
			return max_dist;
		else
			return head->dist;
	}
};



#endif
