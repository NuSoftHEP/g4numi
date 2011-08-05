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

//#include<map.h>

#include "/usr/include/c++/4.1.1/backward/map.h"

class G4Step;
class TFile;
class TTree;
class G4Track;
class data_t;
class hadmmtuple_t;
class zptuple_t;
class NumiAnalysis
{
public:

  NumiAnalysis();
  ~NumiAnalysis();

  void book();
  void finish();
  void FillNeutrinoNtuple(const G4Track& track);
  void FillHadmmNtuple(const G4Track& track,Int_t mm_num,Int_t cellNum);
  void FillHadmmNtuple();
  void FillBXDRAW(const G4Step* aStep);
  void FillAlcEdepInfo(const G4Track& track, G4int alc);
  void WriteHadmmNtuple(const G4Track* aTrack = 0);
  
  void FillZpNtuple();
  void FillZpNtuple(const G4Track& track, Int_t zpnum);
  void WriteZpNtuple();
  
  NumiTrajectory* GetParentTrajectory(G4int parentID);
  static NumiAnalysis* getInstance();

  void SetAlcEdepFlag(G4bool AlcEdep);

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
  char asciiFileName[50], nuNtupleFileName[50], hadmmNtupleFileName[50];
  char zpNtupleFileName[50], bxdrawFileName[50];
  map<int, int> code;

  NumiDataInput* NumiData;

  TFile* hadmmNtuple;
  TFile* nuNtuple;
  TFile* zpNtuple;

  TTree* tree;
  TTree* hadmmtree;
  TTree* zptree;

  data_t *g4data;
  hadmmtuple_t *g4hmmdata;
  zptuple_t* g4zpdata; 

  G4int fcount;
  G4int fentry;

  std::vector<bool> fAlcEdep_called;


};

#endif 
