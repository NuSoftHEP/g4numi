//
// absbkgtuple_t.hh
//
//  ADM, July 2005
//  This is a class that defines the absbkgtuple_t object that is used to store the g4numi output
//   in a root tree.
//------------------
#ifndef ABSBKGTUPLE_T_HH
#define ABSBKGTUPLE_T_HH

#include "TROOT.h"          // Top level (or root) structure for all classes
#include "TObject.h"

class absbkgtuple_t 
{
  
 public:
  // a constructor and a destructor
  absbkgtuple_t();
  virtual ~absbkgtuple_t();

   void Clear();
  
  // the following variables are placed in the root tree
   Float_t ihorn;
   Int_t tgtz;
   Int_t ptype;
   Float_t x;
   Float_t y;
   Float_t z;
   Float_t px;
   Float_t py;
   Float_t pz;
   Float_t KE;
   Float_t impwt;
   Int_t tgen;
   
    
 private:
  ClassDef(absbkgtuple_t ,1) // absbkgtuple_t
    
    };

#endif 

