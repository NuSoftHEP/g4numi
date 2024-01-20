//
// h1trackingplane_exit_t.hh
//
//  ADM, July 2005
//  This is a class that defines the h1trackingplane_exit_t object that is used to store the g4numi output
//   in a root tree.
//------------------
#ifndef TARGET_EXIT_T_HH
#define TARGET_EXIT_T_HH

#include "TROOT.h"          // Top level (or root) structure for all classes
#include "TObject.h"

class h1trackingplane_exit_t 
{
  
 public:
  // a constructor and a destructor
  h1trackingplane_exit_t();
  virtual ~h1trackingplane_exit_t();

  
  // the following variables are placed in the root tree
  
  Int_t run;       
  Int_t evtno;
  /*Double_t beamHWidth;
  Double_t beamVWidth;
  Double_t beamX;
  Double_t beamY;
  Double_t protonX;
  Double_t protonY;
  Double_t protonZ;
  Double_t protonPx;
  Double_t protonPy;
  Double_t protonPz;
  Double_t nuTarZ;
  Double_t hornCurrent;
  Double_t Ndxdz;
  Double_t Ndydz;
  Double_t Npz;
  Double_t Nenergy;
  Double_t NdxdzNear[11];//was 9
  Double_t NdydzNear[11];   
  Double_t NenergyN[11];    
  Double_t NWtNear[11];     
  Double_t NdxdzFar[2];    
  Double_t NdydzFar[2];   
  Double_t NenergyF[2];    
  Double_t NWtFar[2];      
  Int_t    Norig;
  Int_t    Ndecay;
  Int_t    Ntype;
  Double_t Vx;
  Double_t Vy;
  Double_t Vz;
  Double_t pdPx;
  Double_t pdPy;
  Double_t pdPz;
  Double_t ppdxdz;
  Double_t ppdydz;
  Double_t pppz;
  Double_t ppenergy;
  Double_t ppmedium;
  Int_t    ptype;
  Double_t ppvx;
  Double_t ppvy;
  Double_t ppvz;
  Double_t muparpx;
  Double_t muparpy;
  Double_t muparpz;
  Double_t mupare;
  Double_t Necm;
  Double_t Nimpwt;
  Double_t xpoint;
  Double_t ypoint;
  Double_t zpoint;
  */
  Int_t job;
  Double_t Nimpwt;
  Double_t impwt;
  Double_t tvx;
  Double_t tvy;
  Double_t tvz;
  Double_t tpx;
  Double_t tpy;
  Double_t tpz;
  Double_t tof;
  Double_t ppvx;
  Double_t ppvy;
  Double_t ppvz;
  Int_t    tptype;
  /*Int_t    tgen;
  Double_t trkx[10];
  Double_t trky[10];
  Double_t trkz[10];
  Double_t trkpx[10];
  Double_t trkpy[10];
  Double_t trkpz[10];
  */ 
 private:
  ClassDef(h1trackingplane_exit_t ,1) // h1trackingplane_exit_t
    
    };

#endif 

