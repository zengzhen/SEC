/**
 * \file        eventChain.h
 * \author      Zhen Zeng (zengzhen@umich.edu)
 * \brief       semantic evant chains operations
 */

#ifndef EVENTCHAIN_H
#define EVENTCHAIN_H

#include <Eigen/Dense>

#include "typeDef.h"

namespace TableObject{
    
    class eventChain{
    public:
        /** \brief empty Constructor
        */
        eventChain();
        
        /** \brief Constructor
        *   \param[in] mainGraph_file file to be load to read out main graph
        */
        eventChain(std::string mainGraph_file);
        
        /** \brief take derivative of the eventChain (rowwise)
         *  \brief remvoe rows where nothing ever happens
        */
        void takeDerivative();
        
        /** \brief compress the derivative of the eventChain (rowwise)
         *  \brief all '00', '22' are removed
        */
        void compress();
    
    private:
        sec _original_sec;
        sec _derivative_sec;
        sec _compressed_sec;
        
    };
}

#endif  // EVENTCHAIN_H