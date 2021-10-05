#include "setup.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <tuple>
#include <unordered_map>
#include <utility>

void Setup::loadFromFile(const char *filename) {

  std::ifstream setup_file(filename, std::ios::in);

  json setup_json;
  setup_file >> setup_json;

  auto& setup = setup_json["35"]; 
  auto& modules = setup["slot"];
  auto& scins = setup["scin"];
  auto& matrices = setup["matrix"];
  auto& pms = setup["pm"];
  auto& channels = setup["channel"];
  
  // populate modules
  for(auto& item: modules.items()){
    int id = std::stoi(item.value()["id"].get<std::string>());
    double theta = std::stod(item.value()["theta"].get<std::string>());
       
    fModules.emplace(std::piecewise_construct,
                     std::forward_as_tuple(id),
                     std::forward_as_tuple(id, theta)
                     );
  }

  // populate scintillators
  for(auto& item: scins.items()){
    int id = item.value()["id"].get<int>();
    int slot_id = item.value()["slot_id"].get<int>();
    double x = std::stod(item.value()["xcenter"].get<std::string>());
    double y = std::stod(item.value()["ycenter"].get<std::string>());

    fScins.emplace(std::piecewise_construct,
                   std::forward_as_tuple(id),
                   std::forward_as_tuple(x, y, fModules.at(slot_id))
                   );
  }

  // populate matrices
  for(auto& item: matrices.items()){
    int id = item.value()["id"].get<int>();
    int scin_id = item.value()["scin_id"].get<int>();
    Side side = item.value()["side"] == "A" ? A : B;

    fMatrices.emplace(std::piecewise_construct,
                      std::forward_as_tuple(id),
                      std::forward_as_tuple(side, fScins.at(scin_id))
                      );
  }

  // populate photomultipliers
  for(auto& item: pms.items()){
    int id = item.value()["id"].get<int>();
    int matrix_id = item.value()["matrix_id"].get<int>();
    int pos = item.value()["pos_in_matrix"].get<int>();

    fPMs.emplace(std::piecewise_construct,
                      std::forward_as_tuple(id),
                      std::forward_as_tuple(pos, fMatrices.at(matrix_id))
                      );
  }

  // populate channels
  for(auto& item: channels.items()){
    int id = std::stoi(item.value()["id"].get<std::string>());
    int pm_id = std::stoi(item.value()["pm_id"].get<std::string>());
    int thr = std::stoi(item.value()["thr_num"].get<std::string>());

    fChannels.emplace(std::piecewise_construct,
                      std::forward_as_tuple(id),
                      std::forward_as_tuple(thr, fPMs.at(pm_id))
                      );
  }
  
  setup_file.close();
    
}

void Setup::fillEndpointMapping() {

  int base_channel_offset = 2100;

  for(int endp = 0 ; endp < 48; ++endp){

    std::unordered_map<std::uint8_t, Channel&> map;
    int endpoint_offset = endp * 105;
    for(int ch = 0; ch < 104; ++ch){
      map.emplace(ch, fChannels.at(base_channel_offset + endpoint_offset + ch));
    }    

    fEndpointToChannel[fEndpoints.at(endp)] = map;
    
  }
  
}
