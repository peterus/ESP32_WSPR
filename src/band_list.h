#ifndef BAND_LIST_H_
#define BAND_LIST_H_

#include <ESP-Config.h>
#include <list>

class BandConfig {
public:
  BandConfig(String const &name, ConfigBool *enable, ConfigInt *frequency);

  String getName() const;
  bool   isEnabled() const;
  long   getFrequency() const;

private:
  String      _name;
  ConfigBool *_enable;
  ConfigInt * _frequency;
};

class BandList {
public:
  BandList();
  void add(BandConfig frequency);

  BandConfig getNext();

private:
  std::list<BandConfig>           _frequencyList;
  std::list<BandConfig>::iterator _nextEntry;
};

#endif
