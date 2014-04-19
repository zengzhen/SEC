/**
 * \file        similarityMeasure.cpp
 * \author      Zhen Zeng (zengzhen@umich.edu) 
 */

#include "similarityMeasure.h"

namespace TableObject{

    similarityMeasure::similarityMeasure(const eventChain& sec1, const eventChain& sec2)
    {
        _sec1 = sec1;
        _sec2 = sec2;
    }
    
    void similarityMeasure::setSpatialThreshold(float spatial_threshold)
    {
        _spatial_threshold = spatial_threshold*100;
    }

    float similarityMeasure::spatialSimilarity()
    {
        sec comSec1, comSec2;
        _sec1.getCompressedSec(comSec1);
        _sec2.getCompressedSec(comSec2);
        
        // calculate similarity measure for each row of derSec1 vs. each row of derSec2
        std::cout << "calculating spatial similarity (threshold=" << _spatial_threshold << ")" << std::endl;
        float ss = 0;
        for(int i=0; i<comSec1.row; i++)
        {
            float ss_i = 0;
            std::vector<float> ss_i_vector;
            std::cout << "ss[" << i << "][:] = ";
            for(int j=0; j<comSec2.row; j++)
            {
                float ss_ij = subString(comSec1.event_chain[i], comSec2.event_chain[j]);
                if(ss_ij > ss_i) ss_i = ss_ij;
                ss_i_vector.push_back(ss_ij);
            }
            std:: cout << "max_f = " << ss_i << std::endl; 
            
            ss = ss + ss_i;
            _ss.push_back(ss_i_vector);
        }
        
        // calculate total spatial similarity
        ss = ss / comSec1.row;
        std::cout << std::endl;
        std::cout << "spatial simlarity = " << ss << std::endl;
    }

    float similarityMeasure::temporalSimilarity()
    {
        sec derSec1, derSec2;
        _sec1.getDerivativeSec(derSec1);
        _sec2.getDerivativeSec(derSec2);
        
        // find possible permutations based on the best match from spatial similarity measure
        std::vector<std::vector<int>> permutation;
        if(derSec1.row <= derSec2.row)
        {
            // permutation has # of row = derSec1.row
            for(int i=0; i<derSec1.row; i++)
            {
                std::vector<int> match_idx;
                float similarity = 0;
                // find the best match similarity for each row of sec1
                for(int j=0; j<derSec2.row; j++)
                {
                    if(_ss[i][j]>similarity) similarity = _ss[i][j];
                }
                // find all the best matches for each row of sec1
                for(int j=0; j<derSec2.row; j++)
                {
                    if(_ss[i][j]==similarity) match_idx.push_back(j);
                }
                permutation.push_back(match_idx);
            }
        }else{
            // permutation has # of row = derSec2.row
            for(int i=0; i<derSec2.row; i++)
            {
                std::vector<int> match_idx;
                float similarity = 0;
                // find the best match similarity for each row of sec1
                for(int j=0; j<derSec1.row; j++)
                {
                    if(_ss[j][i]>similarity) similarity = _ss[j][i];
                }
                // find all the best matches for each row of sec1
                for(int j=0; j<derSec1.row; j++)
                {
                    if(_ss[j][i]==similarity) match_idx.push_back(j);
                }
                permutation.push_back(match_idx);
            }
        }
        
        
    }
    
    float similarityMeasure::subString(std::vector< std::string> row1, std::vector< std::string> row2)
    {
        int length1 = row1.size();
        int length2 = row2.size();
        int shift_times = std::abs<int>(length1-length2) + 1;
    
        //row1 length < row 2 length (swap if necessary))
        if(length1 > length2) row1.swap(row2);
        int normalization = row2.size();
        
        //sub-string search: row1 vs row2[shift:shift+(length1-1))]]
        float f_shift[shift_times];
        float f = 0;
        for(int i=0; i<shift_times; i++)
        {
            int count = 0;
            for(int j=0; j<row1.size(); j++)
            {
                int index1 = j;
                int index2 = i+j;
                if(std::strcmp(row1[index1].c_str(), row2[index2].c_str())==0) count = count + 1;
            }
            f_shift[i] = (float)100/normalization*count;
//             std::cout << f_shift[i] << " ";
            
            if(f_shift[i] > f) f = f_shift[i];
        }
        std::cout << f;
        if(f >= _spatial_threshold)
        {
            f=100;
            std::cout << "->" << f << " ";
        }else{
            std::cout << " ";
        }
        
        return f;
    }
}