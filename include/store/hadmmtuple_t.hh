//----------------------------------------------------------------------
// hadmmtuple_t.hh
//
// ADM, July 2005
// This is a class that defines the hadmmtuple_t object that is used to store 
// the g4numi output in a root tree.
//
// $Id: hadmmtuple_t.hh,v 1.8.4.2 2014/01/22 22:31:06 kordosky Exp $
//----------------------------------------------------------------------

#ifndef HADMMTUPLE_T_HH
#define HADMMTUPLE_T_HH

#include "TROOT.h"          // Top level (or root) structure for all classes
#include "TObject.h"

class  hadmmtuple_t 
{
  
 public:
  // a constructor and a destructor
  hadmmtuple_t();
  virtual ~hadmmtuple_t();

  void Clear();

   // the following variables are placed in the root tree
    // number of parent that created the muon
   UShort_t evtno;
   
   // x y and z position of muon at EODP
   Float_t muvx;
   Float_t muvy;
   //Float_t muvz;

   // x y and z momentum of muon at EODP
   Float_t mupx;
   Float_t mupy;
   Float_t mupz;

   //muon weight to reach EODP
   Double_t muweight;

   // x y and z position of particle leaving the target
   Float_t tvx;
   Float_t tvy;
   Float_t tvz;

   // x y and z momentum of particle leaving the target
   Float_t tpx;
   Float_t tpy;
   Float_t tpz;

   //type of particle leaving the target
   Short_t tpptype;

   //importance weight of parent particle from gnumi
   Double_t nimpwt;

   // x y and z postion of parent particle at production
   Float_t ppvx;
   Float_t ppvy;
   Float_t ppvz;

   // x y and z postion of parent particle at decay
   Float_t pdvx;
   Float_t pdvy;
   Float_t pdvz;

   // x y and z momentum of parent particle at decay
   Float_t pdpx;
   Float_t pdpy;
   Float_t pdpz;

   //parent type of muon
   Short_t pptype;

   //particle type = muon
   Short_t ptype;

   //material number of parent particle production
   Short_t ppmedium;

   //generation of parent particle
   Short_t pgen;

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

   //x y and z position of muon at 0.5 meters into "rock"
   //in front of monitor
   //Float_t mmxpos_Edep[3];
   //Float_t mmypos_Edep[3];
   //Float_t mmzpos_Edep[3];

   //x y and z momentum of muon at 0.5 meters into "rock"
   //in front of monitor
   Float_t mmpx_Edep[3];
   Float_t mmpy_Edep[3];
   Float_t mmpz_Edep[3];

//////////////////////////////////////////////////////////////////
   /*
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
   */
   /////////////////////////////////////////////////////////////////////////////
   //temporary variable which indicates
   //the zpostion of the cascade which
   //produced the delta ray that intercepted the chamber
   //Float_t zpos_edep[3];
        


   
   /*Float_t hmmenergy;
     Float_t hmmxpos;
     Float_t hmmypos;
     Float_t hmmzpos;
     Float_t hmmpx;
     Float_t hmmpy;
     Float_t hmmpz; 
     Short_t run;
     Float_t mtgthsig;
     Float_t mtgtvsig;
     Float_t mtgthpos;
     Float_t mtgtvpos;
   */  
   
private:
   ClassDef(hadmmtuple_t , 1)
      
      };
#endif // admmtuple_t_h

