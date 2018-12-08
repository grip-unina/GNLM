// GNLM - Guided Non-Local Means
// Date released 10/12/2018, version BETA.
// Code for the guided denoising of a SAR image corrupted
// by multiplicative speckle noise with the technique described in 
// "Guided patch-wise nonlocal SAR despeckling", 
// written by Sergio Vitale, Davide Cozzolino, Giuseppe Scarpa, Luisa Verdoliva and Giovanni Poggi, 
// Submitted, 2018.
// Please refer to this papers for a more detailed description of the algorithm.
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// Copyright (c) 2018 Image Processing Research Group of University Federico II of Naples ('GRIP-UNINA').
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
 * block_matching_duo.hpp
 *
 *  Created on: 30/10/2018
 *      Author: Davide Cozzolino
 *
 *
 */
#ifndef _BLOCK_MATCHING_DUO_HPP_
#define _BLOCK_MATCHING_DUO_HPP_

#include "../utils/accessors.h"
#include "../utils/neighborhood.h"
#include <assert.h>

template <typename TypeDist, typename TypeData, typename TypePoint>
        class BlockMatchingDataList {
    
    struct BlockMatchingItem {
        TypeDist dist; //distanze
        TypeData data; //data
        TypePoint point; //std::pair<int,int>
        BlockMatchingItem* next;
    };
    
    size_t size_;
    size_t max_size;
    TypeDist max_dist;
    BlockMatchingItem* head;
    BlockMatchingItem* allocate;
    
        public:
            
            BlockMatchingDataList( size_t max_length, TypeDist max_distance)
            : size_(0),max_size(max_length),max_dist(max_distance),
                    head(0),allocate(new BlockMatchingItem[max_length]) {};
                    
                    ~BlockMatchingDataList() {
                        delete allocate;
                    }
                    
                    inline size_t size() {
                        return size_;
                    }
                    
                    inline void getMatchingList(std::vector< TypeData > &dest_data ,
                            std::vector< TypePoint > &dest_point, size_t Q) {
                        BlockMatchingItem* pos = head;
                        for(size_t i = size_; i>0;) {
                            if ((--i)<Q) {
                                dest_data[i]  = pos->data;
                                dest_point[i] = pos->point;
                            }
                            pos = pos->next;
                        }
                    }
                    
                    inline TypeDist insert( TypeDist dist, TypeData data, TypePoint point ) {
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
                        item->data  = data;
                        item->point = point;
                        item->next  = pos_next;
                        if (size_<max_size)
                            return max_dist;
                        else
                            return head->dist;
                    }
};

