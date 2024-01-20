//
// data_t.hh
//
//  ADM, July 2005
//  This is a class that defines the data_t object that is used to store the g4numi output
//   in a root tree.
//------------------
#ifndef DATA_T_HH
#define DATA_T_HH

#include "TROOT.h"          // Top level (or root) structure for all classes
#include "TObject.h"

static const std::size_t maxGen = 10;
class data_t 
{
    public:
   // a constructor and a destructor
   data_t();
   virtual ~data_t();
   
   void Clear(const std::string &opt = "");

  
   // the following variables are placed in the root tree
   Int_t run;       
   Int_t evtno;
   Double_t beamHWidth;
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

    //
   //the size of these arrays are arbitray
   //and not descriptive
   //this is bad, get rid of this
   //
   
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
   Double_t tvx;
   Double_t tvy;
   Double_t tvz;
   Double_t tpx;
   Double_t tpy;
   Double_t tpz;
   Int_t    tptype;
   Int_t    tgen;

        
        /// When a neutrino is produced, save the complete tracking history, up to
        /// the primary proton.
        /// number of trajectories between (and including) the neutrino and the primary proton.
        /// By definition, ntrajectory = tgen + 1
    Int_t ntrajectory;
    Bool_t overflow;
        /// Assuming the maximum number of generations is 10, can increase if needed
    Int_t pdg[maxGen];         // particle pdg code
    Int_t trackId[maxGen];     // particle trackId 
    Int_t parentId[maxGen];    // parentId 
    
    Double_t startx[maxGen];   // particle x initial position, mm
    Double_t starty[maxGen];   // particle y initial position
    Double_t startz[maxGen];   // particle z initial position
    Double_t stopx[maxGen];    // particle x final position
    Double_t stopy[maxGen];    // particle y final position
    Double_t stopz[maxGen];    // particle z final position
    
    Double_t startpx[maxGen];  // particle x initial momentum, MeV/c
    Double_t startpy[maxGen];  // particle y initial momentum
    Double_t startpz[maxGen];  // particle z initial momentum
    Double_t stoppx[maxGen];   // particle x final momentum
    Double_t stoppy[maxGen];   // particle y final momentum
    Double_t stoppz[maxGen];   // particle z final momentum
    
    Double_t pprodpx[maxGen];  // parent x momentum when producing this particle, MeV/c
    Double_t pprodpy[maxGen];  // parent y momentum when producing this particle
    Double_t pprodpz[maxGen];  // parent z momentum when producing this particle
    
    TString proc[maxGen]; // name of the process that creates this particle
    
    TString ivol[maxGen]; // name of the volume where the particle starts
    TString fvol[maxGen]; // name of the volume where the particle stops
        
    //
   //the size of these arrays are arbitray
   //and not descriptive
   //this is bad, get rid of this
   //
   
   Double_t trkx[10];
   Double_t trky[10];
   Double_t trkz[10];
   Double_t trkpx[10];
   Double_t trkpy[10];
   Double_t trkpz[10];
   
private:
   ClassDef(data_t ,1) // data_t
      
       };

#endif 

