//
// NumiImpWeight.hh
//

#ifndef NumiImpWeight_H
#define NumiImpWeight_H 1

#include "globals.hh"
#include "G4ThreeVector.hh"

class G4Track;

class NumiImpWeight
{
  public:
    NumiImpWeight();
    virtual ~NumiImpWeight();

  static G4double CalculateImpWeight(const G4Track * aTrack);

  static G4double CalculateSumImpWeight2(const G4Track * aTrack);
  

};

#endif
