#include <bits/stdint-uintn.h>
#include <boost/chrono/duration.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

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

struct membuf: std::streambuf {
     membuf(char* begin, char* end) {
         this->setg(begin, begin, end);
     }
};


int main(int // argc
         , char* argv[]) {

  // load setup from JSON file
  auto filename = "../data/test_modular_setup_v10.json";

  Setup setup(filename);

  // open TCP socket & read data
  long int port = 6002;  
  int fd = 0;
  fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in in_addr;
  memset(&in_addr, 0, sizeof(in_addr));

  in_addr.sin_family = AF_INET;
  //in_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  in_addr.sin_port = htons(port);
  in_addr.sin_addr.s_addr = inet_addr("172.16.32.25");

  socklen_t server_len = sizeof(in_addr);
  if(connect(fd, (struct sockaddr*)&in_addr, server_len) < 0){
    perror("Connect");
    exit(EXIT_FAILURE);
  }

  struct timeval timeout;      
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  
  if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                  sizeof timeout) < 0){
    perror("setsockopt failed\n");
  }
  
  // int flags = fcntl(fd, F_GETFL);
  // flags |= O_NONBLOCK;
  // fcntl(fd, F_SETFL, flags);

  char buf[256];
  int nread = read(fd, buf, 256);
  std::cout << "First read: " << nread << " bytes" << std::endl;

  write(fd, "\n", 1);
  
  strcpy(buf, "getevt    \n");
  write(fd, buf, 11);

  char data[256000];
  
  long int total_read = 0;
  //total_read = read(fd, data, 256000);

  do{                          
    nread = read(fd, data + total_read, 256);
    total_read += nread;
  } while(nread > 0);

  std::cout << "Total read: " << total_read << " bytes" << std::endl;

  membuf       sbuf(data, data+total_read);
  std::istream in(&sbuf);
 
  std::ifstream fp( argv[1], std::ios::in | std::ios::binary );

  //    std::vector<unsigned int> data;

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
    
    int succ = 1;
    //    auto h = boost::histogram::make_histogram(boost::histogram::axis::regular<>(100, 0., 1.5e8, "t"));
    
    Reconstructor reco(setup);

    
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
      
      if(ntw == 10000)break;
      
    }
    
    double avg_proc_time = proc_time / ntw;
    std::cout << "Average TW processing time: " << avg_proc_time << " us" << std::endl;
    
    fp.close();

    
    // print histogram
    // std::ostringstream os;
    // for (auto x : indexed(h, boost::histogram::coverage::all)) {
    // os << boost::format("bin %2i [%4.1f, %4.1f): %i\n") % x.index() % x.bin().lower() %
    //           x.bin().upper() % *x;
    // }

    // std::cout << os.str() << std::flush;

  
  
}
