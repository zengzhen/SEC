/**
 * \file        similarityMeasure.h
 * \author      Zhen Zeng (zengzhen@umich.edu)
 * \brief       similarity measure of two semantic evant chains
 */

#ifndef SIMILARITYMEASURE_H
#define SIMILARITYMEASURE_H

#include <Eigen/Dense>

#include "SEC/eventChain.h"
#include <string.h>

namespace TableObject{
    
    class similarityMeasure{
    public:
        /** \brief empty Constructor
        */
        similarityMeasure(){};
        
        /** \brief Constructor
         *  \param[in] sec1 1st semantic event chain to be compared
         *  \param[in] sec2 2nd semantic event chain to be compared
        */
        similarityMeasure(const eventChain& sec1, const eventChain& sec2);
        
        /** \brief set threshold above which one would consider two spatial similarity values as equal
         *  \param[in] spatial_threshold threshold (eg. 0.8f)
        */
        void setSpatialThreshold(float spatial_threshold);
        
        /** \brief set threshold above which one would consider two temporal similarity values as equal
         *  \param[in] temporal_threshold threshold (eg. 0.8f)
        */
        void setTemporalThreshold(float temporal_threshold);

        /** \brief spatial similarity between two semantic event chains given while construction
        */
        float spatialSimilarity();
        
        /** \brief temporal similarity between two semantic event chains given while construction
        */
        float temporalSimilarity();
        
    private:
        eventChain _sec1;
        eventChain _sec2;
        float _spatial_threshold;
        float _temporal_threshold;
        
        std::vector<std::vector<float>> _ss;
        std::vector<std::vector<float>> _ts;
        
        /** \brief calculate max(similarity(shifted version of short string array, another long string array) )
         *  \param[in] row1 1st string array
         *  \param[in] row2 2nd string array
        */
        float subString(std::vector<std::string> row1, std::vector<std::string> row2);
        
        /** \brief recursive function to find all possible permutations of rows of compressed sec1 to match compressed_sec2
         *  \param[in] match_map row_i of compressed_sec1 is the same with row_match_map[i][j] of compressed_sec2
         *  \param[out] permutations all possible permutations where each row represents one possible permutation
         *                           in ith permutation, row_j of compressed_sec1 is assigned to row_permutations[i][j] of compressed_sec2
        */
        void permute(std::vector<std::vector<int>> match_map, std::vector<std::vector<int>>& permutations, std::vector<int> temp_permutation);
    };
}

#endif
    