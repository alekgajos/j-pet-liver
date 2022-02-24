#include "online_reader.h"

#include <cstddef>
#include <iostream>

int OnlineReader::connect() {

  m_socket = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in in_addr;
  memset(&in_addr, 0, sizeof(in_addr));

  in_addr.sin_family = AF_INET;
  in_addr.sin_port = htons(m_daq_port);
  in_addr.sin_addr.s_addr = inet_addr(m_daq_server.c_str());

  socklen_t server_len = sizeof(in_addr);
  if(::connect(m_socket, (struct sockaddr*)&in_addr, server_len) < 0){
    perror("Connect");
    exit(EXIT_FAILURE);
  }

  struct timeval timeout;      
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  
  if (setsockopt (m_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                  sizeof timeout) < 0){
    perror("setsockopt failed\n");
  }
  
  m_header_size = read(m_socket, m_buffer, 256);
  std::cout << "First read: " << m_header_size << " bytes" << std::endl;

  write(m_socket, "\n", 1);
  
  return 0;
}


std::istream OnlineReader::getData(){

  write(m_socket, "getevt    \n", 11);

  m_bytes_read = m_header_size;
  size_t nread = 0;
  do{                          
    nread = read(m_socket, m_buffer + m_header_size, 256);
    m_bytes_read += nread;
    std::cout << "reading data" << std::endl;
  } while(nread > 0);

  std::cout << "Total read: " << m_bytes_read << " bytes" << std::endl;  

  m_sbuffer.mapOnBuffer(m_buffer, m_buffer+m_bytes_read);
  return std::istream(&m_sbuffer);
  
}
