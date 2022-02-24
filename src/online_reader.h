#ifndef _ONLINE_READER_H_
#define _ONLINE_READER_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstddef>
#include <istream>
#include <streambuf>
#include <string>
#include <bits/stdint-uintn.h>
#include <cstdint>
#include <set>

struct Membuf: std::streambuf {
  void mapOnBuffer(char* begin, char* end) {
    this->setg(begin, begin, end);
  }
};

class OnlineReader {

public:
  OnlineReader(std::string daq_server, long int daq_port = 6002): m_daq_server(daq_server), m_daq_port(daq_port){}
  ~OnlineReader(){
    close(m_socket);
  }
  int connect();

  std::istream getData();

private:
  std::string m_daq_server;
  long int m_daq_port;  
  int m_socket;
  char m_buffer[256000];
  Membuf m_sbuffer;
  long int m_bytes_read = 0;
  long int m_header_size = 0;
};

#endif
