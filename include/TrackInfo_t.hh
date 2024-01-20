
#ifndef TRACKINFO_T_HH
#define TRACKINFO_T_HH

#include "TROOT.h"        
#include "TObject.h"
#include "Rtypes.h"
#include "TVector3.h"
#include "TLorentzVector.h"

class TrackInfo_t 
{
 public:
  TrackInfo_t();
  virtual ~TrackInfo_t();

  TVector3 Pos;
  TLorentzVector Mom;

  Int_t PDGcode;
  Bool_t FromFast;
 private:
  ClassDef(TrackInfo_t ,1)
};
#endif 

