/**
 * \file        actionClustering.cpp
 * \author      Zhen Zeng (zengzhen@umich.edu)
 * \brief       measure the similarities between every pair of action instances in the dataset
 */

#include "SEC/similarityMeasure.h"

int main(int argc, char** argv)
{
    if(argc<2 | argc >3)
    {
        std::cerr << "Usage: ./actionClustering DATASET_PATH (opt)threshold(60)" << std::endl;
        exit(1);
    }
    
    float threshold = 60;
    if(argc==3) threshold = std::stof(argv[2]);
    
    
}