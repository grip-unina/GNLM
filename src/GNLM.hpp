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
 * nlmeansDuoHard.hpp
 *
 *  Created on: 03/05/2018
 *      Author: Davide Cozzolino
 */

#ifndef _NLMEANSG_DUOHARD_HPP_
#define _NLMEANSG_DUOHARD_HPP_
#include <limits>
#include "core/block_matching.h"
#include "core/block_matching_duo.hpp"
#include "core/aggregation.h"
#include "core/collaborative_means.h"
#include "utils/buffers.h"
#include "utils/accessors.h"
#include "utils/stripe.h"
#include "utils/stripe_mat.h"
#include "utils/stepper.h"
#include "core/speckle/distanceSar_int_sum.hpp"
#include "core/awgn/distanceAwgnVec.h"
#include "core/awgn/distanceAwgn.h"



template <typename PixelType>
struct GuidedNLMeansProfile : BlockMatchOptions<PixelType>, AggregationOptions<PixelType>
{

	/* parametri di base */
	int block_rows;				// numero di righe del singolo blocco
	int block_cols;				// numero di colonne del singolo blocco
	int step;					// distanza tra i "reference block"
	int search_diameter;		// diametro della zona di ricerca
    
	/* parametri aggiuntivi */
	PixelType alpha;
    PixelType thDist;
    PixelType lambda1;
    PixelType lambda2;
    
	/* costruttore */
	GuidedNLMeansProfile() {
		config(8, 64, 39, 3, std::numeric_limits<PixelType>::infinity(), 2.0, 
                0.5, std::numeric_limits<PixelType>::infinity(), 1, 1);
	}

	/*
	 * Il metodo configura i parametri dell'algoritmo:
	 *   - N1        = numero di righe/colonne del singolo blocco
	 *   - N2        = lunghezza max dello stack
	 *   - Ns        = diametro del vicinato (cio�, l'ampiezza della zona di ricerca per il block-matching)
	 *   - Nstep     = distanza tra i "reference block"
	 *   - tau_match = [BLOCK MATCHING] parametro per la distanza massima tra 2 blocchi
	 *   - aggregationBeta      = [AGGREGATION] parametro della finestra di Kaiser
	 */
	    
    void config(int N1, int N2, int Ns, int Nstep, PixelType tau_match, PixelType aggregationBeta,
          PixelType alpha_, PixelType thDist_, PixelType lambda1_, PixelType lambda2_) {
		block_rows = N1;   				 // num. di righe del singolo blocco
		block_cols = N1;   				 // num. di colonne del singolo blocco
		search_diameter = Ns;	      	 // diametro del vicinato
		step = Nstep;				     // distanza tra i "reference block"
		alpha = alpha_;	 // alpfa factor in [0,1]
        thDist = thDist_;
        lambda1 = lambda1_;			// 
        lambda2 = lambda2_;
		BlockMatchOptions<PixelType> ::max_matched = N2;					// numero massimo di blocchi da considerare
		BlockMatchOptions<PixelType> ::max_distance = tau_match * N1*N1;	// soglia sulla distanza massima tra 2 blocchi
		AggregationOptions<PixelType>::win2D.config("kaiser",N1,N1,aggregationBeta); // finestra di 'Kaiser' con parametro 'beta'

	}


};

#ifdef TIME_INFO
	#include "utils/timer.h"
	#include "utils/time_info.h"
