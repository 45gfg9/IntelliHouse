#pragma once

#include <Ticker.h>

class FlagTicker {
  Ticker ticker;
  bool trigger;

public:
  FlagTicker();

  void begin(float s);
  void begin_ms(uint32_t ms);
  void stop();
  void done();

  operator bool();
};
