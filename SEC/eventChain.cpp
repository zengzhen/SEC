/**
 * \file        eventChain.cpp
 * \author      Zhen Zeng (zengzhen@umich.edu) 
 */

// struct sec{
//     int row;
//     std::vector<int> cols;
//     std::vector<std::vector<char*>> event_chain;
// };

#include "eventChain.h"
#include <fstream>

namespace TableObject{
    
    eventChain::eventChain()
    {
        _original_sec.row = 0;
        _derivative_sec.row = 0;
        _compressed_sec.row = 0;
    }

    eventChain::eventChain(std::string mainGraph_file)
    {
        std::string line;
        std::string relation;
        std::ifstream mainGraph(mainGraph_file);
        if (mainGraph.is_open())
        {
            while ( getline (mainGraph,line,'\n') )
            {
                std::istringstream str(line);
                std::vector<const char*> sec_row;
                while ( getline (str,relation,' ') )
                {
                    sec_row.push_back(relation.c_str());
                }
                _original_sec.event_chain.push_back(sec_row);
                _original_sec.row = +1;
                _original_sec.cols.push_back(sec_row.size());
            }
            mainGraph.close();
        }
    }
    
    void eventChain::takeDerivative()
    {
        for(int i=0; i<_original_sec.row; i++)
        {
            std::vector<const char*> sec_row;
            bool nothing_happens = true;
            for(int j=1; j<_original_sec.cols[i]; j++)
            {
                char str[80];
                std::strcpy(str, _original_sec.event_chain[i][j-1]);
                std::strcat(str, _original_sec.event_chain[i][j]);
                sec_row.push_back(str);
                
                nothing_happens = std::strcmp( _original_sec.event_chain[i][j-1] , _original_sec.event_chain[i][j] );
            }
            if(!nothing_happens)
            {
                _derivative_sec.event_chain.push_back(sec_row);
                _derivative_sec.row = +1;
                _original_sec.cols.push_back(_original_sec.cols[i]-1);
            }
        }
    }

    void eventChain::compress()
    {
        char term1[] = "00";
        char term2[] = "22";
        
        for(int i=0; i<_derivative_sec.row; i++)
        {
            std::vector<const char*> sec_row;
            
            for(int j=0; j<_derivative_sec.cols[i]; j++)
            {
                bool remove1 = false;
                bool remove2 = false;
                remove1 = std::strcmp( _derivative_sec.event_chain[i][j] , term1 );
                remove2 = std::strcmp( _derivative_sec.event_chain[i][j] , term2 );
                
                if(!remove1 & !remove2) sec_row.push_back(_derivative_sec.event_chain[i][j]);
            }
            _compressed_sec.event_chain.push_back(sec_row);
            _compressed_sec.row = +1;
            _compressed_sec.cols.push_back(sec_row.size());
        }
    }

}