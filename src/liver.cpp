#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

#include <boost/format.hpp>
#include <boost/histogram.hpp>

#define SUPPRESS_TRIGGER_MISMATCH_ERROR
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-label"
#include "unpacker.hpp"
#pragma GCC diagnostic pop

#include "setup.h"
#include "signals.h"

int main(int // argc
         , char* argv[]) {

  // load setup from JSON file
  auto filename = "../data/test_modular_setup_v10.json";

  Setup setup(filename);
  
  std::ifstream fp( argv[1], std::ios::in | std::ios::binary );

    std::vector<unsigned int> data;

    unpacker::meta_t meta_data;
    std::map<unsigned int, ENDPData> original_data;
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
    //    auto h = boost::histogram::make_histogram(boost::histogram::axis::regular<>(100, 0., 1.5e8, "t"));
    
    TimesByMatrix times;

    while( succ ) {
      succ = unpacker::get_time_window(meta_data,
                                       original_data,
                                       filtered_data,
                                       preproc_data,
                                       paths_to_tdc_calib,
                                       fp);
      
      
      
      for (auto const &pair: original_data){

        assembleSignals(pair.first, pair.second, times, setup);  

      }
      //   printf("{%02x\n", pair.first);
          
          // for (auto const &val : pair.second){
          //   printf("\t%02x (%d, %d, %.0f),\n", val.sample,
          //          val.is_falling_edge,
          //          val.channel_id,
          //          val.time);
            
          //   h(val.time);
            

        //          }
          
        //          printf("}\n");
      
      //      break;
    }


    // print histogram
    // std::ostringstream os;
    // for (auto x : indexed(h, boost::histogram::coverage::all)) {
    // os << boost::format("bin %2i [%4.1f, %4.1f): %i\n") % x.index() % x.bin().lower() %
    //           x.bin().upper() % *x;
    // }

    // std::cout << os.str() << std::flush;

  
  
}
