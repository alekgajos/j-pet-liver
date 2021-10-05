#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-label"
#include "unpacker.hpp"
#pragma GCC diagnostic pop

#include "setup.h"

int main(int // argc
         , char* argv[]) {

  // load setup from JSON file
  auto filename = "../data/test_modular_setup_v10.json";
  
  Setup setup(filename);
  
  std::ifstream fp( argv[1], std::ios::in | std::ios::binary );

    std::vector<unsigned int> data;

    unpacker::meta_t meta_data;
    std::map<unsigned int, std::vector<unpacker::hit_t>> original_data;
    std::map<unsigned int, std::vector<unpacker::hit_t>> filtered_data;
    std::map<unsigned int, std::vector<unpacker::sigmat_t>> preproc_data;
    std::map<unsigned int, std::string> paths_to_tdc_calib;
    
    // load TDC calibration
    for (int i=1; i<=4; i++) {
      for (int j=1; j<=12; j++)
        {
          unsigned int id = i << 4 | j;
          std::stringstream ss;
          ss << std::hex << id;
          std::string s_id;
          ss >> s_id;    
          // std::string path_to_calib = "/storage/tdc_calib/A" + s_id + "0_tdc_calib.txt";
          std::string path_to_calib = "../data/calib/0xa" + s_id + "0_calib.txt";
          paths_to_tdc_calib[ 0xa<<12 | i<<8 | j<< 4 ] = path_to_calib;
        }     
    }
    
    int succ = 1;
    double max_time = -1;

    while( succ ) {
      succ = unpacker::get_time_window(meta_data,
                                       original_data,
                                       filtered_data,
                                       preproc_data,
                                       paths_to_tdc_calib,
                                       fp);
      
      for (auto const &pair: original_data){
        //          printf("{%02x\n", pair.first);
          
          for (auto const &val : pair.second){
            //            printf("\t%02x (%d, %d, %.0f),\n", val.sample,
            //                     val.is_falling_edge,
            //                     val.channel_id,
            //                     val.time);

            if(val.time >max_time) max_time = val.time;

          }
          
      //          printf("}\n");
        }
      
      //      break;
    }

    std::cout << max_time << std::endl;
  
  
}
