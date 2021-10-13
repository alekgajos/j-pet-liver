#ifndef _SETUP_H_
#define _SETUP_H_

#include <cstdint>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

enum Side {
  A,
  B  
};

struct Module {

  int id;
  double theta;
  Module(int p_id, double p_theta): id(p_id),theta(p_theta) {}
  
};

struct Scin {

  int id;
  double x;
  double y;
  Module& mod;

  Scin(int p_id, double p_x, double p_y, Module& p_parent): id(p_id), x(p_x), y(p_y), mod(p_parent){}
  
};

struct Matrix {

  Side side;
  Scin& scin;

  Matrix(Side p_side, Scin& p_parent): side(p_side), scin(p_parent) {}
  
};

struct PM {

  int pos_in_matrix;
  Matrix& matrix;

  PM(int p_pos, Matrix& p_parent): pos_in_matrix(p_pos), matrix(p_parent) {}
  
};

struct Channel {

  int thr_num;
  PM& pm;
  
  Channel(int p_thr, PM& p_pm): thr_num(p_thr), pm(p_pm) {}
  
};

class Setup {

public:
  Setup(const char* filename){
    loadFromFile( filename);
    fillEndpointMapping();
  }

  void fillEndpointMapping();

  const Channel& findChannel(std::uint32_t addr, std::uint8_t ch) const {
    return fEndpointToChannel.at(addr).at(ch);
  }

  const std::vector<uint32_t>& getEndpoints() const{
    return fEndpoints;
  }
  
private:
  void loadFromFile(const char* filename);

  std::unordered_map<int, Module> fModules;
  std::unordered_map<int, Scin> fScins;
  std::unordered_map<int, Matrix> fMatrices;
  std::unordered_map<int, PM> fPMs;
  std::unordered_map<int, Channel> fChannels;

  std::unordered_map<std::uint32_t, std::unordered_map<std::uint8_t, Channel&>> fEndpointToChannel;
  
  std::vector<uint32_t> fEndpoints = {
    0xA110, 0xA120, 0xA130, 0xA140, 0xA150, 0xA160,
    0xA170, 0xA180, 0xA190, 0xA1A0, 0xA1B0, 0xA1C0,
    0xA210, 0xA220, 0xA230, 0xA240, 0xA250, 0xA260,
    0xA270, 0xA280, 0xA290, 0xA2A0, 0xA2B0, 0xA2C0,
    0xA310, 0xA320, 0xA330, 0xA340, 0xA350, 0xA360,
    0xA370, 0xA380, 0xA390, 0xA3A0, 0xA3B0, 0xA3C0,
    0xA410, 0xA420, 0xA430, 0xA440, 0xA450, 0xA460,
    0xA470, 0xA480, 0xA490, 0xA4A0, 0xA4B0, 0xA4C0,
  };

  std::unordered_map<uint32_t, uint32_t> fOppositeEndpoints;
  
};

#endif
