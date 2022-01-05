#ifndef SIGGEN_H_
#define SIGGEN_H_

#include <Adafruit_SI5351.h>
#include <Arduino.h>

constexpr uint32_t FREQ_80M    = 3592600;
constexpr uint32_t FREQ_60M    = 5364700;
constexpr uint32_t FREQ_40M    = 7038600;
constexpr uint32_t FREQ_30M    = 10138700;
constexpr uint32_t FREQ_20M    = 14095600;
constexpr uint32_t FREQ_17M    = 18104600;
constexpr uint32_t FREQ_15M    = 21094600;
constexpr uint32_t FREQ_12M    = 24924600;
constexpr uint32_t FREQ_10M    = 28124600;
constexpr uint32_t FREQ_6M     = 50293000;
constexpr uint32_t FREQ_OFFSET = 1500;

class SigGen {
public:
  SigGen();
  ~SigGen();

  bool begin();

  void setFrequency(const uint32_t freq);
  void enableOutput(const bool enable);

private:
  Adafruit_SI5351 _si5351;
};

#endif