template <typename BlockAccessor1, typename BlockAccessor2, typename OpDistance1, typename OpDistance2>
        void block_matching_duo_th(const Neighborhood& neighborhood, typename OpDistance1::DistanceType alpha1,
        typename OpDistance1::DistanceType th1,
        const OpDistance1 &opt1, const OpDistance2 &opt2,
        const BlockAccessor1 &src1, const BlockAccessor2 &src2,
        typename OpDistance1::DistanceType lambda1, typename OpDistance2::DistanceType lambda2,
        std::vector< std::pair<int,int> > &dest_point,
        std::vector<typename OpDistance1::DistanceType> &dest_dist,
        const cv::Mat_<bool> &valClass) {
    
    typedef typename BlockAccessor1::pixel_type pixel1_t;
    typedef typename BlockAccessor2::pixel_type pixel2_t;
    typedef typename    OpDistance1::DistanceType dist_t;
    
    
    dist_t alpha2 = 1.0 - alpha1;
    dist_t max_distance = opt1.max_distance;
    size_t Ncheck = 0;
    size_t NcheckTh1 = 0;
    size_t NcheckThEq = 0;
    BlockMatchingDataList<dist_t, dist_t, std::pair<int,int> > list(opt1.max_matched, max_distance);
    
    if (valClass(neighborhood.central().first, neighborhood.central().second)) {
        
        // blocco di riferimento
        cv::Mat_<pixel1_t> ref_1block = src1(neighborhood.central().first, neighborhood.central().second);
        cv::Mat_<pixel2_t> ref_2block = src2(neighborhood.central().first, neighborhood.central().second);
        IteratorScan2Fast iter(neighborhood);    
        //IteratorSpiralFast iter(neighborhood);
        
        if (alpha1==0) {
            dist_t max_distance1 = opt1.max_distance;
            max_distance = opt2.max_distance;
            
            //mexPrintf("zero %g %g %g %g %g %g %d\n", alpha1, alpha2, th1, max_distance, max_distance/alpha1, 0/alpha1, opt1.max_matched);
            
            while (iter.hasNext()) {
                std::pair<int,int> pos = iter.next();
                if (valClass(pos.first,pos.second)) {
                    Ncheck++;
                    dist_t dist1 = opt1.computeDistance(src1(pos.first,pos.second), ref_1block, max_distance1);
                    if (dist1<th1) {
			NcheckTh1++;
                        dist_t distS = opt2.computeDistance(src2(pos.first,pos.second), ref_2block, max_distance);
                        max_distance = list.insert( distS, lambda1*dist1+lambda2*distS, pos );
                    }
                }
            }
            
            
        } else if (alpha1==1) {
            dist_t max_distance2 = opt2.max_distance;
            max_distance = opt1.max_distance;
            
            //mexPrintf("uno %g %g %g %g %g %g %d\n", alpha1, alpha2, th1, max_distance, max_distance/alpha1, 0/alpha1, opt1.max_matched);
            
            while (iter.hasNext()) {
                std::pair<int,int> pos = iter.next();
                if (valClass(pos.first,pos.second)) {
                    Ncheck++;
                    dist_t distS = opt1.computeDistance(src1(pos.first,pos.second), ref_1block, max_distance);
                    if (distS<th1) {
			NcheckTh1++;
                        dist_t dist2 = opt2.computeDistance(src2(pos.first,pos.second), ref_2block, max_distance2);
                        // considera il blocco solo se la distanza � minore della soglia
                        max_distance = list.insert( distS, lambda1*distS+lambda2*dist2, pos );
                    }
                }
            }
            
            
        } else {
            max_distance = alpha1*opt1.max_distance+alpha2*opt2.max_distance;
            //mexPrintf("dual %g %g %g %g %g %g %d\n", alpha1, alpha2, th1, max_distance, max_distance/alpha1, 0/alpha1, opt1.max_matched);
            
            while (iter.hasNext()) {
                std::pair<int,int> pos = iter.next();
                if (valClass(pos.first,pos.second)) {
                    Ncheck++;
                    dist_t dist1 = opt1.computeDistance(src1(pos.first,pos.second), ref_1block, max_distance/alpha1);
                    dist_t dist2 = 0;
                    dist_t distS = max_distance;
                    if (dist1<th1) {
			NcheckTh1++;
                        dist2 = opt2.computeDistance(src2(pos.first,pos.second), ref_2block, max_distance/alpha2);
                        distS = alpha1*dist1+alpha2*dist2;
                        // considera il blocco solo se la distanza � minore della soglia
                        max_distance = list.insert( distS, lambda1*dist1+lambda2*dist2, pos );
                    }
                }
            }
                        
        }
    } else {
        Ncheck++;
        list.insert( 0.0, 1.0, neighborhood.central());
    }
    
    size_t N = list.size();
    //if ((N<opt1.max_matched) && (N<Ncheck)) {
    //    mexPrintf("info N %d; Nmax  %d; Nchecked %d; NlowTH %d (%g) \n", N, opt1.max_matched, Ncheck, NcheckTh1, th1);
    //}
    dest_point.resize(N);
    dest_dist.resize(N);
    list.getMatchingList(dest_dist, dest_point, N);
    
    
}

#endif
