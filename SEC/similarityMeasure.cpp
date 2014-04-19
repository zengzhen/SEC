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
    
    void similarityMeasure::setTemporalThreshold(float temporal_threshold)
    {
        _temporal_threshold = temporal_threshold*100;
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
        
        // find all pairs of matches between derSec1, derSec2
        // store in match_map ( # row <= # col )
        std::vector<std::vector<int>> match_map;
        if(derSec1.row <= derSec2.row)
        {
            // match_map has # of row = derSec1.row
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
                match_map.push_back(match_idx);
            }
        }else{
            // match_map has # of row = derSec2.row
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
                match_map.push_back(match_idx);
            }
        }
        
        // display the matching
        for(int i=0; i<match_map.size(); i++)
        {
            for(int j=0;j<match_map[i].size(); j++)
            {
                std::cout << match_map[i][j] << " ";
            }
            std::cout << std::endl;
        }
        
//         // contruct test match_map
//         match_map.clear();
//         int row1[] = {0}; std::vector<int> map1 (row1, row1 + sizeof(row1) / sizeof(int) ); match_map.push_back(map1);
//         int row2[] = {0}; std::vector<int> map2 (row2, row2 + sizeof(row2) / sizeof(int) ); match_map.push_back(map2);
//         int row3[] = {1,2}; std::vector<int> map3 (row3, row3 + sizeof(row3) / sizeof(int) ); match_map.push_back(map3);
//         int row4[] = {3,4}; std::vector<int> map4 (row4, row4 + sizeof(row4) / sizeof(int) ); match_map.push_back(map4);
        
        // find all possible permutations
        std::vector<std::vector<int>> permutation;
        std::vector<int> temp_permutation;
        permute(match_map, permutation, temp_permutation);
        
        // display permutations
        std::cout << "permutations: " << std::endl;
        for(int i=0; i<permutation.size(); i++)
        {
            for(int j=0; j<permutation[i].size(); j++)
            {
                std::cout << permutation[i][j] << " ";
            }
            std::cout << std::endl;
        }
        
