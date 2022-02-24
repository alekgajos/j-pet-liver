#include <bits/stdint-uintn.h>
#include <boost/chrono/duration.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

#include <boost/format.hpp>
#include <boost/histogram.hpp>
#include <boost/chrono.hpp>

#define SUPPRESS_TRIGGER_MISMATCH_ERROR
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-label"
#include "unpacker.hpp"
#pragma GCC diagnostic pop

#include "setup.h"
#include "signals.h"
#include "online_reader.h"

int main(// int // argc
         // , char* argv[]
         ) {

  // parse command line args
  
  
  // load setup from JSON file
  auto filename = "../data/test_modular_setup_v10.json";

  Setup setup(filename);



  //  std::ifstream fp( argv[1], std::ios::in | std::ios::binary );

    unpacker::meta_t meta_data;
    std::unordered_map<unsigned int, ENDPData> original_data;
    std::unordered_map<unsigned int, std::vector<unpacker::hit_t>> filtered_data;
    std::unordered_map<unsigned int, std::vector<unpacker::sigmat_t>> preproc_data;
    std::unordered_map<unsigned int, std::string> paths_to_tdc_calib;
    
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
    
    OnlineReader reader("172.16.32.25", 6002);
    reader.connect();

    std::istream in = reader.getData();


    char buf[256];
    int n = 0;
    while(!in.eof()){
      in.read(buf, 1);
      n++;
    }
    std::cout << "Read " << n << std::endl;
    
    int succ = 1;
    //    auto h = boost::histogram::make_histogram(boost::histogram::axis::regular<>(100, 0., 1.5e8, "t"));
    
    Reconstructor reco(setup);
    unpacker::set_online_mode(true);
    
    boost::chrono::high_resolution_clock::time_point t0,t1;
    int ntw = 0;
    double proc_time = 0;
    
    while( succ ) {

      t0 = boost::chrono::high_resolution_clock::now();

      succ = unpacker::get_time_window(meta_data,
                                       original_data,
                                       filtered_data,
                                       preproc_data,
                                       // fp
                                       in
                                       );

      reco.reconstruct(original_data);

      
      

      t1 = boost::chrono::high_resolution_clock::now();

      proc_time += boost::chrono::duration_cast<boost::chrono::microseconds>(t1-t0).count();
      ntw++;
      
      //      if(ntw == 10000)break;
      
    }
    
    double avg_proc_time = proc_time / ntw;
    std::cout << "Average TW processing time: " << avg_proc_time << " us" << std::endl;
    
    //    fp.close();

    
    // print histogram
    // std::ostringstream os;
    // for (auto x : indexed(h, boost::histogram::coverage::all)) {
    // os << boost::format("bin %2i [%4.1f, %4.1f): %i\n") % x.index() % x.bin().lower() %
    //           x.bin().upper() % *x;
    // }

    // std::cout << os.str() << std::flush;

  
  
}
