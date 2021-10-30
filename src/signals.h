#ifndef _SIGNALS_H_
#define _SIGNALS_H_

#include <bits/stdint-uintn.h>
#include <cstdint>
#include <set>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-label"
#include "unpacker_types.hpp"
#pragma GCC diagnostic pop

#include "setup.h"

using TWData = std::map<uint32_t, std::vector<unpacker::hit_t>>;
using ENDPData = std::vector<unpacker::hit_t>;

struct Time {
  double t;
  bool leading;
};

struct LTpair {
  double t;
  double tot;
  std::uint8_t pm;
  std::uint8_t thr;
};

struct Signal {
  double t;
  double tot_sum;
  double multiplicity;
};

struct Hit {
  double t;
  double z;
  const Scin& scin;
  Hit(const Scin& p_scin): scin(p_scin){}
};

using Times = std::vector<Time>;
using TimesByThr = std::array<Times, 2>;
using TimesByPM = std::array<TimesByThr, 4>;
using TimesByMatrix = std::array<TimesByPM, 13>;

using LTTimes = std::vector<LTpair>;
using LTTimesByMatrix = std::array<LTTimes, 13>;

using Signals = std::vector<Signal>;
using SignalsByMatrix = std::array<Signals, 13>;

class Reconstructor{

public:
  Reconstructor(const Setup& setup): fSetup(setup){
    init();
    initTimes();
  }
  
  void reconstruct(std::unordered_map<unsigned int, ENDPData>& unpacker_data);
  
private:
  void init();
  void groupTimes(uint32_t endp, const ENDPData& endpoint_data, TimesByMatrix& output_times);
  void assembleLT(const TimesByPM& times, LTTimes& output_times);
  void assembleSignals(const LTTimes& times, Signals& output_signals);
  void matchHits();
  void matchHitsOnScin(const Signals& signals_a, const Signals& signals_b, const Scin& scin);

  void resetTimes();
  void initTimes();

  
  const Setup& fSetup;

  
  std::unordered_map<std::uint32_t, TimesByMatrix> fEndpoint_times;
  std::unordered_map<std::uint32_t, LTTimesByMatrix> fEndpoint_lt_times;
  std::unordered_map<std::uint32_t, SignalsByMatrix> fEndpoint_signals;
  
  std::vector<Hit> fHits;

  double fLeadTimeWindow = 10000.0; // ps
  double fHitTimeWindow = 10000.0; // ps

};


#endif
