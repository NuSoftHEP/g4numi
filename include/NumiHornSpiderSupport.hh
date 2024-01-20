#ifndef NumiHornSpiderSupport_H
#define NumiHornSpiderSupport_H 1

#include "globals.hh"

class NumiHornSpiderSupport 
{
public:
  NumiHornSpiderSupport();
  NumiHornSpiderSupport(const NumiHornSpiderSupport *);
  ~NumiHornSpiderSupport();

  G4double stripW;
  G4double stripH;
  G4double stripL;
  G4double topW;
  G4double topH;
  G4double topL;
  G4double bottomW;
  G4double bottomH;
  G4double bottomL;
  G4double bottomThickMid;  //Thickness of bottom part in the middle
  G4double bottomR;
  G4double ceramicRodR;
};

#endif
