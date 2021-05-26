#include "FlagTicker.h"

void FlagTicker::begin(float s) {
  ticker.attach(s, [&]() { *this = true; });
}

void FlagTicker::begin_ms(uint32_t ms) {
  ticker.attach_ms(ms, [&]() { *this = true; });
}

void FlagTicker::stop() {
  ticker.detach();
}

FlagTicker::operator bool() const {
  return trigger;
}

FlagTicker &FlagTicker::operator=(bool state) {
  trigger = state;
  return *this;
}