#endif

	
template <typename PixelType1, typename PixelType2, typename OpDistance1, typename OpDistance2>
#ifndef TIME_INFO
void
#else
double
#endif
guided_nlmeans(const cv::Mat_<PixelType1> &noisy_image, 
            const cv::Mat_<PixelType2> &guida_image, 
            const cv::Mat_<bool> &class_image, 
            cv::Mat_<PixelType1> &clean_image,
            cv::Mat_<PixelType1> &sum_image,
            const GuidedNLMeansProfile<PixelType1> &opt)
{
	#ifdef TIME_INFO
		Timer timer;
		double time_init   = 0;
		double time_t2d    = 0;
		double time_it2d   = 0;
		double time_t1d    = 0;
		double time_it1d   = 0;
		double time_block  = 0;
		double time_filter = 0;
		double time_aggre  = 0;
		timer.start();
	#endif

	typedef Sliding_Accessor<PixelType1> sliding1_t;
    typedef Sliding_Accessor<PixelType2> sliding2_t;
	typedef Stack_Buffer<PixelType1> stack_t;

	// immagine d'uscita
	clean_image = PixelType1();  							 // azzera i pixel
	sum_image   = PixelType1();  							 // azzera i pixel

	// immagine dei pesi (per la fase di 'aggregation')
	cv::Mat_<PixelType1> weights_image( noisy_image.size() ); // crea la matrice
	weights_image = PixelType1();  							 // azzera i pixel

	// suddividi le immagini in blocchi "sliding"
	sliding1_t noisy_blocks( (cv::Mat_<PixelType1> &) noisy_image, opt.block_rows, opt.block_cols );
    sliding2_t guida_blocks( (cv::Mat_<PixelType2> &) guida_image, opt.block_rows, opt.block_cols );
	sliding1_t clean_blocks( clean_image, opt.block_rows, opt.block_cols );
	sliding1_t weights_blocks( weights_image, opt.block_rows, opt.block_cols );

	// vicinato
	NeighborhoodRect neighborhood( noisy_blocks.rows(),  noisy_blocks.cols() , opt.search_diameter );

	// distance
    OpDistance1 funDistance1(opt);
    OpDistance2 funDistance2(opt);
    
    std::vector< std::pair<int,int> > matched;
	std::vector< PixelType1 > matched_dist;

	// stack di 'appoggio'
	stack_t stack(opt.block_rows, opt.block_cols, opt.max_matched);

	/* scorri i "reference block" */
	Stepper stepper( noisy_image.rows, noisy_image.cols, opt.block_rows, opt.block_cols, opt.step );
	#ifdef TIME_INFO
		time_init += timer.stop();
		timer.start();
	#endif
	for( int row = stepper.begin_row(); stepper.has_row(); row = stepper.next_row() ) {
		// aggiorna il buffer
		//noisy_log.move_forward(row);
		for( int col = stepper.begin_col(); stepper.has_col(); col = stepper.next_col() ) {
			// aggiorna il vicinato
			neighborhood.set_center(std::make_pair(row, col));
			#ifdef TIME_INFO
				time_init += timer.stop();
				timer.start();
			#endif

            
            // block matching
            block_matching_duo_th(neighborhood, opt.alpha, opt.thDist,  
                funDistance1, funDistance2, noisy_blocks, guida_blocks, 
                opt.lambda1, opt.lambda2, matched, matched_dist, class_image);

            int Nb = matched.size();

            stack.fromBlock(noisy_blocks, matched, Nb);
            #ifdef TIME_INFO
                time_block += timer.stop();
                timer.start();
            #endif

            // collaborative filtering
            PixelType1 w_sum = collaborative_means(stack, matched_dist, PixelType1(1.0), Nb);
            #ifdef TIME_INFO
                time_filter += timer.stop();
                timer.start();
            #endif
            sum_image(row, col) = w_sum;
            PixelType1 scale = (PixelType1)Nb;

            // prima fase di 'aggregation'
            aggregation1( stack[0], matched[0], scale, clean_blocks, weights_blocks, opt );
            #ifdef TIME_INFO
                time_aggre += timer.stop();
                timer.start();
            #endif

		}
	}

	// seconda fase di 'aggregation'
	clean_image /= weights_image;
	#ifdef TIME_INFO
		time_aggre += timer.stop();
	#endif

	#ifdef TIME_INFO
		#ifdef TIME_INFO_PRINT
			printf("time   init: %7f \n", time_init  );
			printf("time     BM: %7f \n", time_block );
			printf("time    T2D: %7f \n", time_t2d   );
			printf("time    T1D: %7f \n", time_t1d   );
			printf("time   filt: %7f \n", time_filter);
			printf("time    T1D: %7f \n", time_it1d  );
			printf("time  I-T2D: %7f \n", time_it2d  );
			printf("time  aggr.: %7f \n", time_aggre );
		#endif
		return time_block;
	#endif

}


template <typename PixelType1, typename OpDistance1>
#ifndef TIME_INFO
void
#else
double
#endif
get_num_block_matching_duo_th(const cv::Mat_<PixelType1> &noisy_image, 
            const cv::Mat_<bool> &class_image, 
            cv::Mat_<int> &count_image,
            const GuidedNLMeansProfile<PixelType1> &opt) {

	typedef Sliding_Accessor<PixelType1> sliding1_t;
	
	// immagine d'uscita
	count_image = 0;  			// azzera i pixel
	cv::Mat_<PixelType1> guida_image( noisy_image.size() );
	guida_image = PixelType1(); // azzera i pixel

	// suddividi le immagini in blocchi "sliding"
	sliding1_t noisy_blocks( (cv::Mat_<PixelType1> &) noisy_image, opt.block_rows, opt.block_cols );
    sliding1_t guida_blocks( (cv::Mat_<PixelType1> &) guida_image, opt.block_rows, opt.block_cols );

	// vicinato
	NeighborhoodRect neighborhood( noisy_blocks.rows(),  noisy_blocks.cols() , opt.search_diameter );

	// distance
    OpDistance1 funDistance1(opt);
    DistanceAwgn<PixelType1> funDistance2(opt);
    
    std::vector< std::pair<int,int> > matched;
	std::vector< PixelType1 > matched_dist;


	/* scorri i "reference block" */
	Stepper stepper( noisy_image.rows, noisy_image.cols, opt.block_rows, opt.block_cols, opt.step );
	for( int row = stepper.begin_row(); stepper.has_row(); row = stepper.next_row() ) {
		// aggiorna il buffer
		//noisy_log.move_forward(row);
		for( int col = stepper.begin_col(); stepper.has_col(); col = stepper.next_col() ) {
			// aggiorna il vicinato
			neighborhood.set_center(std::make_pair(row, col));

            // block matching
            block_matching_duo_th(neighborhood, opt.alpha, opt.thDist,  
                funDistance1, funDistance2, noisy_blocks, guida_blocks, 
                opt.lambda1, opt.lambda2, matched, matched_dist, class_image);

            count_image(row, col) = matched.size();

		}
	}


}

#endif
