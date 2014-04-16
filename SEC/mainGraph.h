/**
 * \file        mainGraph.h
 * \author      Zhen Zeng (zengzhen@umich.edu)
 * \brief       main graph of manipulation action
 */

#ifndef MAINGRAPH_H
#define MAINGRAPH_H

#include "typeDef.h"

namespace TableObject{
    
    class mainGraph {
    public:
        mainGraph(int start_index);
        void addInitialRelationalGraph(int relation);
        void addRelationalGraph(int relation);
        void compareRelationGraph(int frame_index);
        std::vector<graph> getMainGraph();
        void displayMainGraph();
        void recordMainGraph(std::string file_name);
        
    private:
        
        std::vector<graph> _graph; 
        std::vector<int> _cur_graph; //to store relational graph of current frame for comparison
        std::vector<int> _pre_graph; //to store relational graph of previous frame for comparison
        
    };
}

#endif // MAINGRAPH_H