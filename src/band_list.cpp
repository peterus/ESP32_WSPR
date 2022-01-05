#include "band_list.h"

String BandConfig::getName() const {
  return _name;
}

BandConfig::BandConfig(String const &name, ConfigBool *enable, ConfigInt *frequency) {
  _name      = name;
  _enable    = enable;
  _frequency = frequency;
}

bool BandConfig::isEnabled() const {
  return _enable->getValue();
}

long BandConfig::getFrequency() const {
  return _frequency->getValue();
}

BandList::BandList() : _nextEntry(_frequencyList.begin()) {
}

void BandList::add(BandConfig frequency) {
  _frequencyList.push_back(frequency);
  _nextEntry = _frequencyList.begin();
}

BandConfig BandList::getNext() {
  BandConfig value = *_nextEntry;
  _nextEntry++;
  if (_nextEntry == _frequencyList.end()) {
    _nextEntry = _frequencyList.begin();
  }
  return value;
}
