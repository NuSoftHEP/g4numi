//----------------------------------------------------------------------
// draytupleSPB_t.hh
//
// ADM, July 2005
// This is a class that defines the draytupleSPB_t object that is used to store 
// the g4numi output in a root tree.
//
// $Id: draytupleSPB_t.hh,v 1.1.2.2 2014/01/22 22:31:06 kordosky Exp $
//----------------------------------------------------------------------

#ifndef DRAYTUPLESPB_T_HH
#define DRAYTUPLESPB_T_HH

#include "TROOT.h"          // Top level (or root) structure for all classes
#include "TObject.h"

class  draytupleSPB_t 
{
  
 public:
  // a constructor and a destructor
  draytupleSPB_t();
  virtual ~draytupleSPB_t();

  void Clear();

   // the following variables are placed in the root tree
    // number of parent that created the muon
   
   // x y and z position of muon at EODP
   Float_t muvx;
   Float_t muvy;
   Float_t muvz;

   // x y and z momentum of muon at EODP
   Float_t mupx;
   Float_t mupy;
   Float_t mupz;

   //particle type = muon
   Short_t ptype;

   //
   //For the following index 0, 1 and 2 correspond
   //to Alcove 1, Alcove 2 and Alcove 3 respectively
   //
   
   //x y and z position of muon at the muon monitor
   Float_t mmxpos[3];
   Float_t mmypos[3];
   //Float_t mmzpos[3];

   //x y and z momentum of muon at the muon monitor
   Float_t mmpx[3];
   Float_t mmpy[3];
   Float_t mmpz[3];

   // the monitor pixel that the muon intercepts
   Short_t cell[3];

   //total energy deposited by muon in the cell(s)
   Float_t mu_edep[3];

   //total energy deposited by electrons/positrons
   //that are generated from interactions of the
   //muon with the monitor
   Float_t int_edep[3];

   //total energy deposited by electrons/positrons
   //that are generated from interactions of the
   //muon with the elements external to the monitor
   Float_t ext_edep[3];
   
private:
   ClassDef(draytupleSPB_t , 1)
      
      };
#endif 

