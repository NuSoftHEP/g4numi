#ifndef G4NUMINTP_H
#define G4NUMINTP_H 


#include <TH1.h>
#include <TTree.h>
#include <TFile.h>
#include <iostream>
#include <math.h>
#include <TLeaf.h>

class G4numiNtp {

public:
  G4numiNtp(void);
  ~G4numiNtp(void);

      Int_t run;
      Int_t evtno;
      Double_t Ndxdz;
      Double_t Ndydz;
      Double_t Npz;
      Double_t Nenergy;

      Int_t Ndecay;
      Int_t Ntype;
      Double_t Vx;
      Double_t Vy;
      Double_t Vz;
      Double_t pdpx;
      Double_t pdpy;
      Double_t pdpz;
      Double_t ppdxdz;
      Double_t ppdydz;
      Double_t pppz;
      Double_t ppenergy;
      Int_t ptype;
      Double_t ppvx;
      Double_t ppvy;
      Double_t ppvz;
      Double_t muparpx;
      Double_t muparpy;
      Double_t muparpz;
      Double_t mupare;
      Double_t Necm;
      Double_t Nimpwt;
      Double_t tvx;
      Double_t tvy;
      Double_t tvz;
      Double_t tpx;
      Double_t tpy;
      Double_t tpz;
      Int_t tptype;
      Int_t tgen;

      Double_t NdxdzNear;
      Double_t NdydzNear;
      Double_t NenergyN;
      Double_t NWtNear;

  Int_t pdg[10];
  Double_t startpx[10];
  Double_t startpy[10];
  Double_t startpz[10];
  Double_t stoppx[10];
  Double_t stoppy[10];
  Double_t stoppz[10];
  Double_t startx[10];
  Double_t starty[10];
  Double_t startz[10];
  Double_t stopx[10];
  Double_t stopy[10];
  Double_t stopz[10];
  
  //void SetTree(TTree &t, Int_t entry);
  void SetTree(TTree &t);
  
private:
  Int_t entry;
  TLeaf *l_NdxdzNear;
  TLeaf *l_NdydzNear;
  TLeaf *l_NenergyN;
  TLeaf *l_NWtNear;

      TLeaf *l_run;
      TLeaf *l_evtno;
      TLeaf *l_Ndxdz;
      TLeaf *l_Ndydz;
      TLeaf *l_Npz;
      TLeaf *l_Nenergy;
      TLeaf *l_Ndecay;
      TLeaf *l_Ntype;
      TLeaf *l_Vx;
      TLeaf *l_Vy;
      TLeaf *l_Vz;
      TLeaf *l_pdpx;
      TLeaf *l_pdpy;
      TLeaf *l_pdpz;
      TLeaf *l_ppdxdz;
      TLeaf *l_ppdydz;
      TLeaf *l_pppz;
      TLeaf *l_ppenergy;
      TLeaf *l_ptype;
      TLeaf *l_ppvx;
      TLeaf *l_ppvy;
      TLeaf *l_ppvz;
      TLeaf *l_muparpx;
      TLeaf *l_muparpy;
      TLeaf *l_muparpz;
      TLeaf *l_mupare;
      TLeaf *l_Necm;
      TLeaf *l_Nimpwt;
      TLeaf *l_tvx;
      TLeaf *l_tvy;
      TLeaf *l_tvz;
      TLeaf *l_tpx;
      TLeaf *l_tpy;
      TLeaf *l_tpz;
      TLeaf *l_tptype;
      TLeaf *l_tgen;

  // Full ancestry stuff


  TLeaf * l_pdg;
  TLeaf * l_startpx;
  TLeaf * l_startpy;
  TLeaf * l_startpz;
  TLeaf * l_stoppx;
  TLeaf * l_stoppy;
  TLeaf * l_stoppz;
  TLeaf * l_startx;
  TLeaf * l_starty;
  TLeaf * l_startz;
  TLeaf * l_stopx;
  TLeaf * l_stopy;
  TLeaf * l_stopz;

};

#endif
