#ifndef NA49TrackInfo_h
#define NA49TrackInfo_h

#include "globals.hh"
//#include "G4ParticleDefinition.hh"
//#include "NA49Analysis.hh"

#include "G4VUserTrackInformation.hh"

class NA49TrackInfo : public G4VUserTrackInformation {
public:
  NA49TrackInfo();
  virtual ~NA49TrackInfo();
  virtual void Print() const;
  bool primary_chain; // did the primary particle make this one?
  bool fast_decay; // does this particle decay quickly?
  bool fast_decay_progeny; // did this particle have a quickly decaying anscestor?

};


#endif
