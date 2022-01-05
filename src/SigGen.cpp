
#include "SigGen.h"

SigGen::SigGen() {
}

SigGen::~SigGen() {
}

bool SigGen::begin() {
  if (_si5351.begin() != ERROR_NONE) {
    return false;
  }

  _si5351.setupPLL(SI5351_PLL_A, 36, 0, _denom);
  _si5351.setupRdiv(OUTPUT_CHANNEL, SI5351_R_DIV_1);

  return true;
}

void SigGen::setFrequency(const uint32_t freq) {
  uint32_t const div2 = 900000000000 / freq;
  uint32_t const div  = div2 / _denom;
  uint32_t const num  = div2 - (div * _denom);

  Serial.printf("freq: %d\n", freq);
  Serial.printf("denom: %d\n", _denom);
  Serial.printf("div2: %d\n", div2);
  Serial.printf("div: %d\n", div);
  Serial.printf("num: %d\n", num);

  _si5351.setupMultisynth(OUTPUT_CHANNEL, SI5351_PLL_A, div, num, _denom);
}

void SigGen::enableOutput(const bool enable) {
  _si5351.enableOutputs(enable);
}
