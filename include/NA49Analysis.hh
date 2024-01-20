
#ifndef NA49ANALYSIS_HH
#define NA49ANALYSIS_HH

#include "globals.hh"

//root
#include "TSystem.h"

//G4
#include "G4ios.hh"
#include "G4TrajectoryContainer.hh"
#include "G4ParticleDefinition.hh"
#include "G4Element.hh"

#include <map>
#include <vector>

#include "ProdTuple_t.hh"
#include "TrackInfo_t.hh"
#include "NA49Config.hh"

class G4ParticleDefinition;
class G4Step;
class TFile;
class TTree;
class G4Track;
class G4VTrajectory;

//class ProdTuple_t;

class NA49Analysis
{
public:

   NA49Analysis();
   ~NA49Analysis();

   void book(const NA49Config &config);
   void FillNtuple(std::vector<TrackInfo_t>);

   void WriteNtuple();
   void GetPrimGenInfo(Double_t enerPrim,G4ParticleDefinition* Part);

   static NA49Analysis* getInstance();
   void finish();
   
   
private:
   static NA49Analysis* instance;
   
  char NtupleFileName[50];
   TFile* FileNtuple;  
   TTree* ProdTree;
   TTree* NA49Setup;
   ProdTuple_t g4Proddata; 
   Double_t enerPrimGen;
   G4ParticleDefinition* particle;

};

#endif 
