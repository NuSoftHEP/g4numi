 //
// zptuple_t.hh
//
// Jasmine, July 2007
//  This is a class that defines the zptuple_t object that is used to store the g4numi output
//   in a root tree.
//------------------
#ifndef ZPTUPLE_T_HH
#define ZPTUPLE_T_HH

#include "TROOT.h"          // Top level (or root) structure for all classes
#include "TObject.h"

class zptuple_t 
{
  
 public:
  // a constructor and a destructor
  zptuple_t();
  virtual ~zptuple_t();
  
  
  // the following variables are placed in the root tree
  Int_t run;       
  Int_t evtno; 
  Double_t xposatz;
  Double_t yposatz;
  Double_t zposatz;
  Double_t xmomatz;
  Double_t ymomatz;
  Double_t zmomatz;
  Double_t matilen;
  Double_t field;
  Double_t pathlength;
  Int_t ptypeatz;
  Int_t pidtype;
  Double_t zpoint;
 private:
  ClassDef(zptuple_t , 1)
    
    };
#endif // admmtuple_t_h

