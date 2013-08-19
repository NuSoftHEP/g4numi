//
// NumiAnalysis.hh
//
// Modified Jul 2005 by A. MArino to make data_t and hadmmtuple_t classes

#ifndef NUMIANALYSIS_HH
#define NUMIANALYSIS_HH

#include "globals.hh"
#include "NumiTrajectory.hh"
#include "NumiDataInput.hh"

//root
#include "TSystem.h"

//G4
#include "G4ios.hh"
#include "G4TrajectoryContainer.hh"

#include <map>
#include <vector>

class G4ParticleDefinition;
class G4Step;
class TFile;
class TTree;
class G4Track;
class G4VTrajectory;
class data_t;
class hadmmtuple_t;
class draytupleMIB_t;
class draytupleSPB_t;
class absbkgtuple_t;
class zptuple_t;
class target_exit_t;


class NumiAnalysis
{
public:

   NumiAnalysis();
   ~NumiAnalysis();
   
   void book();
   void finish();
   void FillNeutrinoNtuple(const G4Track& track,
                           const std::vector<G4VTrajectory*>& nuHistory);
   void FillHadmmNtuple(const G4Track& track,Int_t mm_num,Int_t cellNum);
   void FillHadmmNtuple();
   void FillBXDRAW(const G4Step* aStep);
   void FillAlcEdepInfo(const G4Track& track, G4int alc);
   void FillEdep(G4double edep, const G4ParticleDefinition* particleDefinition,
                 const G4int alc, const G4int IntExt, const G4double zpos,
                 Int_t cellNum, const Int_t trackID, const G4double weight);
   void FillAbsorberBkgrdNtuple(const G4Track& track);
   void WriteHadmmNtuple();
   
   void FillZpNtuple();
   void FillZpNtuple(const G4Track& track, Int_t zpnum);
   void WriteZpNtuple();
   
   void FillTarNtuple();
   void FillTarNtuple(const G4Track& track);
   void WriteTarNtuple();

   NumiTrajectory* GetParentTrajectory(G4int parentID);
   static NumiAnalysis* getInstance();
   
   void SetAlcEdepFlag(G4bool AlcEdep);
   void SetMuInAlcFlag(G4bool MuInAlc);
   
   void SetCount(G4int count);
   G4int GetCount();
   void SetEntry(G4int entry);
   G4int GetEntry();
   
private:
   std::string GetOFileName(std::string ifilename);
   
private:
   static NumiAnalysis* instance;
   
   G4double x;
   G4double y;
   G4double z;

   G4double noProtons;
   char asciiFileName[50], nuNtupleFileName[50], hadmmNtupleFileName[50], tarNtupleFileName[50];
   char zpNtupleFileName[50], bxdrawFileName[50];
  std::map<int, int> code;
   
   NumiDataInput* NumiData;
   
   TFile* hadmmNtuple;
   TFile* absbkgNtuple;
   TFile* nuNtuple;
   TFile* zpNtuple;
   TFile* tarNtuple;
   
   TTree* tree;
   TTree* hadmmtree;
   TTree* absbkgtree;
   TTree* zptree;
   TTree* tartree;
   
   data_t *g4data;
   hadmmtuple_t *g4hmmdata;
   draytupleMIB_t *g4draydataMIB;
   draytupleSPB_t *g4draydataSPB;
   absbkgtuple_t *g4absbkgdata;
   zptuple_t* g4zpdata; 
   target_exit_t *g4tardata;
   
   G4int fcount;
   G4int fentry;
   
   std::vector<bool> fAlcEdep_called;
   std::vector<bool> fMuInAlc;
   

};

#endif 
