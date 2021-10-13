#include "signals.h"

#include <array>
#include <bits/stdint-uintn.h>
#include <iostream>
#include <ostream>

void Reconstructor::reconstruct(std::unordered_map<unsigned int, ENDPData>& unpacker_data){

  resetTimes();

  for (auto const &pair: unpacker_data){
    
    groupTimes(pair.first, pair.second, fEndpoint_times[pair.first]);  

    for(int matrix = 0; matrix < 13; ++matrix){
      assembleLT(fEndpoint_times[pair.first][matrix],
                 fEndpoint_lt_times[pair.first][matrix]);
      
      assembleSignals(fEndpoint_lt_times[pair.first][matrix],
                      fEndpoint_signals[pair.first][matrix]);
    }
  }
  
  
}

void Reconstructor::assembleSignals(const LTTimes &times,
                                    Signals &output_signals) {

  uint i=0;

  while(i < times.size()){

    double start_time = times.at(i).t;
    Signal signal;
    signal.tot_sum = times.at(i).tot;
    signal.t = times.at(i).t;
    signal.multiplicity = 1;
    
    uint k=0;
    while( i+k < times.size()){
      if(fabs(times.at(i+k).t - start_time) < fLeadTimeWindow){
        signal.tot_sum += times.at(i+k).tot;
        signal.t += times.at(i+k).t;
        signal.multiplicity++;
        k++;
      }else{
        break;
      }      
    }
    i += k;
    signal.t /= signal.multiplicity;    
    output_signals.push_back(signal);

    // convert times to ns
    signal.t /= 1000.;
    signal.tot_sum /= 1000.;

    std::cout << signal.multiplicity << " : " <<
    signal.tot_sum / signal.multiplicity  << std::endl;
    }
  
  
}

void Reconstructor::groupTimes(uint32_t endp, const ENDPData &endp_data, TimesByMatrix& output_times) {

  for(const unpacker::hit_t& hit: endp_data){
    
    if(hit.channel_id == 104){
      continue;
    }

    Time t;

    if(hit.is_falling_edge){
      t.leading = false;
    }else{
       t.leading = true;
    }

    t.t = hit.time;

    const Channel& channel = fSetup.findChannel(endp, hit.channel_id);
    int pos_in_matrix = channel.pm.pos_in_matrix - 1; // we index from 0 for the array storage
    int scin = (channel.pm.matrix.scin.id - 201) % 13;
    int thr = channel.thr_num - 1;

    output_times[scin][pos_in_matrix][thr].push_back(t);
    
  }
  
}

void Reconstructor::assembleLT(const TimesByPM& times, LTTimes& output_times) {

  for(int pm = 0; pm < 4; ++pm){
      for(int thr = 0; thr < 2; ++thr){
        // match lead-trail pairs
        bool searching_trail = false;
        const Time* last_lead;

        for(uint i = 0; i < times[pm][thr].size(); ++i){
          if( times[pm][thr].at(i).leading == true && !searching_trail ){
            searching_trail = true;
            last_lead = &(times[pm][thr].at(i));
            continue;
          }
          if( searching_trail && times[pm][thr].at(i).leading == false){
            // build an LT pair
            LTpair p;
            p.pm = pm;
            p.thr = thr;
            p.t = last_lead->t;
            p.tot = fabs(times[pm][thr].at(i).t - last_lead->t);
            output_times.push_back(p);
            searching_trail = false;
            last_lead = nullptr;
            
          }
        }
      }
  }

  // sort the LT pairs from the single matrix
  std::sort(output_times.begin(), output_times.end(), [](const LTpair& a, const LTpair& b){
    return a.t < b.t;
  });

}

void Reconstructor::resetTimes() {

  for(const uint32_t& endp: fSetup.getEndpoints()){
    for(int matrix = 0; matrix < 13; ++matrix){
      for(int pm = 0; pm < 4; ++pm){
        for(int thr = 0; thr < 2; ++thr){
          fEndpoint_times[endp][matrix][pm][thr].clear();
        } 
      }
      fEndpoint_lt_times[endp][matrix].clear();
      fEndpoint_signals[endp][matrix].clear();
    }
  }
}

void Reconstructor::initTimes() {

  for(const uint32_t& endp: fSetup.getEndpoints()){
    for(int matrix = 0; matrix < 13; ++matrix){
      for(int pm = 0; pm < 4; ++pm){
        for(int thr = 0; thr < 2; ++thr){
          fEndpoint_times[endp][matrix][pm][thr].reserve(100);
        } 
      }
      
      fEndpoint_lt_times[endp][matrix].reserve(100);
    }
  }
}

void Reconstructor::init() {

  for(const uint32_t& endp: fSetup.getEndpoints()){
    fEndpoint_times[endp] = TimesByMatrix();
  }
  for(const uint32_t& endp: fSetup.getEndpoints()){
    fEndpoint_lt_times[endp] = LTTimesByMatrix();
  }
  
}
