
#ifndef PRODTUPLE_T_HH
#define PRODTUPLE_T_HH

#include "TROOT.h"        
#include "TObject.h"
#include "Rtypes.h"

const Int_t maxPart = 150;

class ProdTuple_t 
{
  
 public:
  // a constructor and a destructor
  ProdTuple_t();
  virtual ~ProdTuple_t();
  
  // the following variables are placed in the root tree
  Int_t NPart;
  Int_t PDG[maxPart];
  Double_t X[maxPart][3];//Initial position of the track
  Double_t P[maxPart][4];//4-momentum. P[maxPart][3] is the energy
  Double_t XF[maxPart];
  Double_t PT[maxPart];
  Bool_t FF[maxPart];// true if particle was produced by a quickly decaying particle which G4 explicitly produces (tau<1e-16 sec: pi0, eta, eta', sigma0)
 private:
   ClassDef(ProdTuple_t , 1)
    
    };
#endif 

