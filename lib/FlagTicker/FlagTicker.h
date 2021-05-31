#pragma once

#include <Ticker.h>

class FlagTicker {
  Ticker ticker;
  volatile bool trigger = false;

public:
  void begin(float s);
  void begin_ms(uint32_t ms);
  void stop();

  operator bool() const;
  FlagTicker &operator=(bool state);
};