//         sec temp_derSec1; std::memcpy(&temp_derSec1, &derSec2, sizeof(derSec2));
//         std::memcpy(&derSec2, &derSec1, sizeof(derSec1));
//         std::memcpy(&derSec1, &temp_derSec1, sizeof(temp_derSec1));
        
        // construct premutations
        std::cout << "calculating temporal similarity (threshold=" << _temporal_threshold << ")" << std::endl;
        std::vector<sec> permutated_derSec1_seq;
        std::vector<sec> permutated_derSec2_seq;
        if(derSec1.row <= derSec2.row)
        {
            derSec1.display();
            for(int p=0; p<permutation.size(); p++)
            {
                // construct the derivative sec for each permutation
                sec permutated_derSec2;
                permutated_derSec2.row = 0;
                for(int i=0; i<permutation[p].size(); i++)
                {
                    std::vector<std::string> permutated_row;
                    // fill dummy entries for short col
                    if(permutation[p][i]==-1)
                    {
                        for(int j=0; j<derSec2.event_chain[0].size(); j++) permutated_row.push_back("dummy");
                    }else{
                        permutated_row = derSec2.event_chain[permutation[p][i]];
                    }
                    permutated_derSec2.event_chain.push_back(permutated_row);
                    permutated_derSec2.cols.push_back(permutated_row.size());
                    permutated_derSec2.row = permutated_derSec2.row + 1;
                }
                std::cout << "permutation " << p << ":" << std::endl;
                permutated_derSec2.display();
                permutated_derSec2_seq.push_back(permutated_derSec2);
            }
            permutated_derSec1_seq.push_back(derSec1);
        }else{
            derSec2.display();
            for(int p=0; p<permutation.size(); p++)
            {
                // construct the derivative sec for each permutation
                sec permutated_derSec1;
                permutated_derSec1.row = 0;
                for(int i=0; i<permutation[p].size(); i++)
                {
                    std::vector<std::string> permutated_row;
                    // fill dummy entries for short col
                    if(permutation[p][i]==-1)
                    {
                        for(int j=0; j<derSec1.event_chain[0].size(); j++) permutated_row.push_back("dummy");
                    }else{
                        permutated_row = derSec1.event_chain[permutation[p][i]];
                    }
                    permutated_derSec1.event_chain.push_back(permutated_row);
                    permutated_derSec1.cols.push_back(permutated_row.size());
                    permutated_derSec1.row = permutated_derSec1.row + 1;
                }
                std::cout << "permutation " << p << ":" << std::endl;
                permutated_derSec1.display();
                permutated_derSec1_seq.push_back(permutated_derSec1);
            }
            permutated_derSec2_seq.push_back(derSec2);
        }
        
         // for each permutation, find the similarity matrix
         // permutated_derSec1_seq vs permutated_derSec2_seq
        for(int p=0; p<permutation.size(); p++)
        {
            std::cout << "permutation " << p << ":" << std::endl;
            sec newDerSec1, newDerSec2;
            if(derSec1.row <= derSec2.row) 
            {
                newDerSec1 = permutated_derSec1_seq[0];
                newDerSec2 = permutated_derSec2_seq[p];
            }else{
                newDerSec1 = permutated_derSec1_seq[p];
                newDerSec2 = permutated_derSec2_seq[0];
            }
            
            std::vector<std::vector<float>> similarity_matrix;
            float ts = 0;
            for(int i=0; i<newDerSec1.cols[0]; i++)
            {
                float ts_i = 0;
                std::vector<float> ts_i_vector;
                std::cout << "ts[" << i << "][:] = ";
                for(int j=0; j<newDerSec2.cols[0]; j++)
                {
                    // extract the cols from each newDerSec
                    std::vector<std::string> col1; 
                    std::vector<std::string> col2;
                    for(int row_i=0; row_i<newDerSec1.row; row_i++) col1.push_back(newDerSec1.event_chain[row_i][j]);
                    for(int row_i=0; row_i<newDerSec2.row; row_i++) col2.push_back(newDerSec2.event_chain[row_i][j]);
                    
                    // calculate the similarity between col1 and col2
                    float ts_ij = subString( col1, col2 );
                    if(ts_ij > ts_i) ts_i = ts_ij;
                    ts_i_vector.push_back(ts_ij);
                }
                std:: cout << "max_f = " << ts_i << std::endl; 
                
                ts = ts + ts_i;
                _ts.push_back(ts_i_vector);
            }
            
            // calculate total temporal similarity
            ts = ts / newDerSec1.cols[0];
            std::cout << std::endl;
            std::cout << "temporal simlarity = " << ts << std::endl;
        }
        return 0;
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
    
    void similarityMeasure::permute(std::vector< std::vector< int > > match_map, std::vector< std::vector< int > >& permutations, std::vector<int> temp_permutation)
    {
        // check for update of match_map
        if(!temp_permutation.empty())
        {
            // find which rows has already been assigned
            int count = 0;
            for(int i=0; i<temp_permutation.size(); i++)
            {
                if(temp_permutation[i] != -1)
                {
                    // update match_map 
                    // 1. entries equal to temp_permutation[i] set to -1)
                    // 2. row_i of match_map set to -1
                    for(int p=0; p<match_map.size(); p++)
                    { 
                        for(int q=0; q<match_map[p].size(); q++)
                        {
                            // check whether several rows match to only one row
                            if(match_map[p].size()==1 & match_map[p][q] == temp_permutation[i] & p!=i)
                            {
                                std::vector<int> temp_permutation_special = temp_permutation;
                                temp_permutation_special[p] = temp_permutation[i];
                                temp_permutation_special[i] = -1;
                                permute(match_map, permutations, temp_permutation_special);
//                                 return;
                            }
                            if(match_map[p][q] == temp_permutation[i] | p==i)
                            {
                                match_map[p][q] = -1;
                            }
                        }
                    }
                    count = count + 1;
                }
            }
            // check terminal condition
            if(count == temp_permutation.size()) 
            {
                permutations.push_back(temp_permutation);
                return;
            }
        }
        
        // get # of matches for each row
        std::vector<int> match_num;
        for(int i=0; i<match_map.size(); i++)
        {
            match_num.push_back(0);;
            for(int j=0; j<match_map[i].size(); j++)
            {
                if(match_map[i][j] != -1) match_num[i] = match_num[i] + 1;
            }
        }
        
        // find rows with least # of matches
        int min_match_num = 1000;
        for(int i=0; i<match_map.size(); i++)
        {
            if( match_num[i] < min_match_num & match_num[i] > 0 ) min_match_num = match_num[i];
        }
        // check for terminal condition
        if( min_match_num == 1000)
        {
            permutations.push_back(temp_permutation);
            return;
        }
        int min_match_row; // index of the 1st row that has the least # of matches
        for(int i=0; i<match_map.size(); i++)
        {
            if( match_num[i] == min_match_num)
            {
                min_match_row = i;
                break;
            }
        }
        
        // start permutations from the min_match_row
        for(int i=0; i<match_map[min_match_row].size(); i++)
        {
            if(temp_permutation.empty()) temp_permutation = std::vector< int >(match_map.size(), -1);
            
            if( match_map[min_match_row][i] != -1) 
            {
                temp_permutation[min_match_row] = match_map[min_match_row][i];
                permute(match_map, permutations, temp_permutation);
            }
            
        }
    }

}