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
        
        std::vector<std::vector<float>> _ss;
        
        float subString(std::vector<std::string> row1, std::vector<std::string> row2);
    };
}

#endif
    